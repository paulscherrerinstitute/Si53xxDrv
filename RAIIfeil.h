#ifndef RAIIFEIL
#define RAIIFEIL

// RAII helper

#include <stdio.h>

class RAIIfeil {
	private:
		FILE *f;

	public:
		RAIIfeil(const char *nm, const char *acc="r")
		{
			f = fopen(nm, acc);
			if ( ! f ) {
				throw std::runtime_error( std::string("Unable to open file ") + strerror(errno) );
			}
		}
		~RAIIfeil()
		{
			fclose(f);
		}
		FILE * operator*()
		{
			return f;
		}
};

#endif
