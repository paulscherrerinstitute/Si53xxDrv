
all: tst

tst: TstDrv.o Si53xx.o
	$(CXX) -o $@ $^

Si53xx.o: Si53xx.cc Si53xx.h
	$(CXX) $(CXXFLAGS) -c -I. $<

TstDrv.o: TstDrv.cc Si53xx.h TstDrv.h

TstDrv.h: Si53xx.h


.PHONY: clean

clean:
	$(RM) Si53xx.o TstDrv.o
