CXXFLAGS+=-I../../include -g -Wall
# -fpack-struct
CFLAGS+=-I../../include -g -Wall

LDFLAGS= ../../lib/i386-redhat-linux/gcc-4.1.2/libboost_thread-mt.a -lpthread -lpcan

filtered=$(filter-out %.cc %Makefile,$(shell find . -type f))

.phony: all clean

all:
	@echo usage: \"make programm-to-build\"

clean:
	@rm -rf $(filtered)

%:%.cc
	$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS)
