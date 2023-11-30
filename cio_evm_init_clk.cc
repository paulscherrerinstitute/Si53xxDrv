/* Initialize Si5395 for CIO_EVM firmware */

#include <stdio.h>
#include <string>
#include <string.h>
#include <Si5395.h>
#include <getopt.h>
#include <math.h>

using namespace Si53xx;

static void
usage(const char *prg)
{
	printf("Basic clock setup for CIO_EVM\n");
	printf("\n");
	printf("usage: %s [-a <i2c_bus_addr>] -f <frequency_hz> -[A|B] -h\n", prg);
	printf("    -A|B           : either -A or -B must be given; select 'A' or 'B'\n");
	printf("                     device, respectively.\n");
	printf("    -<i2c_bus_addr>: <i2c_bus>@<i2c_addr>; default:\n\n");
	printf("\n");
	printf("                        /dev/i2c-5@0x68 for  'A' device\n");
	printf("                        /dev/i2c-5@0x69 for  'B' device\n");
	printf("\n");
	printf("    -<frequency_hz>: ZDM frequency (in hz)\n");
}

int
main(int argc, char **argv)
{
	double      freq = 125.0e6;
	char        type = 0;
	double     *d_p;
	const char *busaddr = 0;
	int         rval = 1;
	int         opt;

	while ( (opt = getopt(argc, argv, "a:f:ABh")) > 0 ) {
		d_p = 0;
		switch ( opt ) {
			default:
				fprintf(stderr, "Error: unknown option '-%c'\n", opt);
				rval++;
				/* fall through */
			case 'h':
				rval--;
				usage( argv[0] );
				return rval;
			case 'f':
				d_p = &freq;
				break;
			case 'A':
			case 'B':
				if ( 0 != type ) {
					fprintf(stderr, "Error: only one of -A, -B may be used\n");
					return rval;
				}
				type = opt;
				break;
			case 'a':
				busaddr = optarg;
				break;
		}
		if ( d_p && 1 != sscanf(optarg, "%lg", d_p) ) {
			fprintf(stderr, "Error: unable to scan argument of option '-%c %s'\n", opt, optarg); 
			return rval;
		}
	}
	if ( 0 == type ) {
		fprintf(stderr, "Error: exactly one of -A, -B must be used\n");
		return rval;
	}
	if ( 0 == busaddr ) {
		busaddr = ( 'A' == type ? "/dev/i2c-5@0x68" : "/dev/i2c-5@0x69" );
	}

	char busname[200];
	int  addr;
	int  len = sizeof(busname) + 1;
	int  got;

	if ( 1 != (got = sscanf( busaddr, "%*[^@]%n@%i", &len, &addr )) || (size_t)len > sizeof(busname) - 1 ) {
		fprintf(stderr, "Error: invalid busaddr argument %i %i\n", got, len);
		return rval;
	}
	if ( 2 != (got = sscanf( busaddr, "%[^@]@%i", busname, &addr )) ) {
		fprintf(stderr, "Error: unable to scan invalid busaddr argument\n");
		return rval;
	}
	if ( addr < 0 || addr > 126 ) {
		fprintf(stderr, "Warning: unreasonable I2C address: 0x%x\n", addr);
	}

	Si5395 clk( busname, addr );

	int      f = ('A' == type ? 7 : 6);
	int      t = 9;
	unsigned rdiv = 4;
	for (int o = f; o <= t; o++ ) {
		clk.setOutputMux( o, 0 );
		// the first of these outputs is the event clock; the others are
		// MGT reference clocks which run at 2*event_clk
		clk.setOutput( o, OutputConfig::LVDS33, (o == f ? rdiv * 2 : rdiv ) );
	}
	{
	PreambleMgr mgr( &clk );
		unsigned refInput = 2;
		clk.setZDM( (uint64_t) round( freq ), refInput, 2*rdiv, OutputConfig::LVDS33 );
	}
	clk.syncRDividers();

	rval = 0;
	return rval;
}
