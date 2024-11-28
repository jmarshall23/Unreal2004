
ifeq '$(PLATFORM)' 'ps2'
  define spawn-submakes
    : make -CGreaseMonkey $@
  endef
else
  define spawn-submakes
    make -CGreaseMonkey $@
  endef
endif

default: release ;

release:
	$(spawn-submakes)

debug:
	$(spawn-submakes)
