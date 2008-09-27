$(LIBBOOST):
	@echo Building needed Boost for $(PLATFORM)
	./externals/buildBoost.sh $(LIBDIR) $(INCDIR) $(EXTENSION)
	@$(RANLIB) $(LIBDIR)/*

