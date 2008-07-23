$(LIBBOOST):
	@echo Baue
	./externals/buildBoost.sh $(LIBDIR) $(INCDIR) $(PLATFORM)
	@$(RANLIB) $@

