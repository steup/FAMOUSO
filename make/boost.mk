$(LIBBOOST):
	@echo Building needed Boost for $(PLATFORM)
	./externals/buildBoost.sh $(LIBDIR) $(INCDIR) $(PLATFORM).$(COMPILER)
	@$(RANLIB) $(LIBDIR)/*

