# -*- makefile -*- vim:noet:ts=8:sw=4:ft=make:
###########################################################################
# Emasculate the script, so it can "do no harm"... :^)
#
#      MAKE := echo
#
###########################################################################

export WITH_DEBUG WITH_LICENSE WITH_PLUGIN

PACKAGES	:= Mst Mdt MdtBcl MdtKea MeGlobals MeApp \
                   MeAssetDB MeAssetDBXMLIO MeAssetFactory MeXML \
		   MeViewer2
PACKAGES_CD	:= Mcd 
PACKAGES_MPS	:= Mps
PACKAGES_MPA	:= Mpa
PACKAGES_ALL	:= ${PACKAGES} ${PACKAGES_CD}
EXAMPLESDIR	:= MeSamples
TUTORIALDIR	:= MeTutorials
DEMOSDIR	:= MeDemos
PLUGINSDIR	:= MePlugins

ifneq ($(strip $(WITH_MPS)),)
PACKAGES_ALL += ${PACKAGES_MPS}
endif
ifneq ($(strip $(WITH_MPA)),)
PACKAGES_ALL += ${PACKAGES_MPA}
endif
WITH_PLUGIN 	:= NO
ifeq '$(strip $(WITH_LICENSE))' ''
WITH_LICENSE	:= NO
endif
ifeq '$(strip $(WITH_D3D))' ''
WITH_D3D        := YES  # Now the default in MeViewer2 makefile
endif
ifeq '$(strip $(WITH_DEBUG))' ''
WITH_DEBUG	:= NO
endif

SRC_ROOT := ../..

COMMON_MAKEFILE := ../makerules/makefile.common

.PHONY: all       full            opt             default
.PHONY: developer
.PHONY: release   debug           check_release
.PHONY: dynamics  dynamics_debug  dynamics_check
.PHONY: collision collision_debug collision_check
.PHONY: examples  tutorials       demos
.PHONY: documentation


#########################
#
# Expected top-level and default rules
#

ifeq ($(strip $(PLATFORM)),)
noPlatformFound:
	@echo "fatal error: environment variable 'PLATFORM' unset"
	@exit 1
endif

default:
	@echo "usage: make <target>"
	@echo
	@echo "where target is one (or more) of the following:"
	@echo "  all          (no documentation, no debug samples, no demos)"
	@echo "  full         (no documentation, debug samples and demos)"
	@echo "  opt          (full, with optimization for PII/PIII)"
	@echo "  complete     (all + documentation)"
	@echo "  complete_opt (opt + documentation)"
	@echo "  developer    (=opt)"
	@echo "  list         (List modules that build be built)"
	@echo ""
	@echo "  release   debug           check_release"
	@echo "  dynamics  dynamics_debug  dynamics_check"
	@echo "  collision collision_debug collision_check"
	@echo "  clean     cleanish        clobber"
	@echo

# Careful with the target specific variables: they only apply
# the _commands_ of a target, so using 'ifeq' to test them is
# pointless.

developer: WITH_DEBUG=NO
developer: complete_opt

all: 
	${MAKE} release
	${MAKE} check_release
	if test '$(WITH_LICENSE)' = 'YES'; \
	then ${MAKE} LL=_l2 ${OPTIMISATION} -f makefile.license; \
	else : dont do licensing; \
	fi
	if test '$(WITH_DEBUG)' = 'NO'; \
	then : dont compile for debugging; \
	else ${MAKE} debug; \
	fi
	${MAKE} examples
	${MAKE} tutorials
	@echo "totally built!"

developer: WITH_DEBUG=NO
developer: complete_opt

complete_opt: opt
	${MAKE} documentation
	@echo "REALLY totally built!"

full: all
	if test '$(WITH_DEBUG)' = 'NO';\
	then : dont compile for debugging; \
	else ${MAKE} examples_debug tutorials_debug; \
	fi
	${MAKE} demos
	if test '$(WITH_PLUGINS)' = 'YES'; \
	then ${MAKE} plugins; else : dont do plugins; \
	fi
	@echo "fully totally built!"

opt: OPTIMISATION=mefpu=x86:sse
opt: VISIBLE_COMMANDS=all
opt: full

complete_opt: opt
	${MAKE} documentation
	@echo "REALLY totally built!"

full: 
	${MAKE} release
	${MAKE} check_release
ifeq '$(WITH_LICENSE)' 'YES'
	${MAKE} LL=_l2 ${OPTIMISATION} -f makefile.license
endif
	${MAKE} examples
	${MAKE} tutorials
ifneq '$(WITH_DEBUG)' 'NO'
	${MAKE} debug
	${MAKE} examples_debug
	${MAKE} tutorials_debug
endif
	${MAKE} demos
ifneq '$(WITH_PLUGIN)' 'NO'
	${MAKE} plugins
endif
	@echo "totally built!"

#########################
#
# By-module set rules
#

dynamics: COMPILE_TYPE = release
dynamics: $(foreach pkg, ${PACKAGES}, ${pkg}_rel)
	@echo "totally build $@!"

dynamics_debug: COMPILE_TYPE = debug
dynamics_debug: $(foreach pkg, ${PACKAGES}, ${pkg}_dbg)
	@echo "totally build $@!"

dynamics_check: COMPILE_TYPE = check_release
dynamics_check: $(foreach pkg, ${PACKAGES}, ${pkg}_relchk)
	@echo "totally build $@!"

collision: COMPILE_TYPE = release
collision: $(foreach pkg, ${PACKAGES_CD}, ${pkg}_rel)
	@echo "totally build $@!"

collision_debug: COMPILE_TYPE = debug
collision_debug: $(foreach pkg, ${PACKAGES_CD}, ${pkg}_dbg)
	@echo "totally build $@!"

collision_check: COMPILE_TYPE = check_release
collision_check: $(foreach pkg, ${PACKAGES_CD}, ${pkg}_relchk)
	@echo "totally build $@!"

examples: COMPILE_TYPE = release
examples: compile_samples_in_$(EXAMPLESDIR)
	@echo "totally build $@!"

examples_debug: COMPILE_TYPE = debug
examples_debug: compile_samples_in_$(EXAMPLESDIR)
	@echo "totally build $@!"

examples_check: COMPILE_TYPE = check_release
examples_check: compile_samples_in_$(EXAMPLESDIR)
	@echo "totally build $@!"

tutorials: COMPILE_TYPE = release
tutorials: compile_samples_in_$(TUTORIALDIR)
	@echo "totally build $@!"

tutorials_debug: COMPILE_TYPE = debug
tutorials_debug: compile_samples_in_$(TUTORIALDIR)
	@echo "totally build $@!"

tutorials_check: COMPILE_TYPE = check_release
tutorials_check: compile_samples_in_$(TUTORIALDIR)
	@echo "totally build $@!"


demos: COMPILE_TYPE= release
demos: compile_demos_in_$(DEMOSDIR)
	@echo "totally build $@!"

plugins: COMPILE_TYPE= release
plugins: compile_plugins_in_$(PLUGINSDIR)
	@echo "totally build $@!"

#########################
#
# By-compile type rules
#

list: $(foreach module, ${PACKAGES_ALL}, ${module}_lst)
	@/bin/true

release: COMPILE_TYPE = release
release: $(foreach module, ${PACKAGES_ALL}, ${module}_rel)
	@echo "totally build $@!"
	-@date

debug: COMPILE_TYPE = debug
debug: $(foreach module, ${PACKAGES_ALL}, ${module}_dbg)
	@echo "totally build $@!"
	-@date

check_release: COMPILE_TYPE = check_release
check_release: $(foreach module, ${PACKAGES_ALL}, ${module}_relchk)
	@echo "totally build $@!"
	-@date

#
# Rules for cleaning up
#

.PHONY: realclean clobber clean cleanish

realclean: COMPILE_TYPE = realclean
realclean: $(foreach module, ${PACKAGES_ALL}, ${module}_rcln) \
          forced_samples_in_$(EXAMPLESDIR) forced_samples_in_$(TUTORIALDIR)
	rm -f ../makerules/config.[cls]* ../makerules/makefile.common
	@echo "totally $@ed!"

clobber:  COMPILE_TYPE = clobber
clobber:  $(foreach module, ${PACKAGES_ALL}, ${module}_clb) \
          forced_samples_in_$(EXAMPLESDIR) forced_samples_in_$(TUTORIALDIR)
	rm -f ../makerules/config.[cls]* ../makerules/makefile.common
	@echo "totally $@ed!"

clean:    COMPILE_TYPE = clean 
clean:    $(foreach module, ${PACKAGES_ALL}, ${module}_cln) \
          forced_samples_in_$(EXAMPLESDIR) forced_samples_in_$(TUTORIALDIR)
	@echo "totally $@ed!"

cleanish: COMPILE_TYPE = cleanish 
cleanish: $(foreach module, ${PACKAGES_ALL}, ${module}_cns) \
          forced_samples_in_$(EXAMPLESDIR) forced_samples_in_$(TUTORIALDIR)
	@echo "totally $@ed!"


#################################################################
#
# Flex me up!
#

flex:  full $(COMMON_MAKEFILE)
	make INCLUDE_LICENSING=1 -C$(SRC_ROOT)/MdtKea/src release
	make INCLUDE_LICENSING=1 -C$(SRC_ROOT)/MdtKea/src debug
	make INCLUDE_LICENSING=1 -C$(SRC_ROOT)/MdtKea/src check_release
	make INCLUDE_LICENSING=1 -C$(SRC_ROOT)/Mcd/src/frame release
	make INCLUDE_LICENSING=1 -C$(SRC_ROOT)/Mcd/src/frame debug
	make INCLUDE_LICENSING=1 -C$(SRC_ROOT)/Mcd/src/frame check_release
	@/bin/true

#
# Generic rules for chaining execution
#

.EXPORT_ALL_VARIABLES:

%_lst:
	@echo "$*"

%_rel  %_dbg  %_relchk: process_target_%
	@/bin/true

%_cln  %_clb  %_cns %_rcln: forced_process_target_%
	@/bin/true

# This rule isn't needed any more
%_cfg:
	cd ${SRC_ROOT}/$*/src && sh configure
	@echo "configure completed"


#
# The "real work"
#

process_target_%: $(COMMON_MAKEFILE)
	@echo "***************************** [ Processing $* - $(COMPILE_TYPE)"
	${MAKE} -C${SRC_ROOT}/$*/src ${OPTIMISATION} $(COMPILE_TYPE)
	@echo "***************************** [ Completed  $* - $(COMPILE_TYPE)"
	@echo

forced_process_target_%: $(COMMON_MAKEFILE)
	@echo "***************************** [ Processing $* - $(COMPILE_TYPE)"
	-@${MAKE} -C${SRC_ROOT}/$*/src ${OPTIMISATION} $(COMPILE_TYPE)
	@echo "***************************** [ Completed  $* - $(COMPILE_TYPE)"
	@echo

compile_samples_in_%: $(COMMON_MAKEFILE)
	@echo "***************************** [ Processing $* - $(COMPILE_TYPE)"
	@${MAKE} -C${SRC_ROOT}/$* ${OPTIMISATION} $(COMPILE_TYPE)
	@echo "***************************** [ Completed  $* - $(COMPILE_TYPE)"

forced_samples_in_%: $(COMMON_MAKEFILE)
	@echo "***************************** [ Processing $* - $(COMPILE_TYPE)"
	-@${MAKE} -C${SRC_ROOT}/$* ${OPTIMISATION} $(COMPILE_TYPE)
	@echo "***************************** [ Completed  $* - $(COMPILE_TYPE)"
	@echo

compile_demos_in_%: $(COMMON_MAKEFILE)
	@echo "***************************** [ Processing $* - $(COMPILE_TYPE)"
	@${MAKE} -C${SRC_ROOT}/$* ${OPTIMISATION} $(COMPILE_TYPE)
	@echo "***************************** [ Completed  $* - $(COMPILE_TYPE)"

compile_plugins_in_%: $(COMMON_MAKEFILE)
	@echo "***************************** [ Processing $* - $(COMPILE_TYPE)"
	@${MAKE} -C${SRC_ROOT}/$*/3dsmax/src ${OPTIMISATION} $(COMPILE_TYPE)
	@echo "***************************** [ Completed  $* - $(COMPILE_TYPE)"

documentation: COMPILE_TYPE = release
documentation: $(COMMON_MAKEFILE)
	@echo "***************************** [ Processing $* - $(COMPILE_TYPE)"
	@${MAKE} -C${SRC_ROOT}/documentation/build ${OPTIMISATION} $(COMPILE_TYPE)
	@echo "***************************** [ Completed  $* - $(COMPILE_TYPE)"
	@echo
	@echo "totally build $@!"

$(COMMON_MAKEFILE):
	make -C$(@D) $(@F)

