#include <iocshDeclWrapper.h>
#include <errlog.h>
#include <map>
#include <Si5395.h>
#include <Si53xxIocshSup.h>

#if __cplusplus < 201103L
#error "Need >= C++11"
#endif

static std::map<const std::string, Si53xx::Si5395 *> objMap;

using namespace IocshDeclWrapper;
using namespace Si53xx;

Si5395 *
Si53xx::si5395Find(const char *name)
{
	try {
		return objMap.at( name );
	} catch ( std::out_of_range & ) {
		return 0;
	}
}

static int Si5395RegisterClock(const char *name, const char *i2cBus, unsigned i2cAddr)
{
    try {
        objMap.at( name );
        errlogPrintf("Error: Si5395 with the requested name already exists; registration failed\n");
        return -1;
    } catch ( std::out_of_range &e ) {
        /* this is an OK result; name not used yet */
    }
    try {
        if ( ! i2cBus ) {
            throw std::runtime_error("Error: missing i2c bus name");
        }
        Si5395 *o = new Si5395( i2cBus, i2cAddr );
		o->init();
        objMap[ name ] = o ;
    } catch ( std::exception &e ) {
        errlogPrintf("Error: registration failed due to exception: %s\n", e.what());
        return -1;
    }
    return 0;
}

#define SI5395_WRAP(memb, argHelps...) \
  IOCSH_MEMBER_WRAP( &objMap, Si5395, memb, argHelps )
#define SI5395_WRAP_OVLD(memb, sig, argHelps...) \
  IOCSH_MEMBER_WRAP_OVLD( &objMap, Si5395, memb, sig, "Si5395_"#memb, argHelps )

IOCSH_FUNC_WRAP_REGISTRAR( Si5395Registrar,
  IOCSH_FUNC_WRAP( Si5395RegisterClock, "objName", "i2c-Bus, e.g., /dev/i2c-5", "i2c-Address" );
  SI5395_WRAP(      isPLLOff );
  SI5395_WRAP(      sendPreamble );
  SI5395_WRAP(      sendPostamble );
  SI5395_WRAP_OVLD( getStatusLOS, (void) );
  SI5395_WRAP_OVLD( getStatusOOF, (void) );
  SI5395_WRAP(      getStatusLOL );
  SI5395_WRAP(      getStatusHOLD );
  SI5395_WRAP(      getRDivider, "idx", "alt (set nonzero for 0A, 9A)" );
  SI5395_WRAP(      setRDivider, "idx", "val (even >=2 )", "alt (set nonzero for 0A, 9A)" );
  SI5395_WRAP(      setOutput,   "idx", "outputConfig (3 for lvds33)", "rDivider (even >=2)", "alt (set nonzero for OUT0A, OUT9A)");
  SI5395_WRAP(      getOutputEnable, "idx", "alt (set nonzero for OUT0A, OUT9A)");
  SI5395_WRAP(      setOutputEnable, "idx", "enableVal", "alt (set nonzero for OUT0A, OUT9A)");
  SI5395_WRAP(      syncRDividers );
  SI5395_WRAP_OVLD( get,        (const std::string&), "key" );
  SI5395_WRAP_OVLD( set,        (const std::string&, Si53xx::Si53xx::ValType), "key", "value" );
  SI5395_WRAP_OVLD( setZDM,     (uint64_t, unsigned, unsigned, OutputConfig), "freqHz", "inputSel", "rDivider (even >= 2)", "outputConfig (3 for lvds33)" );
)
