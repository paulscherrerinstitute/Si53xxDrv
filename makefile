SOURCES=Si53xx.cc Si5395Settings.cc Si5395.cc Si53xxI2c.cc TstDrv.cc $(patsubst %.h,%.cc,$(wildcard Si5395-RevA*Registers.h))

ifneq ($(PSIMAKE),YES)

SOURCES+=pysi5395.cc

CXXFLAGS=-g -fpic -std=c++11 -Wall

DESIGN_H=$(wildcard Si5395-RevA-*Registers.h)

OBJS=$(SOURCES:%.cc=%.o) $(DESIGN_H:%.h=%.o)
PROGS=$(addprefix $(ODIR),Tst CsvDiff)
TGTS=$(addprefix $(ODIR),pysi5395.so) $(PROGS)

all: $(TGTS)

$(DESIGN_H:%.h=%.cc):%.cc:%.h Si5395Design.cc.in
	$(RM) $@
	echo "#include <$<>"             >> $@
	cat  Si5395Design.cc.in          >> $@


$(PROGS):%:%.o $(addprefix $(ODIR),TstDrv.o Si53xx.o Si5395Settings.o Si5395.o Si53xxI2c.o)
	$(CXX) $(CXXFLAGS) -o $@ $^

pysi5395_CXXFLAGS=-I/usr/include/python3.10

$(ODIR)pysi5395.so: $(addprefix $(ODIR),Si53xx.o Si5395Settings.o Si5395.o pysi5395.o TstDrv.o Si53xxI2c.o $(DESIGN_H:%.h=%.o))
	$(CXX) -shared -o $@ $^

$(ODIR)pysi5395.cc: pysi5395.pyx
	$(RM) $@
	cython3 --cplus -3 -o $@ $^

$(ODIR)pysi5395.o: $(ODIR)pysi5395.cc
	$(CXX) $(CXXFLAGS) $($(patsubst $(ODIR)%.o,%,$@)_CXXFLAGS) -c -o $@ -I. $<

$(ODIR)%.o: %.cc Si53xx.h
	echo $@
	$(CXX) $(CXXFLAGS) $($(patsubst %.o,%,$@)_CXXFLAGS) -c -o $@ -I. $<

TstDrv.o: TstDrv.cc Si53xx.h TstDrv.h

TstDrv.h: Si5395.h

Si5395.o: Si5395.h Si5395-RevA-cio_timing_base-CIO0100-Registers.h Si5395Design.h

Si5395Settings.cc: $(and $(wildcard Si5395-RevA-Regmap.h),$(ODIR)si5395_reg_extract)
	$(RM) $@
	./$^ > $@

# disable warning about assigning string constants to a char*;
# the regmap header is legay C (= ill-written).
$(ODIR)si5395_reg_extract: Si5395-RevA-Regmap.h reg_extract.cc
	cat $^ | $(CXX) -o $@ -xc++ -DPREFIX='"Si5395"' -DSI_TO_WRAP=si5395_reva_settings -Wno-write-strings -

.PHONY: clean

clean:
	$(RM) $(addprefix $(ODIR),$(OBJS) $(TGTS) pysi5395.cc si5395_reg_extract $(addsuffix .o,$(notdir $(PROGS)))) $(DESIGN_H:%.h=%.cc)
	$(RM) makefile.dep

makefile.dep: $(SOURCES) $(addsuffix .cc,$(notdir $(PROGS))) reg_extract.cc
	$(RM) $@
	$(CXX) $(pysi5395_CXXFLAGS) -MM $^ > $@

-include makefile.dep
endif
