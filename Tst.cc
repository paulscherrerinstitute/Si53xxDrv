#include <stdexcept>
#include <stdio.h>
#include <getopt.h>
#include "Si5395.h"

using namespace Si53xx;

int
main(int argc, char **argv)
{
	const char *fnam = "CLKB-Freerun-142.857MHz.txt";

	while ( int opt = getopt(argc, argv, "f:") > 0 ) {
		switch ( opt ) {
			case 'f': fnam = optarg; break;
		}
	}

	Si5395 si;

	si.readCSV( fnam );

	unsigned long long n = si.get( "N0_NUM" );
	unsigned long long d = si.get( "N0_DEN" );

	printf("N0: %llu/%llu  = 0x%llx/0x%llx = %lg\n", n, d, n, d, (double)n/(double)d);

	si.set( "N0_NUM", 256 );

//	drv->printRegs();
}
