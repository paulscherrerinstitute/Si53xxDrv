#include "TstDrv.h"
#include <stdexcept>
#include <stdio.h>
#include <memory>
#include <getopt.h>
#include "Si5395.h"

using namespace Si53xx;

TstDrv::TstDrv()
: page(0)
{
	for ( int i = 0; i < 0x1000; i++ ) {
		regs.push_back( Reg(i) );
	}
	// must at least initialize the pre/postamble registers
	// to something; otherwise, loading config file fails
	// due to 'uninitialized' registers.
    regs[0x540].update( 0 );
    regs[0xb24].update( 0 );
    regs[0xb25].update( 0 );
	// Also initialize ID
	for ( int i = 0; i < 8; i++ ) {
		regs[0x26b + i].update( 0 );
	}
}

void
TstDrv::wr(uint8_t off, unsigned n, uint8_t *buf)
{
	for ( unsigned i = 0; i < n; i++ ) {
		if ( off == 0x01 ) {
			this->page = *buf;
		} else {
			unsigned a = (this->page << 8) | off;
			if ( 0 ) {
				// debugging
				fprintf(stdout, "0x%04X,0x%02X\n", a, *buf);
			}
			this->regs[a].update( *buf );
		}
		off++;
		buf++;
	}
}

void
TstDrv::rd(uint8_t off, unsigned n, uint8_t *buf)
{
	for ( unsigned i = 0; i < n; i++ ) {
		unsigned a = (this->page << 8) | off;
        int      v = this->regs[a].getValue();
		if ( v < 0 ) {
			char buf[256];
			snprintf(buf, sizeof(buf), "Uninitialized reg 0x%x", a);
			throw std::runtime_error(buf);
		}
		*buf = v;
		buf++;
		off++;
	}
}

void
TstDrv::printRegs(FILE *f)
{
	for (unsigned a = 0; a < this->regs.size(); a++ ) {
		int v = this->regs[a].getValue();
		if ( v >= 0 ) {
			fprintf(f, "0x%04X,0x%02x\n", a, v);
		}
	}
}
