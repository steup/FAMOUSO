include ../../make/Makefile

all: libfamouso.so


%.o:    %.c
	$(RULEECHO) ; \
	$(CC) -fPIC -c $(CFLAGS) -I../include -I../../include -o $@ $<

libfamouso.so: famouso.o
	@$(RULEECHO) ; \
	$(CXX) -shared -Wl,-soname,libfamouso.so.0 -o libfamouso.so.0.1 famouso.o
	@ln -sf libfamouso.so.0.1 libfamouso.so

clean:
	@rm -f *.o *.so libfamouso*

