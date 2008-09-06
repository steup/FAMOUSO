include ../../make/Makefile
INC=../include
FILE=famouso.i
GEN=generated

ifeq ($(PLATFORM),windows)
 LDFLAGS+=-Wl,--add-stdcall-alias -o $(LIBNAME).dll
else
 CXXFLAGS+=-fpic
 LDFLAGS+=-o lib$(LIBNAME).so
endif


all:
	mkdir -p $(GEN)
	swig -csharp -c++ -outdir $(GEN) -namespace famouso -I../include/ $(FILE)
	$(CXX) $(CXXFLAGS) -c ../client/famouso.cc -o $(GEN)/famouso.o -I../../include -I$(INC)
	$(CXX) $(CXXFLAGS) -c famouso_wrap.cxx -o $(GEN)/famouso_wrap.o -I../../include -I$(INC)
	$(CXX) -shared $(GEN)/famouso.o $(GEN)/famouso_wrap.o $(LDFLAGS) $(LIB) $(ADDITIONAL_LIBS)
	mcs -out:Subscriber.exe $(GEN)/*.cs Subscriber.cs
	mcs -out:Publisher.exe $(GEN)/*.cs Publisher.cs

clean:
	rm -fr $(GEN) famouso_wrap.* libfamouso.so famouso.dll *.exe

