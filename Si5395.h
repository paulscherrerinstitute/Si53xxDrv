#ifndef SI5395_DRIVER_H
#define SI5395_DRIVER_H

#include "Si53xx.h"

namespace Si53xx {
	class Si5395 : public Si53xx {
		public:
			Si5395(I2CDriverShp drv);
	};
};

#endif
