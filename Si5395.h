#ifndef SI5395_DRIVER_H
#define SI5395_DRIVER_H

#include "Si53xx.h"

namespace Si53xx {
	class Si5395 : public Si53xx {
		public:
			Si5395(I2cDriverShp drv);
			Si5395(const char *i2cDev, unsigned i2cAddr);
			Si5395(); // test/dummy driver
			// set ZDM assuming N divider 0, feedback output 9a
			using Si53xx::setZDM;
			virtual void setZDM(uint64_t finHz, unsigned inputSel, unsigned rDivider = 2, OutputConfig outputDrvCfg = OutputConfig::LVDS33);
			virtual void loadDefaults(bool force);
	};
};

#endif
