
CXXFLAGS=-g -fpic

SRCS=Si53xx.cc Si5395Settings.cc Si5395.cc pysi5395.cc TstDrv.cc

OBJS=$(SRCS:%.cc=%.o)
TGTS=pysi5395.so tst

all: $(TGTS)

tst: TstDrv.o Si53xx.o Si5395Settings.o Si5395.o
	$(CXX) $(CXXFLAGS) -o $@ $^

pysi5395_CXXFLAGS=-I/usr/include/python3.10

pysi5395.so: Si53xx.o Si5395Settings.o Si5395.o pysi5395.o
	$(CXX) -shared -o $@ $^

pysi5395.cc: pysi5395.pyx
	$(RM) $@
	cython3 --cplus -3 -o $@ $^

%.o: %.cc Si53xx.h
	echo $@
	$(CXX) $(CXXFLAGS) $($(patsubst %.o,%,$@)_CXXFLAGS) -c -I. $<

TstDrv.o: TstDrv.cc Si53xx.h TstDrv.h

TstDrv.h: Si5395.h

Si5395.o: Si5395.h


.PHONY: clean

clean:
	$(RM) $(OBJS) $(TGTS) pysi5395.cc
