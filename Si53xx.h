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
			vector<const Setting*>   users;

		public:
			uint8_t getSelfRstMsk() const { return selfRstMsk;         }
			int     getValue()      const { return valid ? value : -1; }

			void
			update(uint8_t v)
			{
				value = v;
				valid = true;
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
				return m | (m - ((uint64_t)1) << this->right);
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

	class Si53xx {
		public:
			Si53xx(I2cDriverShp drv, const SettingVec &settings, const Si53xxParams &params);

			typedef uint64_t ValType;

			virtual void readCSV(FILE *f = stdin);
			virtual void readCSV(const char *f);
			virtual void readCSV(const std::string &f);

			virtual void dumpCSV(const char *f);
			virtual void dumpCSV(const std::string &f);
			virtual void dumpCSV(FILE *f = stdout);

			// initialize to a sane state; usually to values obtained from CBPro;
			// you can also call 'readCSV' from a subclass.
			// Note that 'init' assumes the preamble has already been sent
			// and it does not send the postamble itself.
			// The rationale is that an application uses this as part
			// of a more complete initialization sequence in order to
			// establish a base-line.
			virtual void init();

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
			ValType       zdmFreq;
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

			virtual bool    isPllOff();

			struct DividerSettings {
				string     prefix;
				int        idx;
				SettingShp num;
				SettingShp den;
				SettingShp update;
				bool       requirePllOff;
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
			virtual void     setRDivider(unsigned idx, bool alt, unsigned val);
			virtual unsigned getRDivider(unsigned idx, bool alt);

			virtual void     sendPreamble();
			virtual void     sendPostamble();

			// enable/disable 
			virtual void     setOutput(unsigned idx, bool alt, OutputConfig drvCfg, unsigned nDivider);

			virtual void     showDiff(Si53xx *other, const char *fn);
			virtual void     showDiff(Si53xx *other, FILE *f=stdout);

			// Program the PLL for ZDM mode and frequency 'hz' on input 'inp'
			// using N divider 'nidx'
			virtual void     setZDM(ValType hz, unsigned inp, unsigned nidx = 0);

			// enable/disables ZDM
			virtual void     setZDM(bool enabled);
			// returns 0 if ZDM is disabled and the configured input freqency
			// if ZDM is enabled;
			virtual ValType  getZDM();

			virtual void     selInput(int inp);
	};

	/* Rational approximation of a floating-point number */
	static void ratapp(double n, uint64_t maxNum, uint64_t maxDen, uint64_t *nump, uint64_t *denp);

}

#endif
