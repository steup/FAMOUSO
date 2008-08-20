include ../../make/Makefile
ADDITIONAL_CFLAGS+= -I./include

TOOLS=sendEvent.cc distance.cc

TOOLBIN = $(TOOLS:.cc=.foobar)
TOOLCLEAR = $(TOOLS:.cc=$(SUFFIX))

all: $(TOOLBIN)

%.foobar: %.o
	echo "$< -> $(basename $@)$(SUFFIX)"; \
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIB) $(ADDITIONAL_LIBS) && \
	mv $@ $(basename $@)$(SUFFIX)

clean:
	rm -f *.o *~ $(TOOLCLEAR)
