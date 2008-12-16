include ../../make/Makefile
ADDITIONAL_CFLAGS+= -I../include

LDFLAGS= $(LIB) $(ADDITIONAL_LIBS)

filtered=$(filter-out %.cc %.h %.py %Makefile,$(shell find . -maxdepth 1 -type f))

.phony: all clean

all:
	@echo usage: \"make programm-to-build\"

%:%.cc
	@$(RULEECHO) ; \
	$(CXX) $< -o $@.foobar $(CXXFLAGS) $(LDFLAGS) && \
	mv $@.foobar $(basename $@)$(SUFFIX)

clean:
	rm -f $(filtered)
