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

	class Setting {
		private:
			class Key {};

		public:
			Setting(const Key &k, const char *name, const string & access, unsigned left, unsigned right, const RegAddrVec addrs);

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
			static SettingShp mkSetting(const char *name, const string & access, unsigned left, unsigned right, const RegAddrVec addrs)
			{
				return std::make_shared<Setting>(Key(), name, access, left, right, addrs);
			}
	};

	class I2CDriver {
		public:
			virtual void rd (uint8_t offset, unsigned n, uint8_t *buf) = 0;
			virtual void wr (uint8_t offset, unsigned n, uint8_t *buf) = 0;
	};

	typedef shared_ptr<I2CDriver> I2CDriverShp;

	class Si53xx {
		public:
			Si53xx(I2CDriverShp drv, const std::vector<Setting> &);

			typedef uint64_t ValType;

			void readCSV(FILE *f = stdin);

		private:
			struct StrCmp {
				int operator()(const char *a, const char *b) const
				{
					return strcmp(a,b);
				}
			};

			typedef map<const char *, SettingShp, StrCmp> Settings;
 
			vector<Reg>   regs;
			Settings      settings;
			I2CDriverShp  drv;
			int           pageNo;

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

		public:
			virtual ValType get(const std::string &k);
			virtual void    set(const std::string &k, ValType v);
	};
}

#endif
