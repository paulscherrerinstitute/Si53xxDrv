#!/usr/bin/env python3
import tabula
# tables where si5395 registers are defined
pages=[i for i in range(92,141)]
tabula.convert_into('si5395-94-92-family.pdf','regs_raw.orig.csv',output_format='csv',pages=pages)
