###########################################################################
#
# MathEngine Canada Inc                                      (c) 2000-2001
#
#
# Image generation wrapper makefile
#
###########################################################################

ifeq '$(PLATFORM)' ''
.DEFAULT:
	@echo Environment variable PLATFORM must be defined.
else

MAKESYS_DIR := ../makerules
include $(MAKESYS_DIR)/makefile.defs

#INFORMER := $(warning Modifier = $(DIR_MODIFIER))

ifneq ($(strip $(BUILD_VER_STRING)),)
  BUILD_VER_STRING := _$(BUILD_VER_STRING)
endif

ifeq ($(findstring ${PLATFORM},win32-xbox-ngc),${PLATFORM})
    ARC_SUFFIX	=zip
else
    ARC_SUFFIX	=tar.gz
endif	

ARC_NAME	=$(PKG_NAME)_$(PLATFORM)$(BUILD_VER_STRING).$(ARC_SUFFIX)

# Main target

all:			developer_archive;			@echo $@ done
developer:		developer_archive;			@echo $@ done
images_to_generate:	developer_archive eval_archive;		@echo $@ done

pack_libs: PKG_NAME		= developer
pack_libs: LYT_NAME		= developer
pack_libs: ME_SOURCE_INCLUDED   = 0
pack_libs: LIBS_ONLY            = 1
pack_libs: WITH_DOCS		= 0
pack_libs: WITH_WITH_MEVIEWER2	= 0
pack_libs: WITH_WITH_MEVIEWER	= 0
pack_libs: WITH_ANY_SRC		= 0
pack_libs: WITH_LICENSE		= NO
pack_libs: GREASEMONKEY		= 0
pack_libs: WITH_RW_BRIDGE	= 0
pack_libs: PLUGIN		= 0
pack_libs: developer_generate developer_package;		@echo $@ done

developer_archive: PKG_NAME		= developer
developer_archive: LYT_NAME		= developer
developer_archive: ME_SOURCE_INCLUDED	= 1
developer_archive: LIBS_ONLY		= $(PACK_LIBS)
developer_archive: GREASEMONKEY		= $(CAN_GREASEMONKEY)
developer_archive: WITH_RW_BRIDGE	= 0
developer_archive: PLUGIN		= 0

ifeq '$(strip $(WITH_LICENSE))' ''
developer_archive: WITH_LICENSE		= NO
endif

developer_archive: developer_generate developer_package;	@echo $@ done

developer_clobber: PKG_NAME		= developer;

#
# Rules to create specific product images
#
# For each, override any of the following variables if necessary:
#   - PKG_NAME   = Base string for archive filename construction
#   - GEN_ROOT   = Path to the root of the target tree (from here)
#   - GEN_SOURCE = Path to the root of the source tree (from here)
#   - REL_ROOT   = Path to the root of the source tree (from within $GEN_ROOT)
#
# then invoke the implicit rule for generalized image generation
#

GEN_TREE   = ../../Images$(ME_VERSION)/$(PKG_NAME)$(DIR_MODIFIER)
REL_ROOT   = ../../../..
GEN_ARC    = $(GEN_TREE)/_archive
GEN_LOG    = $(GEN_TREE)/_log
GEN_ROOT   = $(GEN_TREE)/_image/metoolkit
GEN_SOURCE = ../..

ifeq '$(PLATFORM)' 'irix'
  CAN_PERFORMER         = 1
endif

ifeq '$(PLATFORM)' 'win32'
  CAN_GREASEMONKEY 	= 1
endif

ifeq '$(PLATFORM)' 'xbox'
  CAN_GREASEMONKEY      = 1
endif

ifeq '$(PLATFORM)' 'ps2'
  CAN_GREASEMONKEY 	= 0
endif

ifeq '$(PLATFORM)' 'ngc'
  CAN_GREASEMONKEY 	= 0
endif

####################################################################
#
# Excise need for GreaseMonkey if it's missing.
# Makes catching GM problem an effort, but what the hey!

ifeq '$(CAN_GREASEMONKEY)' '1'
ifeq '$(wildcard $(GEN_SOURCE)/MeDemos/GreaseMonkey/GreaseMonkey$(S_EXE))' ''
CAN_GREASEMONKEY	:= 0
endif
endif

# Bit flags for viewer...

WITH_MEVIEWER		:= 0
WITH_MEVIEWER2		:= 1
CODEWARRIOR             := 1


#################################################################
#
# Obsolete target
#
#
#eval: eval_archive
#
#eval_archive: PKG_NAME   = eval
#eval_archive: LYT_NAME   = developer
#eval_archive: ME_SOURCE_INCLUDED = 0
#eval_archive: GREASEMONKEY = $(CAN_GREASEMONKEY)
#eval_archive: SAMSTOY      = $(CAN_SAMSTOY)
#eval_archive: PLUGIN       = 0
#eval_archive: SCENELOADER  = 1
#eval_archive: WITH_LICENSE = YES
#eval_archive: eval_generate eval_package ;
#
#eval_clobber: PKG_NAME   = eval;


for3dsmax: PKG_NAME   = for3dsmax
for3dsmax: for3dsmax_archive
	[ -d "$(GEN_SOURCE)/../metoolkit" ] || mkdir $(GEN_SOURCE)/../metoolkit
	cp -a $(GEN_ROOT)/* $(GEN_SOURCE)/../metoolkit

for3dsmax_archive: PKG_NAME   = for3dsmax
for3dsmax_archive: LYT_NAME   = developer
for3dsmax_archive: ME_SOURCE_INCLUDED = 0
for3dsmax_archive: GREASEMONKEY   = 0
for3dsmax_archive: SAMSTOY        = 0
for3dsmax_archive: WITH_EXAMPLES  = 0
for3dsmax_archive: WITH_DOCS      = 0
for3dsmax_archive: WITH_EXTRAS    = 0
for3dsmax_archive: WITH_ANY_SRC   = 0
for3dsmax_archive: WITH_RW_BRIDGE = 0
for3dsmax_archive: PLUGIN         = 0
for3dsmax_archive: SCENELOADER    = 0
for3dsmax_archive: WITH_LICENSE   = NO
for3dsmax_archive: WITH_REALDEBUG = 1
for3dsmax_archive: for3dsmax_generate ;


######################################### Do not make changes below this line
#

#
# Generic rules to create/destroy an image
#
# Requires existence of Layouts/layout_<yourName>
#

.EXPORT_ALL_VARIABLES:

%_generate: genLayout_prefix.sh genLayout_suffix.sh Scripts
	[ -f Layouts/layout_$(LYT_NAME) ]
	@echo "******************************************"
	@echo "* Generating product image of type '$*'"
	@echo "*"
	@echo "*  -> Source directory=${GEN_SOURCE}"
	@echo "*  -> Result directory=${GEN_ROOT}"
	@echo "*"
	cat genLayout_platform.sh >  Scripts/generate_$*.sh
	cat genLayout_prefix.sh   >> Scripts/generate_$*.sh
	[ -s Scripts/generate_$*.sh ]
	[ -d "${GEN_LOG}" ] || mkdir -p "${GEN_LOG}"
	touch "${GEN_LOG}/generate.log"
	awk -v MODE=APPEND \
            -v OUTFILE=Scripts/generate_$*.sh  \
            -f makeLayout.awk < Layouts/layout_$(LYT_NAME)
	@echo AWK done
	cat genLayout_suffix.sh >> Scripts/generate_$*.sh
	@echo SCRIPT done
	@echo LIBS $(PACK_LIBS)
	GEN_ROOT=$(GEN_ROOT) sh Scripts/generate_$*.sh | tee $(GEN_LOG)/generate.log
	(cd $(GEN_ROOT) && chmod -R u+w .)
	@echo generation DONE
	@ exit `grep -c ^error $(GEN_LOG)/generate.log`


%_package:
ifeq (${ARC_SUFFIX},zip)
	-test -d $(GEN_ARC) || mkdir $(GEN_ARC); true
	-test -s $(GEN_ARC)/$(ARC_NAME) && rm -f $(GEN_ARC)/$(ARC_NAME); true
	cd $(GEN_ROOT)/.. && zip -8 -r $(ARC_NAME) *
	mv $(GEN_ROOT)/../$(ARC_NAME) $(GEN_ARC)
endif
ifeq (${ARC_SUFFIX},tar.gz)
	-test -d $(GEN_ARC) || mkdir $(GEN_ARC); true
	-test -s $(GEN_ARC)/$(ARC_NAME) && rm -f $(GEN_ARC)/$(ARC_NAME); true
	cd $(GEN_ROOT)/.. && tar cf - * | gzip -8 -c > $(ARC_NAME)
	mv $(GEN_ROOT)/../$(ARC_NAME) $(GEN_ARC)
endif	

%_clobber:
	@echo "******************************************"
	@echo "* Wiping product image of type '$*'"
	@echo "*"
	@echo "*"
	@echo "*  ->  Image directory=${GEN_TREE}"
	@echo "*"
	@echo "Sanity checking the name of the directory to wipe."
	@[ ! -z "${GEN_TREE}" ]
	@echo "Verifying the directory to clobber isn't a pillar of the system."
	@[ "${GEN_TREE}" != "/"     ]
	@[ "${GEN_TREE}" != "/au"   ]
	@[ "${GEN_TREE}" != "/am"   ]
	@[ "${GEN_TREE}" != "/var"  ]
	@[ "${GEN_TREE}" != "/tmp"  ]
	@[ "${GEN_TREE}" != "/lib"  ]
	@[ "${GEN_TREE}" != "/opt"  ]
	@[ "${GEN_TREE}" != "/usr"  ]
	@[ "${GEN_TREE}" != "/mnt"  ]
	@[ "${GEN_TREE}" != "/etc"  ]
	@[ "${GEN_TREE}" != "/bin"  ]
	@[ "${GEN_TREE}" != "/dev"  ]
	@[ "${GEN_TREE}" != "/boot" ]
	@[ "${GEN_TREE}" != "/home" ]
	@[ "${GEN_TREE}" != "/sbin" ]
	[ ! -d "${GEN_TREE}" ] || rm -rf "${GEN_TREE}"

Scripts:
	mkdir Scripts

endif
