#!/usr/bin/env python3
import tabula
# tables where si5395 registers are defined
pages=[i for i in range(92,141)]
ofnam='regs_raw.orig.csv'
tabula.convert_into('si5395-94-92-family.pdf',ofnam,output_format='csv',pages=pages)
print("Extracted table into {} -- must tweak manually (see README.fixes)".format(ofnam))
