
CXXFLAGS=-g

all: tst

tst: TstDrv.o Si53xx.o Si5395Settings.o Si5395.o
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cc Si53xx.h
	$(CXX) $(CXXFLAGS) -c -I. $<

TstDrv.o: TstDrv.cc Si53xx.h TstDrv.h

TstDrv.h: Si5395.h

Si5395.o: Si5395.h


.PHONY: clean

clean:
	$(RM) Si53xx.o TstDrv.o Si5395Settings.o Si5395.o
