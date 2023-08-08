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
#include <sys/uio.h>

/* For unknown reasons I get EGAIN when using I2C_RDWR :-(;
 * resort to write+read but that creates a race condition
 * if multiple masters access the same slave.
 */
#define USE_RDWR 0

namespace Si53xx {

I2cDev::I2cDev(const std::string &name, unsigned addr)
: addr(addr),
  pg  ( -1 )
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
	struct i2c_msg             msgs[2];
	struct i2c_rdwr_ioctl_data iocd;
	uint8_t                    loff = off;

	msgs[0].addr  = this->addr;
	msgs[0].flags = 0; // write
	msgs[0].buf   = &loff;
	msgs[0].len   = 1;

	msgs[1].addr  = this->addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf   = buf;
	msgs[1].len   = n;

	iocd.msgs     = msgs;
	iocd.nmsgs    = sizeof(msgs)/sizeof(msgs[0]);

	if ( ioctl( this->fd, I2C_RDWR, &iocd ) < 0 ) {
		throw std::runtime_error(std::string("Si53xx::I2cDev: i2c read operation failed: ") + strerror(errno));
	}
}

void
I2cDev::wr(uint8_t off, unsigned n, uint8_t *buf)
{
	// gathered write (via I2C_RDWR or writev)
	// does not work...
	if ( n > 4096 ) {
		throw std::runtime_error(std::string("Si53xx::I2cDev: i2c write failed -- excessive data size"));
	}
	uint8_t lbuf[ n + 1 ];
	lbuf[0] = off;
	memcpy( lbuf + 1, buf, n );
	if ( write( this->fd, lbuf, n + 1 ) < 0 ) {
		throw std::runtime_error(std::string("Si53xx::I2cDev: i2c write failed: ") + strerror(errno));
	}
}

I2cDev::~I2cDev()
{
	close( fd );
}

};
