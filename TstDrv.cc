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
}

void
TstDrv::wr(uint8_t off, unsigned n, uint8_t *buf)
{
	for ( int i = 0; i < n; i++ ) {
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
