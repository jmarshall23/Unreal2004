# Assuming GNU 'make' (>v3.77)

#------------------------------
#Platform independent Makefile
#------------------------------

MCD_BUILD := frame primitives 

MCD_BUILD += QHull
MCD_BUILD += convex
MCD_BUILD += util

#MCD_BUILD += gjk
#MCD_BUILD += terrain
#MCD_BUILD += particle

# This is no longer current, and in any case uses STL
# which causes trouble on NGC etc.
#MCD_BUILD += mesh

# List containing all directories that have a 'makefile'
MCD_CLEAN := util frame rwbsp convex terrain particle primitives QHull mesh gjk convex

# If WITH_MPS is defined in the environment then the right thing
# will happen building particle systems here, in MeViewer and Mcd/examples
ifneq ($(strip $(WITH_MPS)),)
MCD_BUILD += particle
endif

ifeq '$(PLATFORM)' 'win32'
#MCD_BUILD += rwbsp
endif

ifeq '$(PLATFORM)' 'ps2'
#MCD_BUILD += rwbsp
endif

ifeq '$(PLATFORM)' 'irix'
MCD_BUILD += performer
endif

all: default release debug
default:
	@echo "alternatives: make <release|debug|cleanish|clean|clobber>"

release: ${MAKE_RULES} $(foreach dir, ${MCD_BUILD}, ${dir}_rel)
	@echo "finished release builds!"

check_release: ${MAKE_RULES} $(foreach dir, ${MCD_BUILD}, ${dir}_ckrel)
	@echo "finished check builds!"

debug: ${MAKE_RULES} $(foreach dir, ${MCD_BUILD}, ${dir}_dbg)
	@echo "finished debug builds!"

clobber: clean $(foreach dir, ${MCD_CLEAN}, ${dir}_clb)
	-@rm -f config.cache
	-@rm -f config.status
	-@rm -f config.log
	-@rm -f makefile.common;
	@echo "all clobbered!"

clean: $(foreach dir, ${MCD_CLEAN}, ${dir}_cln)
	-@rm -rf ../lib;
	@echo "all cleaned!"

cleanish: $(foreach dir, ${MCD_CLEAN}, ${dir}_cns)
	@echo "all cleanished!"

realclean: $(foreach dir, ${MCD_CLEAN}, ${dir}_rcln)
	@echo "all cleanished!"

# Special rules for Renderware library that will
# fail "gracefully" if Renderware is not available

rwbsp_rel:
	-@${MAKE} -Crwbsp release
rwbsp_dbg:
	-@${MAKE} -Crwbsp debug
rwbsp_ckrel:
	-@${MAKE} -Crwbsp check_release
%_rel:
	@${MAKE} -C$* release
%_ckrel:
	@${MAKE} -C$* check_release
%_dbg:
	@${MAKE} -C$* debug
%_dpnd:
	@${MAKE} -C$* depend
%_clb:
	-@${MAKE} -C$* clobber
%_cln:
	-@${MAKE} -C$* clean
%_cns:
	-@${MAKE} -C$* cleanish
%_rcln:
	-@${MAKE} -C$* realclean
