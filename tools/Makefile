include ../make/Makefile

LDFLAGS= $(LIB) $(ADDITIONAL_LIBS)

filtered=$(filter-out %.cc %.h %.py %Makefile,$(shell find . -maxdepth 1 -type f))

.phony: all clean

all:
	@echo usage: \"make programm-to-build\"

clean:
	@rm -rf $(filtered)

# rule for creating the object file
$(MODULEDIR)/%.o : %.cc
	@$(RULEECHO) ; \
	$(CXX) -c $(CXXFLAGS) -o $@ $<

# rule for linking the application and give it
# the correct name dependend of the platform
%:$(MODULEDIR)/%.o
	@$(RULEECHO) ; \
	$(CXX) $< -o $@.foobar $(CXXFLAGS) $(LDFLAGS) ; \
	mv $@.foobar $(basename $@)$(SUFFIX)

ifneq ($(subst dist,,$(MAKECMDGOALS)),depend)
-include $(DEPENDDIR)/*
endif
