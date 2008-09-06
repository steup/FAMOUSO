CONFIG=linux
#CONFIG=windows/cross-mingw
#CONFIG=windows/native-mingw
#CONFIG=avr

COMPILER=$(word 2, $(subst /, ,$(CONFIG)))
PLATFORM=$(word 1, $(subst /, ,$(CONFIG)))

#$(info Compiler $(COMPILER))
#$(info PLatform $(PLATFORM))

