include ../ilixi-env.mk

DIRECTFB_APP_SOURCES := \
	$(ILIXI_SOURCE)/apps/calc/Calc.cpp	\
	$(ILIXI_SOURCE)/apps/calc/Node.cpp	\
	$(ILIXI_SOURCE)/apps/calc/NumNode.cpp	\
	$(ILIXI_SOURCE)/apps/calc/OpNode.cpp	\
	$(ILIXI_SOURCE)/apps/calc/Pair.cpp	\
	$(ILIXI_SOURCE)/apps/calc/Parser.cpp	\
	$(ILIXI_SOURCE)/apps/calc/UnaryNode.cpp	\
	$(ILIXI_SOURCE)/apps/calc/Util.cpp

include ../ilixi.mk

