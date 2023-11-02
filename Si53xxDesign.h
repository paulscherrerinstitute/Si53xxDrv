#ifndef SI53XX_DESIGN_H
#define SI53XX_DESIGN_H

#include <map>
#include <Si53xx.h>

namespace Si53xx {

template <typename T, int ID_SIZE = 8>
class Si53xxDesignVault {
private:
	std::map<const std::string, InitValProvider *> _map;
    Si53xxDesignVault(const Si53xxDesignVault &)       = delete;
    Si53xxDesignVault & operator=(Si53xxDesignVault &) = delete;
public:
	Si53xxDesignVault()
	{
	}

	// returns dummy value so it can be used from a static initializer
	virtual int addDesign(InitValProvider *p)
	{
		T        dummyDev;
		dummyDev.readCSV( p );
		p->rewind();
		std::string key( dummyDev.getDesignId() );
		_map[ key ] = p;

		return 0;
	}

	virtual InitValProvider * getDesign(const std::string id)
	{
		if ( 0 == id.size() ) {
			// default is the lexically largest ID (= biggest version number?)
			if ( _map.rbegin() == _map.rend() ) {
				return 0;
			}
			return _map.rbegin()->second;
		}
		return _map[id];
	} 
};

};

#endif
