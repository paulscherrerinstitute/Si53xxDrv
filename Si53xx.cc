
#include "Si53xx.h"
#include <stdarg.h>
#include <time.h>
#include "RAIIfeil.h"
#include <math.h>

using namespace Si53xx;

Reg::Reg(RegAddr addr)
: addr      ( addr ),
  selfRstMsk( 0 ),
  value     ( 0 ),
  valid     ( false )
{
}

// Intended to be used locally; not particularly efficient.
// For small strings holding Settings names.
class FMT {
	private:
		char buf[64];
	public:
		FMT(const char *fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			int st = vsnprintf(this->buf, sizeof(this->buf), fmt, ap);
			va_end(ap);
			if ( st >= sizeof(this->buf) ) {
				throw std::range_error("FMT buffer not big enough");
			}
		}

		FMT(const std::string &fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			int st = vsnprintf(this->buf, sizeof(this->buf), fmt.c_str(), ap);
			va_end(ap);
			if ( st >= sizeof(this->buf) ) {
				throw std::range_error("FMT buffer not big enough");
			}
		}


		FMT(const FMT &orig)
		{
			strcpy(this->buf, orig.buf);
		}

		FMT & operator=(const FMT &orig)
		{
			::strcmp( this->buf, orig.buf );
			return *this;
		}

		const char *operator*() const {
			return this->buf;
		}
};

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
: Setting( k, name, toAccess( access ), left, right, addrs )
{
}

Setting::Setting(const Key &k, const char *name, Access access, unsigned left, unsigned right, const RegAddrVec addrs)
: name      ( name   ),
  access    ( access ),
  left      ( left   ),
  right     ( right  ),
  addrs     ( addrs  ),
  cont      ( true   )
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
	if ( s == "R/O" ) {
		return Access::RO;
	} else if ( s == "R/W" ) {
		return Access::RW;
	} else if ( s == "S" ) {
		return Access::SelfClear;
	}
	throw std::invalid_argument("Si53xx::toAccess: unable to convert string to 'Access' type");
}

Si53xx::Si53xx::Si53xx(I2cDriverShp drv, const SettingVec &settings, const Si53xxParams &p)
: drv              ( drv         ),
  pageNo           ( -1          ),
  params           ( p           ),
  finFreq          ( 0           ),
  refFreq          (    48000000 ),
  // undocumented; observed min/max as produced by CBPro (while maintaining BW params)
  vcoMinFreq       ( 13280000000 ),
  vcoMaxFreq       ( 13600000000 ),
  // undocumented; observed min/max as produced by CBPro (while maintaining BW params)
  pfdMinFreq       (     1770000 ),
  pfdMaxFreq       (     2000000 )
{
	// create the array of all registers
	for (int i = 0; i <= 0xfff; i++ ) {
		this->regs.push_back( Reg( i ) );
	}

	// register settings
	for (auto it = settings.begin(); it != settings.end(); ++it) {
		SettingShp s = *it;
		// map the register
		auto rv = this->settings.insert( { s->getName().c_str(), s } );
		if ( ! rv.second ) {
			throw std::invalid_argument("Si53xx:Si3xx: setting with that name already exists");
		}

		for ( auto a = s->getAddrs().begin(); a != s->getAddrs().end(); ++a ) {
			this->regs.at( *a ).addUser( s.get() );
		}
	}

	if ( 0 ) {
		// debugging
		for (auto it = this->settings.begin(); it != this->settings.end(); ++it ){
			auto kv = *it;
			printf("in map %p: %s\n", kv.first, kv.first);
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
			n--;
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
	return this->get( this->at( k ) );
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
	return this->set( this->at( k ), v );
}


void
Si53xx::Si53xx::readCSV(const std::string &fn, bool noAutoPreamble)
{
	this->readCSV( fn.c_str(), noAutoPreamble );
}

void
Si53xx::Si53xx::readCSV(const char *fn, bool noAutoPreamble)
{
	this->readCSV( *RAIIfeil( fn ), noAutoPreamble );
}


void
Si53xx::Si53xx::readCSV(FILE *f, bool noAutoPreamble)
{
char     buf[2048];
uint8_t  rbuf[256];
int      off          = -1;
int      idx          = 0;
bool     preambleSent = false;

	if ( ! noAutoPreamble && ! isPLLOff() ) {
		this->sendPreamble();
		preambleSent = true;
	}

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

	if ( ! noAutoPreamble && isPLLOff() && preambleSent ) {
		this->sendPostamble();
	}
}

void
Si53xx::Si53xx::dumpCSV(const std::string &f)
{
	this->dumpCSV( f.c_str() );
}

void
Si53xx::Si53xx::dumpCSV(const char *f)
{
	if ( f ) {
		this->dumpCSV( *RAIIfeil( f, "w+" ) );
	} else {
		this->dumpCSV();
	}
}

void
Si53xx::Si53xx::dumpCSV(FILE *f)
{
uint8_t buf[4096];
	this->readRegs(0, sizeof(buf), buf);
	for ( int i = 0; i < sizeof(buf); i++ ) {
		if ( fprintf(f, "0x%04X,0x%02X\n", i, buf[i]) < 0 ) {
			throw std::runtime_error("Si53xx::dumpCSV(): file write error");
		}
	}	
}

void
Si53xx::Si53xx::dumpSettings(const std::string &f)
{
	this->dumpSettings( f.c_str() );
}

void
Si53xx::Si53xx::dumpSettings(const char *f)
{
	if ( f ) {
		this->dumpSettings( *RAIIfeil( f, "w+" ) );
	} else {
		this->dumpSettings();
	}
}

void
Si53xx::Si53xx::dumpSettings(FILE *f)
{
Settings::iterator it  = this->settings.begin();
Settings::iterator ite = this->settings.end();
	while ( it != ite ) {
		const char *k( (*it).first );
		ValType     vMine;
		try {
			vMine = this->get( k );
		} catch ( std::runtime_error &e ) {
			// skip unininitialized/untouched regs when using the
			// dummy driver...
			++it;
			continue;
		}
		fprintf(f, "%40s: 0x%lx\n", k, vMine);
		++it;
	}
}

void
Si53xx::Si53xx::getDivider(DividerSettings &s, Si53xx::ValType *nump, Si53xx::ValType *denp)
{
	*nump = get( s.num );
	*denp = get( s.den );
}

double
Si53xx::Si53xx::getDivider(DividerSettings &s)
{
ValType n, d;
	getDivider( s, &n, &d );
	return (double)n / (double)d;
}

void
Si53xx::Si53xx::setDivider(DividerSettings &s, Si53xx::ValType num, Si53xx::ValType den)
{
	if ( s.requirePLLOff && ! isPLLOff() ) {
		throw std::logic_error("Si53xx::setDivider: cannot set divider while PLL is running");
	}
	set( s.num,    num );
	set( s.den,    den );

	if ( 'P' == s.prefix[0] || 0 == strcmp(s.prefix.c_str(), "M") ) {
		if ( 'P' == s.prefix[0] ) {

			this->set( *FMT( s.prefix + "_FRACN_EN"), (den > 1 ? 1 : 0 ) );

			if ( den > 1 ) {
				this->andmsk( "PDIV_FRACN_CLK_DIS", ~ ( 1 << s.idx ) );
			} else {
				this->ormsk ( "PDIV_FRACN_CLK_DIS",   ( 1 << s.idx ) );
			}
		} else {
			this->set( "M_FRAC_EN",         ( den > 1 ? 1 : 0 ) );
			this->set( "FRACN_CLK_DIS_PLL", ( den > 1 ? 0 : 1 ) );
		}
	}

	if ( 'N' == s.prefix[0] ) {
		ValType m = (1 << s.idx);

		// always use fractional mode for N
		this->andmsk( "N_PIBYP",          ~ m );
		this->ormsk ( "N_CLK_TO_OUTX_EN",   m );
		this->ormsk ( "N_PDNB",             m );
		this->andmsk( "N_CLK_DIS",        ~ m );
	}

	set( s.update, 1   );
}

void
Si53xx::Si53xx::setDivider(DividerSettings &s, double val)
{
	uint64_t n, d;
	uint64_t maxn, maxd;


	maxn = (1 << (s.num->getLeft() - s.num->getRight())); maxn |= (maxn - 1);
	maxd = (1 << (s.den->getLeft() - s.den->getRight())); maxd |= (maxd - 1);

	ratapp( val, maxn, maxd, &n, &d );

	setDivider( s, n, d );
}

Si53xx::Si53xx::DividerSettings
Si53xx::Si53xx::getDividerSettings(const char *prefix, int idx)
{
	DividerSettings s;
	s.prefix        = (idx >= 0 ? *FMT( "%s%d", prefix, idx ) : prefix);
	s.idx           = idx;
	s.num           = this->at( *FMT( s.prefix + "_NUM"   ) );
	s.den           = this->at( *FMT( s.prefix + "_DEN"   ) );
	s.update        = this->at( *FMT( s.prefix + "_UPDATE") );
	s.requirePLLOff = (s.prefix[0] != 'N');
	return s;
}

Si53xx::Si53xx::DividerSettings
Si53xx::Si53xx::getNDividerSettings(unsigned idx)
{
	return getDividerSettings( "N", idx );
}

Si53xx::Si53xx::DividerSettings
Si53xx::Si53xx::getMDividerSettings()
{
	return getDividerSettings( "M" );
}

Si53xx::Si53xx::DividerSettings
Si53xx::Si53xx::getPDividerSettings(unsigned idx)
{
	return getDividerSettings( "P", idx );
}

Si53xx::Si53xx::DividerSettings
Si53xx::Si53xx::getMXAXBDividerSettings()
{
	return getDividerSettings( "MXAXB" );
}

void
Si53xx::Si53xx::getNDivider(unsigned idx, Si53xx::ValType *nump, Si53xx::ValType *denp)
{
	DividerSettings s(this->getNDividerSettings( idx ) );
	getDivider( s, nump, denp );
}

double
Si53xx::Si53xx::getNDivider(unsigned idx)
{
	DividerSettings s(this->getNDividerSettings( idx ) );
	return getDivider( s );
}

void
Si53xx::Si53xx::setNDivider(unsigned idx, Si53xx::ValType num, Si53xx::ValType den)
{
	DividerSettings s(this->getNDividerSettings( idx ) );
	setDivider( s, num, den );
}

void
Si53xx::Si53xx::setNDivider(unsigned idx, double  val)
{
	DividerSettings s(this->getNDividerSettings( idx ) );
	setDivider( s, val );
}

void
Si53xx::Si53xx::getPDivider(unsigned idx, Si53xx::ValType *nump, Si53xx::ValType *denp)
{
	DividerSettings s(this->getPDividerSettings( idx ) );
	getDivider( s, nump, denp );
}

double
Si53xx::Si53xx::getPDivider(unsigned idx)
{
	DividerSettings s(this->getPDividerSettings( idx ) );
	return getDivider( s );
}

void
Si53xx::Si53xx::setPDivider(unsigned idx, Si53xx::ValType num, Si53xx::ValType den)
{
	DividerSettings s(this->getPDividerSettings( idx ) );
	setDivider( s, num, den );
}

void
Si53xx::Si53xx::setPDivider(unsigned idx, double  val)
{
	DividerSettings s(this->getPDividerSettings( idx ) );
	setDivider( s, val );
}

void
Si53xx::Si53xx::getMDivider(Si53xx::ValType *nump, Si53xx::ValType *denp)
{
	DividerSettings s(this->getMDividerSettings() );
	getDivider( s, nump, denp );
}

double
Si53xx::Si53xx::getMDivider()
{
	DividerSettings s(this->getMDividerSettings() );
	return getDivider( s );
}

void
Si53xx::Si53xx::setMDivider(Si53xx::ValType num, Si53xx::ValType den)
{
	DividerSettings s(this->getMDividerSettings() );
	setDivider( s, num, den );
}

void
Si53xx::Si53xx::setMDivider(double  val)
{
	DividerSettings s(this->getMDividerSettings() );
	setDivider( s, val );
}

void
Si53xx::Si53xx::getMXAXBDivider(Si53xx::ValType *nump, Si53xx::ValType *denp)
{
	DividerSettings s(this->getMXAXBDividerSettings() );
	getDivider( s, nump, denp );
}

double
Si53xx::Si53xx::getMXAXBDivider()
{
	DividerSettings s(this->getMXAXBDividerSettings() );
	return getDivider( s );
}

void
Si53xx::Si53xx::setMXAXBDivider(Si53xx::ValType num, Si53xx::ValType den)
{
	DividerSettings s(this->getMXAXBDividerSettings() );
	setDivider( s, num, den );
}

void
Si53xx::Si53xx::setMXAXBDivider(double  val)
{
	DividerSettings s(this->getMXAXBDividerSettings() );
	setDivider( s, val );
}

/* multiply two 64bit numbers and detect overflow; does not have
 * to be efficient; we use this rarely.
 */
static int mu64(uint64_t *p, uint64_t a, uint64_t b)
{
	// break into upper and lower parts; we'll compute the
	// partial products
	//   a = al + 2^M ah
	//   b = bl + 2^M bh
	// thus
	//   a*b = al*bl + 2^M (al*bh + ah*bl) + 2^(2M) ah*bh
	// if we chose M=32 (for uint64_t) then
	//  -> overflow if ah*bh != 0
	//  -> overflow if the al*bh or ah*bh >= 2^M
	//  -> overflow if adding the partial products overflows
	uint64_t al = a & 0xffffffff;
	uint64_t bl = b & 0xffffffff;
	uint64_t ah = (a >> 32) & 0xffffffff;
	uint64_t bh = (b >> 32) & 0xffffffff;
	uint64_t t;
	uint64_t r;

	if ( ah ) {
		if ( bh ) {
			// If both upper parts are nonzero then we have an overflow
			return -1;
		}
		t = ah*bl;
	} else {
		t = al*bh;
	}

	// upper*lower must be < 1^32
	if ( t & 0xfffffff0000000ULL ) {
		return -1;
	}
	r = (t << 32);

	t = al * bl + r;
	if ( t < r ) {
        // addition overflow
		return -1;
	}
	*p = t;
	return 0;
}


static void
Si53xx::ratapp(double x, uint64_t maxNum, uint64_t maxDen, uint64_t *nump, uint64_t *denp)
{
uint64_t n2 = 0;
uint64_t n1 = 1;
uint64_t d2 = 1;
uint64_t d1 = 0;

uint64_t n, d, a, p;
	
	/* Keep computing convergents until we hit a max.
	 * Compute 1/x actually (same result just with num/den switched);
	 * which allows us to do the x != 0 test at the top of the loop
	 */
	while ( x != 0.0 ) {
		x   = 1.0/x;
		a   = (uint64_t)x;

		// compute n = a*n1 + n2

		// try a * n1 checking for overflow
		if ( mu64( &p, a, n1 ) ) {
			break;
		}
		if ( ( n = p + n2 ) < n2 ) {
			// addition overflow
			break;
		}

		// compute d = a*d1 + d2

		// try a * d1 checking for overflow
		if ( mu64( &p, a, d1 ) ) {
			break;
		}
		if ( (d = p + d2) < d2 ) {
			// addition overflow
			break;
		}

		// check requested limits
		if ( n > maxNum || d > maxDen ) {
			break;
		}
		n2 = n1; n1 = n;
		d2 = d1; d1 = d;
		x -= a;
	}
	/* We started out approximating 1/x */
	*nump = d1;
	*denp = n1;
}

void
Si53xx::Si53xx::sendPreamble()
{
	if ( isPLLOff() ) {
		throw std::logic_error("Si53xx::sendPreamble: preamble has already been sent?");
	}
	// send magical, undocumented values :-(
	set( "AMBLE0", 0xC0 );
	set( "AMBLE1", 0x00 );
	set( "AMBLE2", 0x01 );
	struct timespec ms300 = { tv_sec: 0, tv_nsec: 300000000 };
	while ( nanosleep( &ms300, 0 ) )
		;
}

void
Si53xx::Si53xx::sendPostamble()
{
	if ( ! isPLLOff() ) {
		throw std::logic_error("Si53xx::sendPostamble: postamble has already been sent?");
	}
	set( "BW_UPDATE_PLL", 1 );
	set( "SOFT_RST_ALL",  1 );
	// send magical, undocumented values :-(
	set( "AMBLE2", 0x00 );
	set( "AMBLE0", 0xC3 );
	set( "AMBLE1", 0x02 );
}

bool
Si53xx::Si53xx::isPLLOff()
{
	return get("AMBLE2") != 0x00;
}

Si53xx::SettingShp
Si53xx::Si53xx::at(const string &k)
{
	try {
		return settings.at( k.c_str() );
	} catch ( std::exception &e ) {
		throw std::runtime_error("Si53xx::at(): key '" + k + "' not found");
	}
}

Si53xx::SettingShp
Si53xx::Si53xx::at(const char *ch)
{
	return settings.at( ch );
}

static void chkAlt(unsigned idx, bool alt, const std::string pre)
{
	if ( alt && (idx != 0) && (idx != 9) ) {
		throw std::invalid_argument(pre + ": 'A' selection only valid for outputs 0 and 9");
	}
}

void
Si53xx::Si53xx::setOutputMux(unsigned idx, unsigned nDivider)
{
	// CBP shows out0a and out9a sharing the N divider with out0
	// and out9, respectively. We'll enforce that even though there
	// are separate registers...
	set( *FMT( "OUT%u_MUX_SEL", idx ) , nDivider ); 
	if ( 0 == idx || 9 == idx ) {
		set( *FMT( "OUT%uA_MUX_SEL", idx ) , nDivider ); 
	}
}

unsigned
Si53xx::Si53xx::getOutputMux(unsigned idx, bool alt)
{
	chkAlt( idx, alt, "Si53xx::getOutputMux");

	// should only be needed for diagnostics; the 'A' outputs must be retrieved manually
	return get( *FMT( "OUT%u%s_MUX_SEL", idx, (alt ? "A" : "") ) );
}

void
Si53xx::Si53xx::setOutput(unsigned idx, OutputConfig drvCfg, unsigned rdiv, bool alt)
{
	chkAlt( idx, alt, "Si53xx::SetOutput" );
	FMT pre( "OUT%u%s_", idx, (alt ? "A" : "") );

	set( *FMT( "%sSYNC_EN",   *pre ), 1 );

	ValType outEn = ( OutputConfig::OFF != drvCfg );

	if ( ! outEn ) {
		set( *FMT( "%sOE",       *pre ),   0 );
		return;
	}

	ValType vsel = 0;
	switch ( drvCfg ) {
		case OutputConfig::LVDS25: vsel++; // fall through
		case OutputConfig::LVDS18: vsel++; // fall through
		case OutputConfig::LVDS33:
			set( *FMT( "%sFORMAT",  *pre ), 1 );
			break;
		default:
			break;
		// case LVCMOS18, LVCMOS25, LVCMOS33: handle here
	}

	set( *FMT( "%sVDD_SEL_EN",   *pre ), 1    );
	set( *FMT( "%sVDD_SEL",      *pre ), vsel );

	if ( 0 == get( *FMT( "R%d%s_REG", idx, (alt ? "A" : "") ) ) ) {
		this->setRDivider( idx, 2, alt );
	}

	// changing the PDN state seems to always reset
	// OE, so we write OE last. BTW: changing PDN
	// also resets the RDIV to 0, apparently.

	set( *FMT( "%sPDN",      *pre ), ! outEn );

	setRDivider( idx, rdiv, alt );

	set( *FMT( "%sOE",       *pre ),   outEn );
}
			
void
Si53xx::Si53xx::showDiff(Si53xx *other, const char *fn)
{
	if ( fn ) {
		this->showDiff( other, *RAIIfeil( fn, "w+" ) );
	} else {
		this->showDiff( other );
	}
}

void
Si53xx::Si53xx::showDiff(Si53xx *other, FILE *f)
{
Settings::iterator it  = this->settings.begin();
Settings::iterator ite = this->settings.end();
	while ( it != ite ) {
		const char *k( (*it).first );
		ValType     vMine;
		try {
			vMine = this->get( k );
		} catch ( std::runtime_error &e ) {
			// skip unininitialized/untouched regs when using the
			// dummy driver...
			++it;
			continue;
		}
		ValType     vOthr = other->get( k );
		if ( vMine != vOthr ) {
			fprintf(f, "%40s: 0x%lx => 0x%lx\n", k, vOthr, vMine);
		}
		++it;
	}
}

void
Si53xx::Si53xx::selInput(unsigned inp)
{
	if ( inp > 3 ) {
		throw std::invalid_argument("Si53xx::selInput: invalid input");
	}
	ValType v = (1 << inp);
	this->set( "IN_SEL_REGCTRL", 1 );
	this->ormsk( "IN_EN",                 v );
	this->ormsk( "INX_TO_PFD_EN",         v );
	// CBP sets this
	this->ormsk( "IN_CLK_VAL_PWR_UP_DIS", v );
	this->set( "IN_SEL", inp );
	if ( inp != 3 ) {
		this->set( "ZDM_IN_SEL", inp );
	}
}

struct DivParm {
	Si53xx::Si53xx::ValType num, den;
	double                  r;

	DivParm()
	: num(0), den(0), r(0.0)
	{
	}
};

class ZDMParms;

typedef std::shared_ptr<ZDMParms> ZDMParmsShp;

class ZDMParms : public Si53xx::Si53xx::PLLParms {
protected:
	unsigned nidx;
public:
	Si53xx::Si53xx::DivParm N;

	ZDMParms(const Key &k, Si53xx::Si53xx *obj, unsigned pidx, unsigned nidx)
	: Si53xx::Si53xx::PLLParms( k, obj, pidx ),
	  nidx( nidx )
	{
		if ( pidx > 2 ) {
			throw std::invalid_argument("Si53xx::ZDMParms: invalid input (must be < 3)");
		}
	}

	virtual void get()
	{
		Si53xx::Si53xx::PLLParms::get();

		obj->getNDivider( nidx,  &N.num,     &N.den );
	}

	virtual void set()
	{
		Si53xx::Si53xx::PLLParms::set();
		if ( N.den > 0 ) {
			obj->setNDivider    ( nidx,   N.num,      N.den );
		} else {
			obj->setNDivider    ( nidx,   N.r     );
		}
	}

	virtual Si53xx::Si53xx::PLLParmsShp clone()
	{
		return std::make_shared<ZDMParms>(*this);
	}
	
	static ZDMParmsShp create(Si53xx::Si53xx *obj, unsigned pidx, unsigned nidx)
	{
		return std::make_shared<ZDMParms>(Key(), obj, pidx, nidx);
	}

	virtual void validate()
	{
		PLLParms::validate();
		if ( 1 != N.den ) {
			throw std::invalid_argument("Si53xx::ZDMParms::validate: min. N not integer");
		}
		if ( 1 != P.den ) {
			throw std::invalid_argument("Si53xx::ZDMParms::validate: min. P not integer");
		}
		// ignore uint64 overflow
		if ( P.num * N.num * 2 * M.den != 5 * M.num ) {
			throw std::invalid_argument("Si53xx::ZDMParms::validate: invalid divider combination for ZDM mode!");
		}
	}
};


Si53xx::Si53xx::PLLParms::PLLParms(const Key &k, Si53xx *obj, unsigned pidx)
: obj(obj), pidx(pidx), fin(0)
{
	if ( pidx > 3 ) {
		throw std::invalid_argument("Si53xx::PLLParms: invalid input selection");
	}
}

void
Si53xx::Si53xx::PLLParms::get()
{
	obj->getPDivider    ( pidx,  &P.num,     &P.den );
	obj->getMDivider    (        &M.num,     &M.den );
	obj->getMXAXBDivider(        &MXAXB.num, &MXAXB.den );
	this->fin = obj->finFreq;
}

void
Si53xx::Si53xx::PLLParms::set()
{
	if ( P.den > 0 ) {
printf("Setting P%d %ld/%ld\n", pidx, P.num, P.den);
		obj->setPDivider    ( pidx,   P.num,      P.den );
	} else {
printf("Setting P%d %g\n", pidx, P.r);
		obj->setPDivider    ( pidx,   P.r );
	}
	if ( M.den > 0 ) {
printf("Setting M %ld/%ld\n", M.num, M.den);
		obj->setMDivider    (         M.num,      M.den );
	} else {
printf("Setting M %g\n",  M.r);
		obj->setMDivider    (         M.r     );
	}
	if ( MXAXB.den > 0 ) {
printf("Setting MXAXB %ld/%ld\n", MXAXB.num, MXAXB.den);
		obj->setMXAXBDivider(         MXAXB.num,  MXAXB.den );
	} else {
printf("Setting MXAXB %g\n", MXAXB.r);
		obj->setMXAXBDivider(         MXAXB.r );
	}

	obj->set( "IN_SEL", pidx );

	// assume OOF reference is XAXB
    double oofRefFreq = (double)obj->refFreq/(double)(1 << obj->get("OOFXO_DIV_SEL"));
printf("fin %g, OOF ref freq %g\n", fin, oofRefFreq);

    double oofDivLd   = round( log2( (double)fin/oofRefFreq ) );

    ValType  oofDiv   = (1 << (ValType )oofDivLd);
    
	obj->set( *FMT( "OOF%d_DIV_SEL", pidx ), oofDivLd );

	double oofRatio   = ( (double)(1<<24) ) * (double)fin / (double)oofDiv / oofRefFreq;
printf("OOF ratio %g\n", oofRatio);

	obj->set( *FMT( "OOF%d_RATIO_REF", pidx ), oofRatio );

	double fpfd       = (double) fin / P.get(); 
    double fvco       = (double) fin / P.get() * 5.0 * M.get();
printf("fpfd %g, fvco %g\n", fpfd, fvco);

	// magial time constant: 0.01seconds (extrapolated from cbpro)
    double holdCyc    = ((double)(1<<24)) / 0.01 / fpfd;

	obj->set( "HOLD_15M_CYC_COUNT", (ValType)holdCyc);

	// more magic:
	obj->set( "OUT_MAX_LIMIT_LMT",  (ValType)(fvco/0.7324384635355068) );
	obj->set( "HOLD_SETTLE_TARGET", (ValType)(fvco/17.89784753839401 ) );

	obj->finFreq = this->fin;
}

void
Si53xx::Si53xx::PLLParms::validate()
{
	double fpfd = fin / P.get();

	if ( fpfd  < obj->pfdMinFreq ) {
		throw std::invalid_argument("Si53xx::PLLParms::validate: min. PFD violation by fin and or P divider");
	}
	if ( fpfd  > obj->pfdMaxFreq ) {
		throw std::invalid_argument("Si53xx::PLLParms::validate: max. PFD violation by fin and or P divider");
	}

	double fvco = fpfd * M.get() * 5.0;

	if ( fvco  < obj->vcoMinFreq ) {
		throw std::invalid_argument("Si53xx::PLLParms::validate: min. VCO violation by M divider");
	}
	if ( fvco  > obj->vcoMaxFreq ) {
		throw std::invalid_argument("Si53xx::PLLParms::validate: max. VCO violation by M divider");
	}
}


// Program the PLL for ZDM mode using integer dividers.
// If no all-integer configuration is found then this simple
// algorithm does not work.
//
// The following problem would have to be solved:
//
//  fvco_min <= fin * 2 * Nden/Nnum  <= fvco_max
//
//  fpfd_min <= fin * Pden/Pnum      <= fpfd_max
//
//     Mnum/Mden = 2/5 * Pnum * Nnum / Pden / Nden
//
//  with all the numbers remaining within their valid range
//

void
Si53xx::Si53xx::setZDM(uint64_t hz, unsigned inp, unsigned ndiv)
{
	ZDMParmsShp np = ZDMParms::create( this, inp, ndiv );

	ValType p,n, p5 = 0, n5 = 0, r = 2;

	// try to find integer dividers with the constraint that fvco remain
	// within the 'known' range.
	//   fvco = fin / P * 5 * M      (fixed 5 divider present)
	//   fout = fin (zdm) = fvco/2/N (min. R divider is 2    )
	//
	//   thus for integer ratios:  5*M = 2 * N * P

	n  = this->vcoMaxFreq/hz/r; 
	if ( hz*n*r < this->vcoMinFreq ) {
		throw std::runtime_error("Si53xx::setZDM: unable to find all-integer PLL configuration");
		// frequency probably too high; must resort to fractional N
	}

	// see if we find a p divisible by 5; this may work for larger fin/fpfd ratios
	// where we are less likely to find an N divisible by 5.
	for ( p = (hz/this->pfdMinFreq); (hz <= this->pfdMaxFreq * p) && 0 == p5; p-- ) {
		// record a valid integer P
		np->P.num = p;
		np->P.den = 1;
		if ( 0 == ( np->P.num % 5 ) ) {
			// 5 divides P => M = 2*N*p5
			p5 = p/5;
			np->M.num = 2*n*p5;
			np->M.den = 1;
			np->N.num = n;
			np->N.den = 1;
		}
	}

	if ( 0 == np->P.den ) {
		throw std::runtime_error("Si53xx::setZDM: unable to find all-integer PLL configuration");
		// would have to resort to fractional P
	}

	if ( 0 == p5 ) {
		// P not divisible by 5; try to find an N. Better for small fin
		while ( hz*n*r >= this->vcoMinFreq && 0 == n5 ) {
			// record a valid integer N
			np->N.num = n;
			np->N.den = 1;
			if ( 0 == ( n % 5 ) ) {
				// found one
				n5        = n/5;
				np->N.num = n;
				np->N.den = 1;
				// np->P.den != 0 was tested above
				np->M.num = r * n5 * np->P.num;
				np->M.den = 1;
			}
			n--;
		}
	}

	// at this point we do have integer n and p but not necessarily a n5 or a p5

	if ( 0 == p5 && 0 == n5 ) {
		/* no multiple of 5 found; must use fractional divider;
		 * use on M. Note the 0 == p5 test also covers the
		 * case when no integer P divider at all is found...
		 */
		np->M.num = r * np->N.num * np->P.num;
		np->M.den = 5;
	}

	// program the reference divider
	np->MXAXB.r = (double)hz * (double)np->N.num/(double)np->N.den * (double)r / this->refFreq;

	np->fin = hz;

	this->setPLL( np );

	this->selInput( inp );
}

void
Si53xx::Si53xx::setPLL(Si53xx::Si53xx::PLLParmsShp p)
{
	p->validate();

	PLLParmsShp orig = p->clone();
	orig->get();
	try {
		p->set();
	} catch ( std::exception &e ) {
		// if the very first modification failed we may not be able to restore
		try {
			// try to restore original settings
			orig->set();
		} catch ( std::exception &e ) {
		}
		throw;
	}
}

void
Si53xx::Si53xx::setZDM(bool ena)
{
	if ( ( 0 == this->finFreq ) and ena ) {
		throw std::invalid_argument("Si53xx::setZDM: must set an input frequency before you can engage ZDM");
	}
	this->set( "ZDM_EN", (ena ? 1 : 0 ) );

	if ( ena ) {
		ValType v = ( 1 << 3 );
		this->ormsk( "IN_EN",                 v );
		// CBP sets this
		this->ormsk( "IN_CLK_VAL_PWR_UP_DIS", v );
		this->ormsk( "INX_TO_PFD_EN",         v );
	}

	// assume out9a; aka #11
	ValType outx = ( ena ? (1<<11) : 0 );
	if ( ena ) {
		this->ormsk( "OUTX_ALWAYS_ON", outx );
	} else {
		this->andmsk( "OUTX_ALWAYS_ON", ~outx );
	}
}

uint64_t
Si53xx::Si53xx::getZDM()
{
	return this->get( "ZDM_EN" ) ? this->finFreq : 0;
}

unsigned
Si53xx::Si53xx::getRDivider(unsigned idx, bool alt)
{
	chkAlt(idx, alt, "Si53xx::getRDivider");
	if ( this->get( *FMT( "OUT%u%s_RDIV_FORCE2", idx, (alt ? "A" : "") ) ) ) {
		return 2;
	}

	unsigned rv = this->get( *FMT( "R%u%s_REG", idx, (alt ? "A" : "" ) ) );

	if ( 0 == rv ) {
		// if force2 is off and no value is set the divider
		// is not initialized!
		// Communicate by returning 0
		return 0;
	}

	return (rv + 1) << 1;
}

void
Si53xx::Si53xx::ormsk (const std::string &k, ValType m)
{
	SettingShp s = this->at( k );
	set( s, get( s ) | m );
}

void
Si53xx::Si53xx::andmsk(const std::string &k, ValType m)
{
	SettingShp s = this->at( k );
	set( s, get( s ) & m );
}

void
Si53xx::Si53xx::setRDivider(unsigned idx, unsigned val, bool alt)
{
	chkAlt(idx, alt, "Si53xx::setRDivider");
	if ( 0 == val || (val & 1) ) {
		throw std::out_of_range("Si53xx::setRDivider: R divider must be even >= 2" );
	}
	this->set( *FMT( "OUT%u%s_RDIV_FORCE2", idx, (alt ? "A" : "") ), (2 == val ? 1 : 0 ) );

	if ( 2 != val ) {
		val = (val >> 1) - 1;
		this->set( *FMT( "R%u%s_REG", idx, (alt ? "A" : "" ) ), val );
	}
}

void
Si53xx::Si53xx::init()
{
}

unsigned
Si53xx::Si53xx::getStatusLOS()
{
	// max. 4 inputs; all others bits set
	return (-16) | this->get("LOS");
}

bool
Si53xx::Si53xx::getStatusLOL()
{
	return this->get("LOL");
}

unsigned
Si53xx::Si53xx::getStatusOOF()
{
	// max. 4 inputs; all others bits set
	return (-16) | this->get("OOF");
}

bool
Si53xx::Si53xx::getStatusHOLD()
{
	return this->get("HOLD");
}

void
Si53xx::Si53xx::setIOVDD3V3(bool sel)
{
	this->set("IO_VDD_SEL", (sel ? 1 : 0));
}

bool
Si53xx::Si53xx::getIOVDD3V3()
{
	return !! this->get("IO_VDD_SEL");
}
