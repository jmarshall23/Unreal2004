# Assuming GNU 'make' (>v3.77)

#------------------------------
#Platform independent Makefile
#------------------------------


#
# Sub-modules to be built from here
#

-include examples.def

ifeq '$(EXAMPLES)' ''

IRISPerformer	:=
ifeq '$(PLATFORM)' 'irix'
IRISPerformer	:= IRISPerformer
endif

EXAMPLES	?= \
		Chair \
		Hinge \
		HiSpeed \
		Prismatic \
		TriangleList
	
#	FixedPath \
#	BallHitsWall1 \
#	BallHitsWall2 \
#	BallHitsWall3 \
#	Bounce \
#	Bounce2 \
#	BoxCar \
#	CarTerrain \
#	ConvexPrims \
#	Drop \
#	HiSpeed2 \
#	HiSpeed2 \
#	KeaOnly \
#	LoadTutorial1 \
#	LoadTutorial2 \
#	LoadTutorial3 \
#	SaveTutorial1 \
#	SaveTutorial2 \
#	Snooker \
#	Spring 

endif


ALL_RELEASE	= $(foreach target_base, $(EXAMPLES), $(target_base)_rel)
ALL_DEBUG	= $(foreach target_base, $(EXAMPLES), $(target_base)_dbg)
ALL_CHECK_REL	= $(foreach target_base, $(EXAMPLES), $(target_base)_chk)
ALL_REALCLEAN	= $(foreach target_base, $(EXAMPLES), $(target_base)_rlc)
ALL_CLOBBER	= $(foreach target_base, $(EXAMPLES), $(target_base)_clb)
ALL_CLEAN	= $(foreach target_base, $(EXAMPLES), $(target_base)_cln)
ALL_CLEANISH	= $(foreach target_base, $(EXAMPLES), $(target_base)_cns)

MAKE_RULES	:= makefile.common

#
# Main targets
#

.PHONY: all help release debug

HELP1 := "Usage: make <target>"
HELP2 := "Where <target> is one of:"
HELP3 := "  { release  | debug | check_release } - Compile (if necessary)"
HELP4 := "  { all }                              - Compile all"
HELP5 := "  { cleanish | clean | clobber }       - Remove intermediates and executables"
HELP6 := "  { help }                             - This help"

help:
	@echo $(HELP1)
	@echo $(HELP2)
	@echo $(HELP3)
	@echo $(HELP4)
	@echo $(HELP5)
	@echo $(HELP6)
	@echo

all: $(MAKE_RULES) release

check_release: $(MAKE_RULES) $(ALL_CHECK_REL)
	-@find . -path './obj' -prune -o -path './bin' -prune \
		-o -name \*.tif -exec cp {} ./bin \; \
		-o -name \*.bmp -exec cp {} ./bin \; -printf .

ifeq '$(PLATFORM)' 'xbox'
	../build-pg/build_xbe.sh ../build-pg/MeXbImg.flags
endif

	@echo "finished $@ builds!"

release: $(MAKE_RULES) $(ALL_RELEASE)
	-@find . -path './obj' -prune -o -path './bin' -prune \
		-o -name \*.tif -exec cp {} ./bin \; \
		-o -name \*.bmp -exec cp {} ./bin \; -printf .

ifeq '$(PLATFORM)' 'xbox'
	../build-pg/build_xbe.sh ../build-pg/MeXbImg.flags
endif
	@echo "finished $@ builds!"

debug: $(MAKE_RULES) $(ALL_DEBUG)
	-@find . -path './obj' -prune -o -path './bin' -prune \
		-o -name \*.tif -exec cp {} ./bin \; \
		-o -name \*.bmp -exec cp {} ./bin \; -printf .

ifeq '$(PLATFORM)' 'xbox'
	../build-pg/build_xbe.sh ../build-pg/MeXbImg.flags
endif
	@echo "finished $@ builds!"


#
# Common core rules file, needed for most anything else
#

${MAKE_RULES}:
	echo sh ./configure

#
# Rules for cleanup
#

.PHONY: realclean clobber clean cleanish

realclean : $(MAKE_RULES) $(ALL_REALCLEAN)
	-@rm -rf bin.rel bin.dbg bin.chk
	@echo "all $@ed!"

clobber: $(MAKE_RULES) $(ALL_CLOBBER)
	-@rm -fr makefile.common;
	-@rm -fr config.cache config.log config.status;
	@echo "all $@ed!"

clean: $(MAKE_RULES) $(ALL_CLEAN)
	-@rm -fr  config.cache config.log config.status;
	@echo "all $@ed!"

cleanish: $(MAKE_RULES) $(ALL_CLEANISH)
	@echo "all $@ed!"


#
# Chaining rules
#

%_chk:
#	@echo Starting  $* - release
	@$(MAKE) -C$* check_release
#	@echo Completed $* - release

%_rel:
#	@echo Starting  $* - release
	@$(MAKE) -C$* release
#	@echo Completed $* - release

%_dbg:
#	@echo Starting  $* - debug
	@$(MAKE) -C$* debug
#	@echo Completed $* - debug

%_rlc:
#	@echo Starting  $* - realclean
	@$(MAKE) -C$* realclean
#	@echo Completed $* - realclean

%_clb:
#	@echo Starting  $* - clobber
	@$(MAKE) -C$* clobber
#	@echo Completed $* - clobber

%_cln:
#	@echo Starting  $* - clean
	@$(MAKE) -C$* clean
#	@echo Completed $* - clean

%_cns:
#	@echo Starting  $* - cleanish
	@$(MAKE) -C$* cleanish
#	@echo Completed $* - cleanish

