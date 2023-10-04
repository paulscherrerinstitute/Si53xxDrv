
#include "Si5395.h"

#include "Si53xxI2c.h"
#include "TstDrv.h"

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
}
