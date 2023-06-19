#include "Si53xxI2c.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdexcept>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include <sys/ioctl.h>

namespace Si53xx {

I2cDev::I2cDev(const std::string &name, unsigned addr)
: addr(addr)
{
	if ( (fd = open( name.c_str(), O_RDWR )) < 0 ) {
		throw std::runtime_error(std::string("Si53xx::I2cDev: unable to open device: ") + strerror(errno));
	}
	if ( ioctl( fd, I2C_SLAVE, addr ) ) {
		close( fd );
		throw std::runtime_error(std::string("Si53xx::I2cDev: unable to set i2c address: ") + strerror(errno));
	}
}

void
I2cDev::rd(uint8_t off, unsigned n, uint8_t *buf)
{
	rdwr( true, off, n, buf );
}

void
I2cDev::wr(uint8_t off, unsigned n, uint8_t *buf)
{
	rdwr( false, off, n, buf );
}


void
I2cDev::rdwr(bool doRead, uint8_t off, unsigned n, uint8_t *buf)
{
	struct i2c_msg msg[2];
	uint8_t        loff = off;

	msg[0].addr  = this->addr;
	msg[0].flags = 0; // write
	msg[0].buf   = &loff;
	msg[0].len   = 1;

	msg[1].addr  = this->addr;
	msg[1].flags = ( doRead ? I2C_M_RD : 0 );
	msg[1].buf   = buf;
	msg[1].len   = n;

	if ( ioctl( this->fd, I2C_RDWR, &msg, sizeof(msg)/sizeof(msg[0]) ) ) {
		std::string dir( doRead ? "read" : "write" );
		throw std::runtime_error(std::string("Si53xx::I2cDev: i2c ") + dir + "read operation faileed: " + strerror(errno));
	}
}

I2cDev::~I2cDev()
{
	close( fd );
}

};
