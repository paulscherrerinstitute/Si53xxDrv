#include "Si5395.h"
#include <stdexcept>

using namespace Si53xx;

int
main(int argc, char **argv)
{
	Si5395 si;
	Si5395 sidflt;

	try {
		sidflt.readCSV( "regDflt.csv" );
		if ( 1 ) {
		sidflt.sendPreamble();

		sidflt.setZDM( true );
		sidflt.selInput( 2 );
		sidflt.setOutput( 9, OutputConfig::LVDS33, 2, false );
		sidflt.setOutput( 9, OutputConfig::LVDS33, 2, true );
		sidflt.setOutputMux( 9, 0 );
		sidflt.setPDivider( 2, 75, 1 );
		sidflt.setPDivider( 3, 75, 1 );

		sidflt.sendPostamble();
		}
		si.readCSV("Si5395-RevA-Registers.txt");
		si.showDiff( &sidflt );
	} catch ( std::exception &e ) {
		printf("Exception: %s\n", e.what());
	}
}
