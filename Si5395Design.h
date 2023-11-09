#ifndef SI5395_DESIGN_H
#define SI5395_DESIGN_H

#include <Si5395.h>
#include <Si53xxDesign.h>

namespace Si53xx {
	Si53xxDesignVault<Si5395> *getSi5395DesignVault();
};

#endif
