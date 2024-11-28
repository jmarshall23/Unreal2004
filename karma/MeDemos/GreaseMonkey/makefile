# -*- makefile -*- vim:noet:ts=8:sw=4:ft=make:
################################################################
#
# Wrapper makefile for GreaseMonkey
#
#   This makefile wraps the build of the GreaseMonkey demo
#   For Windows, it chains to the DSP.
#   For PS2, it chains to makefile.ps2
#   For anything else, it complains.
#   NOTE: It also filters the conditions of the requested
#         build, as this demos might not properly function
#         in come configs.
#
#

.PHONY: all release debug check_release clean realclean clobber

MAKESYS_DIR 		 =../../build/makerules
MAKE_DEFS		:=$(MAKESYS_DIR)/makefile.defs

-include $(MAKE_DEFS)

ifeq '$(PLATFORM)_$(DEF_C_LIBRARY)_$(DEF_PRECISION)' 'win32_STATIC_SINGLE'
    MONKEY_FINE		:=1
    SAFE_C_LIBRARY	:=STATIC
    SAFE_PRECISION	:=SINGLE
endif

ifeq '$(PLATFORM)_$(DEF_C_LIBRARY)_$(DEF_PRECISION)' 'xbox_STATIC_SINGLE'
    MONKEY_FINE 	:=1
    SAFE_C_LIBRARY	:=STATIC
    SAFE_PRECISION	:=SINGLE
endif

ifeq '$(PLATFORM)_$(DEF_PRECISION)' 'ps2_SINGLE'
    MONKEY_FINE		:=1
    SAFE_C_LIBRARY	:=$(DEFAULT_C_LIBRARY)
    SAFE_PRECISION	:=SINGLE
endif

ifeq '$(PLATFORM)_$(DEF_PRECISION)' 'ngc_SINGLE'
    MONKEY_FINE		:=0
    SAFE_C_LIBRARY	:=$(DEFAULT_C_LIBRARY)
    SAFE_PRECISION	:=SINGLE
endif

ifneq '$(MONKEY_FINE)' '1'

.DEFAULT:
ifeq '$(PLATFORM)' 'win32'
	@echo "GreaseMonkey($(PLATFORM)): unavailable configuration: C runtime=$(DEF_C_LIBRARY) precision=$(DEF_PRECISION)"
	@echo "GreaseMonkey($(PLATFORM)): only available for: C runtime=$(SAFE_C_LIBRARY) precision=$(SAFE_PRECISION)"
endif
ifeq '$(PLATFORM)' 'xbox'
	@echo "GreaseMonkey($(PLATFORM)): unavailable configuration: C runtime=$(DEF_C_LIBRARY) precision=$(DEF_PRECISION)"
	@echo "GreaseMonkey($(PLATFORM)): only available for: C runtime=$(SAFE_C_LIBRARY) precision=$(SAFE_PRECISION)"
endif
ifeq '$(PLATFORM)' 'ps2'
	@echo "GreaseMonkey($(PLATFORM)): unavailable configuration: precision=$(DEF_PRECISION)"
	@echo "GreaseMonkey($(PLATFORM)): only available for: precision=$(SAFE_PRECISION)"
endif
ifeq '$(PLATFORM)' 'ngc'
	@echo "GreaseMonkey($(PLATFORM)): unavailable configuration: C runtime=$(DEF_C_LIBRARY) precision=$(DEF_PRECISION)"
	@echo "GreaseMonkey($(PLATFORM)): only available for: C runtime=$(SAFE_C_LIBRARY) precision=$(SAFE_PRECISION)"
endif

else

default:			GreaseMonkey_for_$(PLATFORM);
release:			GreaseMonkey_release_for_$(PLATFORM);
debug:				GreaseMonkey_debug_for_$(PLATFORM);
realclean clobber clean:	GreaseMonkey_clean_for_$(PLATFORM);

check_release:
	@echo "'check_release' doesn't yet apply to this demo."

# WIN32

GreaseMonkey_for_win32: GreaseMonkey_release_for_win32;

GreaseMonkey_release_for_win32:
	msdev GreaseMonkey.dsp /MAKE "GreaseMonkey - Win32 Release" /USEENV

GreaseMonkey_debug_for_win32:
	msdev GreaseMonkey.dsp /MAKE "GreaseMonkey - Win32 Debug" /USEENV

GreaseMonkey_clean_for_win32:
	msdev GreaseMonkey.dsp /MAKE "GreaseMonkey - Win32 Release" /CLEAN
	msdev GreaseMonkey.dsp /MAKE "GreaseMonkey - Win32 Debug"   /CLEAN

# XBOX

GreaseMonkey_for_xbox: GreaseMonkey_release_for_xbox ;

GreaseMonkey_release_for_xbox:
	msdev GreaseMonkeyXb.dsp /MAKE "GreaseMonkeyXb - Xbox Release" /USEENV
	../../build-pg/build_xbe.sh ../../build-pg/MeXbImg.flags

GreaseMonkey_debug_for_xbox:
	msdev GreaseMonkeyXb.dsp /MAKE "GreaseMonkeyXb - Xbox Debug" /USEENV
	../../build-pg/build_xbe.sh ../../build-pg/MeXbImg.flags


GreaseMonkey_clean_for_xbox:
	msdev GreaseMonkeyXb.dsp /MAKE "GreaseMonkeyXb - Xbox Release" /CLEAN
	msdev GreaseMonkeyXb.dsp /MAKE "GreaseMonkeyXb - Xbox Debug"   /CLEAN

# PS2

GreaseMonkey_for_ps2: GreaseMonkey_release_for_ps2 ;

GreaseMonkey_release_for_ps2:
	${MAKE} -f makefile.ps2

GreaseMonkey_debug_for_ps2:
	${MAKE} DEBUG=1 -f makefile.ps2

GreaseMonkey_clean_for_ps2:
	${MAKE} -f makefile.ps2 clean

# NGC

GreaseMonkey_for_ngc: GreaseMonkey_release_for_ngc ;

GreaseMonkey_release_for_ngc:
	${MAKE} -f makefile.ngc

GreaseMonkey_debug_for_ngc:
	${MAKE} DEBUG=1 -f makefile.ngc

GreaseMonkey_clean_for_ngc:
	${MAKE} -f makefile.ngc clean

# Fail to do useful work for other platforms

GreaseMonkey_for_%:
	@echo "GreaseMonkey is not available for platform $(PLATFORM)"

GreaseMonkey_release_for_%:
	@echo "GreaseMonkey is not available for platform $(PLATFORM)"

GreaseMonkey_debug_for_%:
	@echo "GreaseMonkey is not available for platform $(PLATFORM)"

endif
