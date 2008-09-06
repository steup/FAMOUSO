include ../../make/Makefile
INC=../include
FILE=famouso.i
GEN=generated

all:
	mkdir -p $(GEN)
	touch $(GEN)/__init__.py
	swig -python -c++ -outdir $(GEN) -module famouso -I../include/ -I../../include $(FILE)
	$(CXX) -Wall -fpic -c ../client/famouso.cc -o $(GEN)/famouso.o -I/usr/include/python2.5 -I../../include -I$(INC)
	$(CXX) -Wall -fpic -c famouso_wrap.cxx -o $(GEN)/famouso_wrap.o -I/usr/include/python2.5 -I../../include -I$(INC)
	$(CXX) -shared $(GEN)/famouso.o $(GEN)/famouso_wrap.o -o _$(LIBNAME).so $(LIB)

clean:
	rm -fr *.o *~ _famouso.so $(GEN) famouso_wrap.*
