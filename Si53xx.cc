
#include "Si53xx.h"

using namespace Si53xx;

Reg::Reg(RegAddr addr)
: addr      ( addr ),
  selfRstMsk( 0 ),
  value     ( 0 ),
  valid     ( false )
{
}

void
Reg::addUser(const Setting *s)
{
	auto it = s->getAddrs().begin();
	auto e  = s->getAddrs().end();
	int  i  = 0;

	while ( it != e ) {
		if ( *it == this->addr ) {
			break;
		}
		++it;
		++i;
	}
	if ( it == e ) {
		throw std::invalid_argument("Si53xx::Reg::addUser: setting does not list this register!");
	}
	for ( auto sit = this->users.begin(); sit != this->users.end(); ++sit ) {
		if ( (*sit)->getName() == s->getName() ) {
			throw std::invalid_argument("Si53xx::Reg::addUser: setting already part of this register!");
		}
	}
	this->users.push_back( s );

	// maybe we have to add to the self-clearing mask?
	if ( s->getAccess() == Access::SelfClear ) {
		uint8_t m = (s->toMask() >> i);
		this->selfRstMsk |= m;
	}
}


Setting::Setting(const Key &k, const char *name, const string &access, unsigned left, unsigned right, const RegAddrVec addrs)
: name      (name  ),
  access    (toAccess( access )),
  left      (left  ),
  right     (right ),
  addrs     (addrs ),
  cont      (true  )
{

	if ( right > 7 ) {
		throw std::out_of_range("Si53xx::Setting: right must be in 0..7");
	}

	if ( addrs.size() != (left >> 3) - (right >> 3) + 1 ) {
		throw std::out_of_range("Si53xx::Setting: left..right range does not match number of registers given");
	}
	if ( left >= 8*sizeof(Si53xx::ValType) ) {
		throw std::out_of_range("Si53xx::Setting: left must be < 64");
	}

	// is the address-range contiguous?
	auto i = addrs.begin();
	int  l = (*i) - 1;
	auto e = addrs.end();

	while ( i != e ) {
		if ( *i != l + 1 ) {
			cont = false;
			break;
		}
		l = *i;
		++i;
	}
}

Access
Si53xx::toAccess(const std::string &s)
{
	if ( s == "RO" ) {
		return Access::RO;
	} else if ( s == "RW" ) {
		return Access::RW;
	} else if ( s == "S" ) {
		return Access::SelfClear;
	}
	throw std::invalid_argument("Si53xx::toAccess: unable to convert string to 'Access' type");
}

Si53xx::Si53xx::Si53xx(I2CDriverShp drv, const std::vector<Setting> &settings)
: drv   ( drv ),
  pageNo( -1  )
{
	// create the array of all registers
	for (int i = 0; i <= 0xfff; i++ ) {
		this->regs.push_back( Reg( i ) );
	}

	// register settings
	for (auto it = settings.begin(); it != settings.end(); ++it) {
		SettingShp s = std::make_shared<Setting>( *it );
		// map the register
		auto rv = this->settings.insert( { s->getName().c_str(), s } );
		if ( ! rv.second ) {
			throw std::invalid_argument("Si53xx:Si3xx: setting with that name already exists");
		}

		for ( auto a = s->getAddrs().begin(); a != s->getAddrs().end(); ++a ) {
			this->regs.at( *a ).addUser( s.get() );
		}
	}
}

void
Si53xx::Si53xx::readRegs(unsigned offset, unsigned n, uint8_t *buf)
{
	while ( n > 0 ) {
		const Reg &r = this->regs.at( offset );
		int        v = r.getValue();
		uint8_t    m = r.getSelfRstMsk();

		if ( v >= 0 && ( ( v & m ) == 0 ) ) {
			// we have a valid cache and all self-clearing bits are reset
			*buf++ = v;
			offset++;
			n++;
		} else {
			this->readRange( offset, n, buf );
			while ( n > 0 ) {
				Reg &r = this->regs.at( offset );
				r.update( *buf );
				buf++;
				offset++;
				n--;
			}
		}
	}
}

void
Si53xx::Si53xx::writeRegs(unsigned offset, unsigned n, uint8_t *buf)
{
	this->writeRange( offset, n, buf );
	while ( n > 0 ) {
		Reg &r = this->regs.at( offset );
		r.update( *buf );
		buf++;
		offset++;
		n--;
	}
}

#define PG_SHFT 8
#define PG_MASK ( ( 1 << PG_SHFT ) - 1 )
#define PG_REG  0x01

void
Si53xx::Si53xx::setPage(unsigned offset)
{
	uint8_t pg = (offset >> PG_SHFT);
	if ( this->pageNo != pg ) {
		this->drv->wr( PG_REG, 1, &pg );
		this->pageNo = pg;
		// update all copies
		for ( unsigned pgReg = PG_REG; pgReg < this->regs.size(); pgReg += (1<<PG_SHFT) ) {
			this->regs.at( pgReg ).update( pg );
		}
	}
}

void
Si53xx::Si53xx::readRange(unsigned offset, unsigned n, uint8_t *buf)
{
	this->rwRange( true, offset, n, buf );
}

void
Si53xx::Si53xx::rwRange(bool rd, unsigned offset, unsigned n, uint8_t *buf)
{
	while ( n > 0 ) {
		uint8_t  pgoff    = offset & PG_MASK;

		// do we have to cross pages?
		unsigned pgRemain = ( offset | PG_MASK ) - offset + 1;

		if ( pgRemain > n ) {
			pgRemain = n;
		}

		this->setPage( offset );
		if ( rd ) {
			this->drv->rd( pgoff, pgRemain, buf );
		} else {
			this->drv->wr( pgoff, pgRemain, buf );
		}

		offset += pgRemain;
		buf    += pgRemain;
		n      -= pgRemain;
	}
}

void
Si53xx::Si53xx::writeRange(unsigned offset, unsigned n, uint8_t *buf)
{
	this->rwRange( false, offset, n, buf );
}

Si53xx::Si53xx::ValType
Si53xx::Si53xx::get(const std::string &k)
{
	return this->get( this->settings.at( k.c_str() ) );
}

Si53xx::Si53xx::ValType
Si53xx::Si53xx::get(SettingShp s)
{
	uint8_t    buf[sizeof(ValType) + 1];

	int        len = s->getAddrs().size();

	if ( s->isContiguous() ) {
		this->readRegs( s->getAddrs()[0], len, buf );
	} else {
		for ( int i = 0; i < len; ++i ) {
			this->readRegs( s->getAddrs()[i], 1, & buf[i] );
		}
	}

	// regs are little-endian
	uint64_t v = 0;
	for ( int i = len - 1; i >= 0; --i ) {
		v = (v << 8 ) | buf[i];
	}
	v &= s->toMask();
	return v >> s->getRight();
}

void
Si53xx::Si53xx::set(SettingShp s, ValType v)
{
	uint64_t m  = s->toMask();
	uint64_t ov = 0;

	uint64_t max = ~((uint64_t)0);
	int      len = s->getAddrs().size();
	uint8_t  buf[sizeof(ValType) + 1];


	max >>= s->getRight();

	if ( v > max ) {
		throw std::invalid_argument("Si53xx::Si53xx::set(): value too big");
	}

	v <<= s->getRight();

	bool getFirst = ((s->getRight()& 7) != 0);
	bool getLast  = ((s->getLeft() & 7) != 0);

	if ( getFirst && getLast && s->isContiguous() && (len == 2) ) {
		this->readRegs( s->getAddrs()[0], 2, buf );
		ov = ( buf[1] << 8 ) | buf[0];
	} else {
		ov = 0;
		if ( getLast ) {
			this->readRegs( s->getAddrs()[len - 1], 1, buf );
			ov = *buf;
			ov <<= (len - 1);
		}
		if ( getFirst ) {
			this->readRegs( s->getAddrs()[0], 1, buf );
			ov |= *buf;
		}
	}

	v = ( v & m ) | ( ov & ~m );

	for ( int i = 0; i < len; i++ ) {
		buf[i]  = (uint8_t)v;
		v     >>= 8;
	}

	if ( s->isContiguous() ) {
		this->writeRegs( s->getAddrs()[0], len, buf );
	} else {
		for ( int i = 0; i < len; i++ ) {
			this->writeRegs( s->getAddrs()[i], 1, & buf[i] );
		}
	}
}

void
Si53xx::Si53xx::set(const std::string &k, ValType v)
{
	return this->set( this->settings.at( k.c_str() ), v );
}

void
Si53xx::Si53xx::readCSV(FILE *f)
{
char     buf[2048];
int      off  = -1;
uint8_t  rbuf[256];
int      idx  = 0;

	while ( fgets(buf, sizeof(buf), f) ) {
		unsigned a,v;
		if ( 2 == sscanf(buf, "%i,%i", &a, &v) ) {
			if ( a >= this->regs.size() ) {
				throw std::runtime_error("readCSV: address out of range");
			}
			if ( v > 255 ) {
				throw std::runtime_error("readCSV: value out of range");
			}
			if ( (a == off + idx) && idx < sizeof(rbuf)  ) {
				rbuf[idx] = v;
				idx ++;
			} else {
				// short circuit idx == 0 case
				if ( idx > 0 ) {
					this->writeRegs( off, idx, rbuf );
				}
				rbuf[0] = v;
				idx     = 1;
				off     = a;
			}
		}
	}
	// mop up
	if ( idx > 0 ) {
		this->writeRegs( off, idx, rbuf );
	}
}
