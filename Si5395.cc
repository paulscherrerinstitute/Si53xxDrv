
#include "Si5395.h"

#include "Si53xxI2c.h"
#include "TstDrv.h"

#include "Si5395-RevA-cio_timing_base-CIO0100-Registers.h"

namespace Si53xx {
	extern SettingVec Si5395Settings;
};

using namespace Si53xx;

static const Si53xxParams params = {
	numNDividers:  5,
	numOutputs:   10
};

Si5395::Si5395(I2cDriverShp drv)
: Si53xx(drv, Si5395Settings, ::params)
{
}

Si5395::Si5395(const char *i2cDev, unsigned i2cAddr)
: Si5395( std::make_shared<I2cDev>( i2cDev, i2cAddr ) )
{
}

Si5395::Si5395()
: Si5395( std::make_shared<TstDrv>() )
{
}

void
Si5395::setZDM(uint64_t finHz, unsigned inputSel, unsigned rDivider, OutputConfig outputDrvCfg)
{
	ZDMParms prm( finHz );
	prm.inputSel     = inputSel;
	prm.nDividerSel  = 0;
	prm.outputSel    = 9;
	prm.outputSelAlt = true;
	prm.rDivider     = rDivider;
	prm.outputDrvCfg = outputDrvCfg;
	this->setZDM( &prm );
}

void
Si5395::loadDefaults(bool force)
{
	const unsigned ID_SIZE = 8;
	ArrayInitValProvider<si5395_reva_register_t> prov(
		&si5395_reva_registers[0],
		sizeof(si5395_reva_registers)/sizeof(si5395_reva_registers[0])
	);

	if ( ! force ) {
		Si5395        tstDev;
		char          buf[30];
		/* load these values into a soft device */
		tstDev.readCSV( &prov );
		/* rewind for reuse */
		prov.rewind();
		/* Retrieve the ID */
		for ( unsigned i = 0; i < ID_SIZE; i++ ) {
			snprintf( buf, sizeof(buf), "DESIGN_ID%d", i );
			if ( tstDev.get( buf ) != this->get ( buf ) ) {
				/* this design ID has not been loaded yet */
				force = 1;
				break;
			}
		}
	}
	if ( force ) {
		/* Now load to the real device */
		this->readCSV( &prov );
		printf("Si5395: Loaded Default Settings\n");
	}
}
