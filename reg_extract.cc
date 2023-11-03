/* Convert Skyworks-generated header into our C++ format
 */

/* This header must include the register map file to be wrapped and
 * #define the register array to SI_TO_WRAP
 */

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include "Si5395-RevA-Regmap.h"
extern "C" {
#define SI_TO_WRAP si5395_reva_settings
};

#include <string>
#include <unordered_set>

static const char *toAccess(const char *name, int ro, int sc)
{
	if ( ro )
		return "Access::RO";
	if ( sc )
		return "Access::SelfClear";
	// a hack; self-setting (non-RO) bits are not specifically
	// marked but they happen to be have _FLG in their name...
	if ( strstr( name, "_FLG" ) ) {
		return "Access::SelfSet";
	}
	return "Access::RW";
}

typedef struct setting_def_t {
	const char       *name;
	unsigned char     read_only;
	unsigned char     self_clearing;
	unsigned char     bit_length;
	unsigned char     start_bit;
	unsigned char     reg_length;
	unsigned short    addr[1];
} setting_def_t;

static setting_def_t amble_dflt[] = {
	{ "AMBLE0", 0, 0, 8, 0, 1, { 0xb24 } },
	{ "AMBLE1", 0, 0, 8, 0, 1, { 0xb25 } },
	{ "AMBLE2", 0, 0, 8, 0, 1, { 0x540 } }
};

#ifndef EXTRA_ENTRIES
#define EXTRA_ENTRIES amble_dflt
#endif

#ifdef NO_EXTRA_ENTRIES
#undef EXTRA_ENTRIES
#endif

/* Found in the manual but missing from the register map header */
static setting_def_t missing[] = {
	{ "XAXB_ERR", 1, 0, 1, 3, 1, { 0x00c } },
	{ "HARD_RST", 1, 0, 1, 3, 1, { 0x00c } },
};

typedef std::unordered_set<std::string> NameSet;

template <class T, size_t N> void
writeDefinitions(const T (&defList)[N], FILE *f, NameSet *s)
{
size_t i,j;
	for ( i = 0; i < sizeof(defList)/sizeof(defList[0]); i++ ) {
		if ( !! s->count( defList[i].name ) ) {
			/* skipping redundant entries */
			continue;
		}
		s->insert( defList[i].name );
		fprintf(f,"\tSetting::mkSetting(\"%s\", %s, %d, %d, {",
			defList[i].name,
			toAccess(defList[i].name, defList[i].read_only, defList[i].self_clearing),
			defList[i].start_bit + defList[i].bit_length - 1,
			defList[i].start_bit);
		for ( j = 0; j < defList[i].reg_length; j++ ) {
			fprintf(f,"%s0x%03x", (0 == j ? "" : ","), defList[i].addr[j]);
		}
		fprintf(f,"}),\n");
	}
}

int
main(int argc, char **argv)
{
int i, j;
const char *prefix = PREFIX;
FILE       *f      = stdout;
NameSet     allNames;

	fprintf(f,"/* THIS FILE WAS AUTOMATICALLY GENERATED (reg_extract.cc) -- DO NOT _MODIFY! */\n");
	fprintf(f,"#include <Si53xx.h>\n");
	fprintf(f,"namespace Si53xx {\n");
	fprintf(f,"SettingVec %sSettings = {", prefix);

	writeDefinitions( SI_TO_WRAP, f, &allNames );

#ifdef EXTRA_ENTRIES
    writeDefinitions( EXTRA_ENTRIES, f, &allNames );
#endif

	writeDefinitions( missing, f, &allNames );

	fprintf(f,"\n};\n");
	fprintf(f,"\n}");
	return 0;
}
