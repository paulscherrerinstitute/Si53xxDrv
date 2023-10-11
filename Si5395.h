#ifndef SI5395_DRIVER_H
#define SI5395_DRIVER_H

#include "Si53xx.h"

namespace Si53xx {
	class Si5395 : public Si53xx {
		public:
			Si5395(I2cDriverShp drv);
			Si5395(const char *i2cDev, unsigned i2cAddr);
			Si5395(); // test/dummy driver
			virtual void loadDefaults(bool force);
			virtual unsigned getZDMNIdx() const { return 0;    }
			virtual unsigned getZDMRIdx() const { return 9;    }
			virtual bool     getZDMRAlt() const { return true; }
	};
};

#endif
