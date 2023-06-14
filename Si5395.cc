
#include "Si5395.h"

namespace Si53xx {
	extern SettingVec Si5395Settings;
};

using namespace Si53xx;

static const Si53xxParams params = {
	numNDividers:  5,
	numOutputs:   10
};

Si5395::Si5395(I2CDriverShp drv)
: Si53xx(drv, Si5395Settings, ::params)
{
}
