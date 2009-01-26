# additional taging of the boost thread lib if it is
# cross-compiled as it is always the case in our
# development system
ifeq ($(PLATFORM), windows)
THREADTAG=_win32
endif

