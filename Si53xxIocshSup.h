#ifndef SI53XX_IOCSH_SUP_H
#define SI53XX_IOCSH_SUP_H

#include <Si5395.h>

namespace Si53xx {

// Si53xx devices are created and registered from IOCSH
// (Si5395Register) and they are given a name.
// This routine looks up the devices by name for use
// by EPICS drivers/device support.
// A 'out_of_range' exception is thrown if the name is
// not found.
Si5395 *si5395Find(const char *name);

};

#endif
