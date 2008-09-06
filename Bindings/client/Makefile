include ../../make/Makefile

all: libfamouso.so


%.o:    %.cc
	$(RULEECHO) ; \
	$(CXX) -fPIC -c $(CFLAGS) -I./ -I../include -I../../include -o $@ $<

libfamouso.so: famouso.o
	$(RULEECHO) ; \
	$(CXX) -shared -Wl,-soname,libfamouso.so.0 -o libfamouso.so.0.1 famouso.o $(LIBFAMOUSO)
	@ln -sf libfamouso.so.0.1 libfamouso.so

clean:
	@rm -f *.o *.so libfamouso*

