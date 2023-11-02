
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
Si5395::loadDefaults(bool force, const std::string &designId)
{

	InitValProvider * prov    = si5395DesignVault.getDesign( designId );
	if ( ! prov ) {
		throw std::runtime_error("Si5395::loadDefaults: no design with requested ID found");
	}

	if ( ! force ) {
		Si5395        tstDev;
		/* load these values into a soft device */
		tstDev.readCSV( prov );
		/* rewind for reuse */
		prov->rewind();
		/* Retrieve the ID */
		if ( tstDev.getDesignId() != this->getDesignId() ) {
			/* this design ID has not been loaded yet */
			force = 1;
		}
	}
	if ( force ) {
		/* Now load to the real device */
		this->readCSV( prov );
	}
}

std::string
Si5395::getDesignId()
{
	char           buf[30];
	const unsigned ID_SIZE = 8;
	char           id[ID_SIZE + 1];
	unsigned       idx;
	for ( idx = 0; idx < ID_SIZE; idx++ ) {
		snprintf( buf, sizeof(buf), "DESIGN_ID%d", idx );
		if ( 0 == (id[idx] = this->get( buf )) ) {
			break;
		}
	}
	id[idx]    = 0;
	return std::string( id );
}

Si53xxDesignVault<Si5395> Si53xx::si5395DesignVault;
