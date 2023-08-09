/* Convert Skyworks-generated header into our C++ format
 */

/* This header must include the register map file to be wrapped and
 * #define the register array to SI_TO_WRAP
 */

#include <stdio.h>
#include <string.h>
#include <getopt.h>

static const char *toAccess(int ro, int sc)
{
	if ( ro )
		return "Access::RO";
	if ( sc )
		return "Access::SelfClear";
	return "Access::RW";
}

typedef struct amble_dflt_t {
	const char       *name;
	unsigned char     read_only;
    unsigned char     self_clearing;
    unsigned char     bit_length;
    unsigned char     start_bit;
	unsigned char     reg_length;
	unsigned short    addr[1];
} amble_dflt_t;

static amble_dflt_t amble_dflt[] = {
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

int
main(int argc, char **argv)
{
int i, j;
const char *prefix = PREFIX;
FILE       *f      = stdout;

	fprintf(f,"/* THIS FILE WAS AUTOMATICALLY GENERATED (reg_extract.c) -- DO NOT _MODIFY! */\n");
	fprintf(f,"#include <Si53xx.h>\n");
	fprintf(f,"namespace Si53xx {\n");
	fprintf(f,"SettingVec %sSettings = {", prefix);

	for ( i = 0; i < sizeof(SI_TO_WRAP)/sizeof(SI_TO_WRAP[0]); i++ ) {
		fprintf(f,"%s\n\tSetting::mkSetting(\"%s\", %s, %d, %d, {",
			(0 == i ? "" : ","),
			SI_TO_WRAP[i].name,
			toAccess(SI_TO_WRAP[i].read_only, SI_TO_WRAP[i].self_clearing),
			SI_TO_WRAP[i].start_bit + SI_TO_WRAP[i].bit_length - 1,
			SI_TO_WRAP[i].start_bit);
		for ( j = 0; j < SI_TO_WRAP[i].reg_length; j++ ) {
			fprintf(f,"%s0x%03x", (0 == j ? "" : ","), SI_TO_WRAP[i].addr[j]);
		}
		fprintf(f,"})");
	}

#ifdef EXTRA_ENTRIES
	for ( i = 0; i < sizeof(EXTRA_ENTRIES)/sizeof(EXTRA_ENTRIES[0]); i++ ) {
		fprintf(f,",\n\tSetting::mkSetting(\"%s\", %s, %d, %d, {",
			EXTRA_ENTRIES[i].name,
			toAccess(EXTRA_ENTRIES[i].read_only, EXTRA_ENTRIES[i].self_clearing),
			EXTRA_ENTRIES[i].start_bit + EXTRA_ENTRIES[i].bit_length - 1,
			EXTRA_ENTRIES[i].start_bit);
		for ( j = 0; j < EXTRA_ENTRIES[i].reg_length; j++ ) {
			fprintf(f,"%s0x%03x", (0 == j ? "" : ","), EXTRA_ENTRIES[i].addr[j]);
		}
		fprintf(f,"})");
	}
#endif
	fprintf(f,"\n};\n");
	fprintf(f,"\n}");
	return 0;
}
