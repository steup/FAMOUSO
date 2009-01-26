include ../make/Makefile

LDFLAGS= $(LIB) $(ADDITIONAL_LIBS)

filtered=$(filter-out %.cc %.h %.py %Makefile,$(shell find . -maxdepth 1 -type f))

.phony: all clean

all:
	@echo usage: \"make programm-to-build\"

clean:
	@rm -rf $(filtered)

%:%.cc
	@$(RULEECHO) ; \
	$(CXX) $< -o $@.foobar $(CXXFLAGS) $(LDFLAGS)
	mv $@.foobar $(basename $@)$(SUFFIX)

ifneq ($(subst dist,,$(MAKECMDGOALS)),depend)
-include $(DEPENDDIR)/*
endif
