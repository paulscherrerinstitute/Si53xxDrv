
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
  zdmFreq          ( 0           ),
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
Si53xx::Si53xx::readCSV(const std::string &fn)
{
	this->readCSV( fn.c_str() );
}

void
Si53xx::Si53xx::readCSV(const char *fn)
{
	this->readCSV( *RAIIfeil( fn ) );
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
		if ( fprintf(f, "0x%04x, 0x%02x\n", i, buf[i]) < 0 ) {
			throw std::runtime_error("Si53xx::dumpCSV(): file write error");
		}
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
	if ( s.requirePllOff && ! isPllOff() ) {
		throw std::logic_error("Si53xx::setDivider: cannot set divider while PLL is running");
	}
	set( s.num,    num );
	set( s.den,    den );

	if ( 'P' == s.prefix[0] || 0 == strcmp(s.prefix.c_str(), "M") ) {
		this->set( *FMT( s.prefix + "_FRACN_EN"), (den > 1 ? 1 : 0 ) );
		if ( 'P' == s.prefix[0] ) {
			if ( den > 1 ) {
				this->andmsk( "PDIV_FRACN_CLK_DIS", ~ ( 1 << s.idx ) );
			} else {
				this->ormsk ( "PDIV_FRACN_CLK_DIS",   ( 1 << s.idx ) );
			}
		} else {
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
	s.requirePllOff = (s.prefix[0] != 'N');
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
	if ( isPllOff() ) {
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
	if ( ! isPllOff() ) {
		throw std::logic_error("Si53xx::sendPostable: postamble has already been sent?");
	}
	set( "BW_UPDATE_PLL", 1 );
	set( "SOFT_RST_ALL",  1 );
	// send magical, undocumented values :-(
	set( "AMBLE2", 0x00 );
	set( "AMBLE0", 0xC3 );
	set( "AMBLE1", 0x02 );
}

bool
Si53xx::Si53xx::isPllOff()
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
Si53xx::Si53xx::setOutput(unsigned idx, bool alt, OutputConfig drvCfg, unsigned nDivider)
{
	chkAlt( idx, alt, "Si53xx::SetOutput" );
	FMT pre( "OUT%u%s_", idx, (alt ? "A" : "") );

	set( *FMT( "%sSYNC_EN",   *pre ), 1 );

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

	ValType outEn = ( OutputConfig::OFF != drvCfg );

	set( *FMT( "%sOE",       *pre ),   outEn );
	set( *FMT( "%sPDN",      *pre ), ! outEn );

	set( *FMT( "%sMUX_SEL", *pre ), nDivider ); 

	if ( 0 == get( *FMT( "R%d%s_REG", idx, (alt ? "A" : "") ) ) ) {
		this->setRDivider( idx, alt, 2 );
	}
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
Si53xx::Si53xx::selInput(int inp)
{
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

struct PLLParm {
	DivParm          P, M, MXAXB, N;
	unsigned         pidx, nidx;
	Si53xx::Si53xx  *obj;

	PLLParm(Si53xx::Si53xx *obj, unsigned pidx, unsigned nidx)
	: obj(obj), pidx(pidx), nidx(nidx)
	{
	}

	void get()
	{
		obj->getPDivider    ( pidx,  &P.num,     &P.den );
		obj->getMDivider    (        &M.num,     &M.den );
		obj->getMXAXBDivider(        &MXAXB.num, &MXAXB.den );
		obj->getNDivider    ( nidx,  &N.num,     &N.den );
	}

	void set()
	{
		if ( P.den > 0 ) {
			obj->setPDivider    ( pidx,   P.num,      P.den );
		} else {
			obj->setPDivider    ( pidx,   P.r );
		}
		if ( M.den > 0 ) {
			obj->setMDivider    (         M.num,      M.den );
		} else {
			obj->setMDivider    (         M.r     );
		}
		if ( MXAXB.den > 0 ) {
			obj->setMXAXBDivider(         MXAXB.num,  MXAXB.den );
		} else {
			obj->setMXAXBDivider(         MXAXB.r );
		}
		if ( N.den > 0 ) {
			obj->setNDivider    ( nidx,   N.num,      N.den );
		} else {
			obj->setNDivider    ( nidx,   N.r     );
		}
	}
};

static void findDiv(
	Si53xx::Si53xx::ValType f,
	Si53xx::Si53xx::ValType min,
	Si53xx::Si53xx::ValType max,
	DivParm                *p
)
{
	Si53xx::Si53xx::ValType v;

	v = f/max;

	while ( f/v > max ) v++;

	while ( f/v >= min ) {
		p->num = v;
		p->den = 1;
		// try to find multiples of 5
		if ( ( v % 5 ) == 0 ) {
			return;
		}
		v++;
	}
	if ( 0 == p->den ) {
		p->r   = (double)f/(double)max;
	}
}

void
Si53xx::Si53xx::setZDM(uint64_t hz, unsigned inp, unsigned ndiv)
{
	PLLParm op( this, inp, ndiv );
	PLLParm np( this, inp, ndiv );

	op.get();

	try {
		ValType p,n, p5 = 0, n5 = 0;

		// try to find integer dividers with the constraint that fvco remain
		// within the 'known' range.
		//   fvco = fin / P * 5 * M      (fixed 5 divider present)
		//   fout = fin (zdm) = fvco/2/N (min. R divider is 2    )
		//
		//   thus for integer ratios:  5*M = 2 * N * P

		n  = this->vcoMinFreq/hz/2; 

		for ( p = (hz/this->pfdMinFreq); hz/p <= this->pfdMaxFreq && 0 == p5; p-- ) {
			// record a valid integer P
			np.P.num = p;
			np.P.den = 1;
			if ( 0 == ( np.P.num % 5 ) ) {
				// 5 divides P => M = 2*N*p5
				p5 = p/5;
				if ( hz*n*2 <= this->vcoMaxFreq ) {
					np.M.num = 2*n*p5;
					np.M.den = 1;
					np.N.num = n;
					np.N.den = 1;
				} else {
					/* seriously? */
					np.N.r   = ((double)this->vcoMinFreq + (double)this->vcoMaxFreq)/2.0;
					np.N.r   /= 2.0*(double)hz;
					np.M.r   = 2.0 * np.N.r * (double) p5;
				}
			}
		}

		if ( 0 == p5 ) {
			// P not divisible by 5; try to find an N
			while ( hz*n*2 <= this->vcoMaxFreq && 0 == n5 ) {
				if ( 0 == ( n % 5 ) ) {
					// found one
					n5       = n/5;
					np.N.num = n;
					np.N.den = 1;
					if ( 0 == np.P.den ) {
						np.M.num = n5 * 2 * (ValType)round( np.P.r );
					} else {
						np.M.num = n5 * 2 * np.P.num;
					}
				}
				n--;
			}
		}

		if ( 0 == p5 && 0 == n5 ) {
			/* no multiple of 5 found; must use fractional divider;
			 * use on P. Note the 0 == p5 test also covers the
			 * case when no integer P divider at all is found...
			 */
			ValType fvco;
			double  pflt = 0.0;
			for ( n = this->vcoMinFreq/2/hz; (fvco = n*2*hz) <= this->vcoMaxFreq && 0.0 == pflt; n-- ) {
				for ( ValType m = fvco/5/this->pfdMinFreq; fvco / 5 / m <= this->pfdMaxFreq && 0.0 == pflt; m-- ) {
					np.N.num = n;
					np.N.den = 1;
					np.M.num = m;
					np.M.den = 1;
					pflt = (5.0 * (double)m)/2.0/(double)n;
				}
			}
			if ( 0.0 == pflt ) {
				/* still no solution? In this unlikely case we brute-force all
				 * fractional dividers...
				 */
				double vcoMid = (double)(this->vcoMinFreq + this->vcoMaxFreq)/2.0;
				double pfdMid = (double)(this->pfdMinFreq + this->pfdMaxFreq)/2.0;

				pflt   = (double)hz / pfdMid;
				np.N.r = vcoMid/(double)hz/2.0;
				np.M.r = vcoMid/5.0/pfdMid;
			}
			np.P.r   = pflt;
			np.P.den = 0; // reset an stale integer divider
		}

		np.set();
		this->zdmFreq = hz;
	} catch ( std::exception &e ) {
		// if the very first modification failed we may not be able to restore
		try {
			// try to restore original settings
			op.set();
		} catch ( std::exception &e ) {
		}
		throw;
	}
}

void
Si53xx::Si53xx::setZDM(bool ena)
{
	if ( 0 == this->zdmFreq ) {
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
	this->set( "OUTX_ALWAYS_ON", outx );
}

uint64_t
Si53xx::Si53xx::getZDM()
{
	return this->get( "ZDM_EN" ) ? this->zdmFreq : 0;
}

unsigned
Si53xx::Si53xx::getRDivider(unsigned idx, bool alt)
{
	chkAlt(idx, alt, "Si53xx::getRDivider");
	if ( this->get( *FMT( "OUT%u%s_RDIV_FORCE2", idx, (alt ? "A" : "") ) ) ) {
		return 2;
	}
	return (this->get( *FMT( "R%u%s_REG", idx, (alt ? "A" : "" ) ) ) + 1) << 1;
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
Si53xx::Si53xx::setRDivider(unsigned idx, bool alt, unsigned val)
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
