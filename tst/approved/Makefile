include ../../make/Makefile

LDFLAGS= $(LIB) $(ADDITIONAL_LIBS)
CXXFLAGS+= -O3

filtered=$(filter-out %.cc %.h %.py %Makefile,$(shell find . -maxdepth 1 -type f))

.phony: all clean

all:
	@echo usage: \"make programm-to-build\"

clean:
	@rm -rf $(filtered)

%:%.cc
	@$(RULEECHO) ; \
	$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS)
