#ifndef SI53XX_H
#define SI53XX_H

#include <cstdint>
#include <vector>
#include <map>
#include <stdexcept>
#include <string>
#include <string.h>
#include <memory>
#include <stdio.h>

namespace Si53xx {

	using std::vector;
	using std::string;
	using std::map;
	using std::shared_ptr;

	class Setting;

	enum class Access { RO, RW, SelfClear };

	Access toAccess(const std::string &s);

	typedef uint16_t RegAddr;

	typedef vector<RegAddr> RegAddrVec;

	class Reg {
		public:
			Reg(RegAddr addr);

			virtual void
			addUser(const Setting *);

		private:
			RegAddr                  addr;
			uint8_t                  selfRstMsk;
			uint8_t                  value;
			bool                     valid;
                        bool                     cacheable;
			vector<const Setting*>   users;

		public:
			uint8_t getSelfRstMsk() const { return selfRstMsk;                      }
			int     getValue()      const { return valid && cacheable ? value : -1; }
			RegAddr getAddr()       const { return addr;                            }

			void
			update(uint8_t v)
			{
				value = v;
				valid = true;
			}

			void
			invalidate()
			{
				valid = false;
			}
	};

	typedef shared_ptr<Setting> SettingShp;
	typedef vector<SettingShp>  SettingVec;

	class Setting {
		private:
			class Key {};

		public:
			Setting(const Key &k, const char *name, const string & access, unsigned left, unsigned right, const RegAddrVec addrs);
			Setting(const Key &k, const char *name, Access access, unsigned left, unsigned right, const RegAddrVec addrs);

		private:
			string         name;
			Access         access;
			int            left, right;
			RegAddrVec     addrs;
			bool           cont;

		public:
			const string &
			getName() const
			{
				return name;
			}

			Access
			getAccess() const
			{
				return access;
			}

			int
			getLeft() const
			{
				return left;
			}

			int
			getRight() const
			{	
				return right;
			}

			uint64_t
			toMask() const
			{
				uint64_t m = (((uint64_t)1) << this->left);
				return m | (m - ( ((uint64_t)1) << this->right ) );
			}

			const RegAddrVec &
			getAddrs() const
			{
				return addrs;
			}

			bool
			isContiguous() const
			{
				return cont;
			}
			static SettingShp mkSetting(const char *name, Access access, unsigned left, unsigned right, const RegAddrVec addrs)
			{
				return std::make_shared<Setting>(Key(), name, access, left, right, addrs);
			}
			static SettingShp mkSetting(const char *name, const string & access, unsigned left, unsigned right, const RegAddrVec addrs)
			{
				return std::make_shared<Setting>(Key(), name, access, left, right, addrs);
			}
	};

	class I2cDriver {
		public:
			virtual void rd (uint8_t offset, unsigned n, uint8_t *buf) = 0;
			virtual void wr (uint8_t offset, unsigned n, uint8_t *buf) = 0;
	};

	typedef shared_ptr<I2cDriver> I2cDriverShp;

	class Si53xx;

	struct Si53xxParams {
		unsigned        numNDividers;
		unsigned        numOutputs;
	};

	enum class OutputConfig { OFF, LVDS18, LVDS25, LVDS33 };

	class InitValProvider {
	public:
		/* fetch next <address, value> pair;
		 * return -1 if no more pairs are available
		 */
		virtual int	get(unsigned *addr, unsigned char *val) = 0;
		virtual ~InitValProvider() {};
	};

	/* handle CBPro-generated headers (array element typenames change) */
	template <typename T>
	class ArrayInitValProvider : public InitValProvider {
	private:
		unsigned  idx;
		const T  *arr;
		size_t    len;
	public:
		ArrayInitValProvider(const T *a, size_t nelms)
		: idx( 0     ),
		  arr( a     ),
		  len( nelms )
		{
		}

		virtual int get(unsigned *addr, unsigned char *val)
		{
			if ( this->idx >= this->len ) {
				return -1;
			}
			*addr = this->arr[this->idx].address;
			*val  = this->arr[this->idx].value;
			this->idx++;
			return 0;
		}

		virtual void rewind()
		{
			this->idx = 0;
		}
	};

	class Si53xx {
		public:
			Si53xx(I2cDriverShp drv, const SettingVec &settings, const Si53xxParams &params);

			// to be called after object is constructed
			virtual void init(bool force = false);

			typedef uint64_t ValType;

			// load hard-coded defaults; this is only done if
			// the project ID cannot be found in the registers
			// or if 'force' is true.
			// The base/default version does nothing (to be implemented)
			// in device-specific subclass.
			virtual void loadDefaults(bool force) {}

			// by default the algorithm checks if the preamble has already been written
			// and automatically inserts it if this was not the case. By setting the
			// flag 'noAutoPreamble' the user retains full control...
			virtual void readCSV(FILE *f = stdin, bool noAutoPreamble = false);
			virtual void readCSV(const char *f, bool noAutoPreamble = false);
			virtual void readCSV(const std::string &f, bool noAutoPreamble = false);
			// use the same method but via an abstrace 'InitValProvider' object
			virtual void readCSV(InitValProvider *, bool noAutoPreamble = false);

			virtual void dumpCSV(const char *f);
			virtual void dumpCSV(const std::string &f);
			virtual void dumpCSV(FILE *f = stdout);

			virtual void dumpSettings(const char *f);
			virtual void dumpSettings(const std::string &f);
			virtual void dumpSettings(FILE *f = stdout);

		private:
			struct StrCmp {
				int operator()(const char *a, const char *b) const
				{
					return strcmp(a,b) < 0;
				}
			};

			typedef map<const char *, SettingShp, StrCmp> Settings;

			Si53xxParams  params;
			vector<Reg>   regs;
			Settings      settings;
			I2cDriverShp  drv;
			int           pageNo;
			ValType       finFreq;
			ValType       refFreq;
			ValType       vcoMinFreq;
			ValType       vcoMaxFreq;
			ValType       pfdMinFreq;
			ValType       pfdMaxFreq;

		private:
			void readRange (unsigned offset, unsigned n, uint8_t *buf);
			void writeRange(unsigned offset, unsigned n, uint8_t *buf);
			void rwRange   (bool rd, unsigned offset, unsigned n, uint8_t *buf);
			void setPage   (unsigned offset);

		protected:
			virtual void readRegs (unsigned offset, unsigned n, uint8_t *buf);
			virtual void writeRegs(unsigned offset, unsigned n, uint8_t *buf);

			virtual ValType get(SettingShp);
			virtual void    set(SettingShp, ValType);

			struct DividerSettings {
				string     prefix;
				int        idx;
				SettingShp num;
				SettingShp den;
				SettingShp update;
				bool       requirePLLOff;
			};

			virtual DividerSettings getDividerSettings (const char *prefix, int idx = -1);
			virtual DividerSettings getNDividerSettings(unsigned idx);
			virtual DividerSettings getMDividerSettings();
			virtual DividerSettings getPDividerSettings(unsigned idx);
			virtual DividerSettings getMXAXBDividerSettings();

			virtual void    getDivider(DividerSettings &s,  ValType *num, ValType *den);
			virtual double  getDivider(DividerSettings &s);
			virtual void    setDivider(DividerSettings &s, ValType  num, ValType  den);
			virtual void    setDivider(DividerSettings &s, double val);

		public:
			virtual bool    isPLLOff();

			virtual ValType get(const std::string &k);
			virtual void    set(const std::string &k, ValType v);
			virtual void    ormsk (const std::string &k, ValType m);
			virtual void    andmsk(const std::string &k, ValType m);

			virtual SettingShp at(const std::string &k);
			virtual SettingShp at(const char        *k);

			virtual unsigned getNumNDividers() const { return params.numNDividers; }
			virtual unsigned getNumOutputs()   const { return params.numOutputs;   }

			virtual void     getNDivider(unsigned idx, ValType *num, ValType *den);
			virtual double   getNDivider(unsigned idx);
			virtual void     setNDivider(unsigned idx, ValType num, ValType den);
			virtual void     setNDivider(unsigned idx, double  val);

			virtual void     getPDivider(unsigned idx, ValType *num, ValType *den);
			virtual double   getPDivider(unsigned idx);
			virtual void     setPDivider(unsigned idx, ValType num, ValType den);
			virtual void     setPDivider(unsigned idx, double  val);

			virtual void     getMDivider( ValType *num, ValType *den);
			virtual double   getMDivider();
			virtual void     setMDivider(ValType num, ValType den);
			virtual void     setMDivider(double  val);

			virtual void     getMXAXBDivider( ValType *num, ValType *den);
			virtual double   getMXAXBDivider();
			virtual void     setMXAXBDivider(ValType num, ValType den);
			virtual void     setMXAXBDivider(double  val);

			// R-divider value must be even > 0;
			// set 'alt' for R0A/R9A.
			virtual void     setRDivider(unsigned idx, unsigned val, bool alt = false);
			virtual unsigned getRDivider(unsigned idx, bool alt = false);

			virtual void     sendPreamble();
			virtual void     sendPostamble();

			virtual void     setOutputMux(unsigned idx, unsigned nDivider);
			virtual unsigned getOutputMux(unsigned idx, bool alt = false);

			// rdiv is ignored (not programmed) when switching the output OFF!
			virtual void     setOutput(unsigned idx, OutputConfig drvCfg, unsigned rdiv = 2, bool alt = false);

			virtual bool     getOutputEnable(unsigned idx, bool alt = false);
			virtual void     setOutputEnable(unsigned idx, bool enable, bool alt = false);

			virtual void     setOutputOff(unsigned idx, bool alt = false)
			{
				this->setOutput( idx, OutputConfig::OFF, alt );
			}

			virtual void     showDiff(Si53xx *other, const char *fn);
			virtual void     showDiff(Si53xx *other, FILE *f=stdout);

			struct ZDMParms {
				uint64_t     finHz;
				unsigned     inputSel;
				unsigned     nDividerSel;
				unsigned     outputSel;
				bool         outputSelAlt;
				unsigned     rDivider; // must be even
				OutputConfig outputDrvCfg;

				ZDMParms(ValType finHz)
				: finHz       (finHz),
				  inputSel    (0),
				  nDividerSel (0),
				  outputSel   (0),
				  outputSelAlt(true),
				  rDivider    (2),
				  outputDrvCfg(OutputConfig::OFF)
				{}
			};

			// Program the PLL for ZDM mode and frequency 'hz' on input 'inp'
			// using N divider 'nidx'
			virtual void     setZDM(ZDMParms *p);

			// enable/disables ZDM
			virtual void     setZDM(bool enabled);
			// returns 0 if ZDM is disabled and the configured input freqency
			// if ZDM is enabled;
			virtual ValType  getZDM();

			virtual void     selInput(unsigned inp);

			// Parameters describing a divider; if 'den' is zero a floating-
			// point may be used.
			struct DivParm {
				Si53xx::Si53xx::ValType num, den;
				double                  r;

				DivParm()
				: num(0), den(0), r(0.0)
				{
				}

				double get()
				{
					return 0 == den ? r : (double)num/(double)den;
				}
			};

			class PLLParms;
			typedef shared_ptr<PLLParms> PLLParmsShp;

			// PLL Parameters (assuming constant bandwidth and fpfd, fvco within supported range
			class PLLParms {
			friend class Si53xx;
			protected:
				struct Key {
					Key() {}
				};
				unsigned         pidx;
				Si53xx          *obj;
				virtual void     set(); // update registers
			public:	
				DivParm          P, M, MXAXB;
				uint64_t         fin;

				PLLParms(const Key &k, Si53xx *obj, unsigned pidx);

				virtual void     validate(); // throws std::invalid_argument() if the params don't make sense

				virtual void     get(); // update from associated Si53xx registers

				virtual PLLParmsShp clone()
				{
					return std::make_shared<PLLParms>(*this);
				}

				static PLLParmsShp create(Si53xx *obj, unsigned pidx)
				{
					return std::make_shared<PLLParms>(Key(), obj, pidx);
				}
			};

			// program the PLL; valid divider settings must have been precomputed!
			virtual void setPLL(PLLParmsShp parms);

			// bit-set of inputs
			virtual unsigned getStatusLOS();
			virtual bool     getStatusLOS(unsigned idx)
			{
				return !! (getStatusLOS() & (1<<idx));
			}
			virtual unsigned getStatusOOF();
			virtual bool     getStatusOOF(unsigned idx)
			{
				return !! (getStatusOOF() & (1<<idx));
			}
			// PLL status
			virtual bool     getStatusLOL();
			virtual bool     getStatusHOLD();

			// IO voltage: 3v3 (true), 1v8 (false)
			virtual void     setIOVDD3V3(bool);
			virtual bool     getIOVDD3V3();

			virtual void     flushCache();

			virtual void     reset(bool hard = true);

			virtual void     syncRDividers();

	};

	// RAII object for preamble management
	class PreambleMgr {
	private:
		bool    wasOff;
		Si53xx *ctx;
	public:
		PreambleMgr(Si53xx *ctx)
		: wasOff( ctx->isPLLOff() ),
		  ctx   ( ctx             )
		{
			if ( ! wasOff ) {
				ctx->sendPreamble();
			}
		}

		~PreambleMgr()
		{
			if ( ! wasOff ) {
				ctx->sendPostamble();
			}
		}
	};

	/* Rational approximation of a floating-point number */
	void ratapp(double n, uint64_t maxNum, uint64_t maxDen, uint64_t *nump, uint64_t *denp);

}

#endif
