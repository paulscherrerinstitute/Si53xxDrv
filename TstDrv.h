#ifndef SI53XX_TST_DRV_Y
#define SI53XX_TST_DRV_Y
#include "Si53xx.h"

using namespace Si53xx;
using std::vector;

class TstDrv : public I2cDriver {
	private:
		vector<Reg> regs;
		int         page;
		
	public:
		TstDrv();
		virtual void rd(uint8_t off, unsigned n, uint8_t *buf);
		virtual void wr(uint8_t off, unsigned n, uint8_t *buf);

		void printRegs(FILE *f = stdout);
};
#endif
