#ifndef SI53XX_I2C_H
#define SI53XX_I2C_H

#include <Si53xx.h>
#include <string>

namespace Si53xx {
	// A driver for i2c-dev
	class I2cDev : public I2cDriver {
		private:
			unsigned addr;
			int      fd;
			int      pg;

			I2cDev(const I2cDev&)             = delete;
			I2cDev & operator=(const I2cDev&) = delete;
		public:
			I2cDev(const std::string&, unsigned addr);
			virtual ~I2cDev();

			virtual void rd(uint8_t off, unsigned n, uint8_t *buf);
			virtual void wr(uint8_t off, unsigned n, uint8_t *buf);

	};
};

#endif
