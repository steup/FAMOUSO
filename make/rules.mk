RULEECHO = echo "$(notdir $<) -> $@"

# How to compile a C++ file.
%.o:    %.cc
	@$(RULEECHO) ; \
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

# How to compile a C file.
%.o:    %.c
	@$(RULEECHO) ; \
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) -o $@ -c $<

# How to compile a C file.
%.o:    %.C
	@$(RULEECHO) ; \
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) -o $@ -c $<

# How to assemble a C++ file.
%.s:    %.cc
	@$(RULEECHO) ; \
	$(CXX) $(ASMOUT) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

# How to compile an assembler file.
%.o: %.S
	@$(RULEECHO) ; \
	$(CC) -c $< -o $@
