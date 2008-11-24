include ../make/Makefile

LDFLAGS= $(LIB) $(ADDITIONAL_LIBS)

filtered=$(filter-out %.cc %.h %Makefile,$(shell find . -type f))

.phony: all clean

all:
	@echo usage: \"make programm-to-build\"

clean:
	@rm -rf $(filtered)

%:%.cc
	@$(RULEECHO) ; \
	$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS)
