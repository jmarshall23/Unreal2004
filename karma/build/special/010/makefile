.PHONY: all components examples tutorials warning clean
.EXPORT_ALL_VARIABLES: ;

#
# Top level rules that enforce existence of PLATFORM variable
#

ifeq ($(strip $(PLATFORM)),)

all debug release check_release clean:
	@echo 'Environment variable PLATFORM must be set in order to compile.'

else

CONFIG = release

all: components examples tutorials ;

debug: CONFIG = debug
debug: all ;

release: CONFIG = release
release: all ;

check_release: CONFIG = check_release
check_release: all ;

clean: CONFIG = clean
clean: all ;


#
# Additional include paths for global include directories
#

ME_INCLUDE   := ../../../../include
GLUT_INCLUDE := ../../../../3rdParty/glut
ifeq ($(strip $(INCLUDE_PATH)),)
  INCLUDE_PATH := $(ME_INCLUDE):$(GLUT_INCLUDE)
else
  INCLUDE_PATH := $(ME_INCLUDE):$(GLUT_INCLUDE):$(INCLUDE_PATH)
endif



#
# Rules to compile the provided MeToolkit source code
#

components: warning
	cd components/Mdt/src       && ${MAKE} $(CONFIG)
	cd components/MeApp/src     && ${MAKE} $(CONFIG)
	cd components/MeGlobals/src && ${MAKE} $(CONFIG)
	cd components/MeMessage/src && ${MAKE} $(CONFIG)
	cd components/MeViewer2/src && ${MAKE} $(CONFIG)
	cd components/Mst/src       && ${MAKE} $(CONFIG)

.PHONY: components_debug components_check

components_debug: CONFIG = debug
components_debug: components ;

components_check: CONFIG = check_release
components_check: components ;


#
# Rules to compile the MathEngine toolkit examples
#

examples: warning
	cd $@ && sh configure && ${MAKE} $(CONFIG)

.PHONY: examples_debug examples_check

examples_debug: CONFIG = debug
examples_debug: examples ;

examples_check: CONFIG = check_release
examples_check: examples ;

#
# Rules to compile the MathEngine toolkit tutorials
#

tutorials: warning
	cd $@ && sh configure && ${MAKE} $(CONFIG)

.PHONY: tutorials_debug tutorials_check

tutorials_debug: CONFIG = debug
tutorials_debug: tutorials ;

tutorials_check: CONFIG = check_release
tutorials_check: tutorials ;


warning:
	@echo "**********************************************************"
	@echo "Please note: compilation here will NOT replace the files"
	@echo "  originally provided. The output will all be found within"
	@echo "  this subdirectory."
	@echo "**********************************************************"


endif
