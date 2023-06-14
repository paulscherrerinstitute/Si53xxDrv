#include "TstDrv.h"
#include <stdexcept>
#include <stdio.h>
#include <memory>
#include <getopt.h>
#include "Si5395.h"

using namespace Si53xx;

class feil {
	private:
		FILE *f;

	public:
		feil(const char *nm, const char *acc="r")
		{
			f = fopen(nm, acc);
			if ( ! f ) {
				throw std::runtime_error( std::string("Unable to open file ") + strerror(errno) );
			}
		}
		~feil()
		{
			fclose(f);
		}
		FILE * operator*()
		{
			return f;
		}
};

TstDrv::TstDrv()
: page(0)
{
	for ( int i = 0; i < 0x1000; i++ ) {
		regs.push_back( Reg(i) );
	}
}

void
TstDrv::wr(uint8_t off, unsigned n, uint8_t *buf)
{
	for ( int i = 0; i < n; i++ ) {
		if ( off == 0x01 ) {
			this->page = *buf;
		} else {
			unsigned a = (this->page << 8) | off;
			fprintf(stdout, "0x%04X,0x%02X\n", a, *buf);
			this->regs[a].update( *buf );
		}
		off++;
		buf++;
	}
}

void
TstDrv::rd(uint8_t off, unsigned n, uint8_t *buf)
{
	for ( int i = 0; i < n; i++ ) {
		unsigned a = (this->page << 8) | off;
        int      v = this->regs[a].getValue();
		if ( v < 0 ) {
			throw std::runtime_error("Uninitialized reg");
		}
		*buf = v;
		buf++;
		off++;
	}
}

void
TstDrv::printRegs(FILE *f)
{
	for (int a = 0; a < this->regs.size(); a++ ) {
		int v = this->regs[a].getValue();
		if ( v >= 0 ) {
			fprintf(f, "0x%04X,0x%02x\n", a, v);
		}
	}
}

class TstDev : public Si5395 {
	public:
		TstDev()
		: Si5395( std::make_shared<TstDrv>( TstDrv() ) ) {}
        void readCSV(const char *fnam)
		{
			Si5395::readCSV( *feil( fnam ) );
		}
};

int
main(int argc, char **argv)
{
	const char *fnam = "CLKB-Freerun-142.857MHz.txt";

	while ( int opt = getopt(argc, argv, "f:") > 0 ) {
		switch ( opt ) {
			case 'f': fnam = optarg; break;
		}
	}

//	std::shared_ptr<TstDrv> drv = std::make_shared<TstDrv>( TstDrv() );
//	Si5395 si(drv);
	TstDev si;

	si.readCSV( fnam );

	unsigned long long n = si.get( "N0_NUM" );
	unsigned long long d = si.get( "N0_DEN" );

	printf("N0: %llu/%llu  = 0x%llx/0x%llx = %lg\n", n, d, n, d, (double)n/(double)d);

	si.set( "N0_NUM", 256 );

//	drv->printRegs();
}
