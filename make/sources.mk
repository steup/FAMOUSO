# -----------------------------------------------------------------------------
#
#                             SOURCES
#
# -----------------------------------------------------------------------------

SOURCES  = \
src/mw/common/AttributeList.cc \
src/mw/common/Attribute.cc \
src/mw/common/Event.cc \
src/mw/common/CallBack.cc \
src/mw/el/EventLayerCallback.cc

#$(shell find $(SRCDIR) -name "*.cc")
TOOLSRC  = $(shell find $(TOOLSDIR) -name "*.cc")
