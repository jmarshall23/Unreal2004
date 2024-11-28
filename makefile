#=============================================================================
# Unreal makefile for all platforms other than Win32/Visual C++.
#=============================================================================

# Unreal directory. (Required by makefile-header.)
UNREAL_DIR			= .
MAKE				= make

# Include global definitions.
include				$(UNREAL_DIR)/makefile-header

# No default sub-make arguments.
ARGS				=

#-----------------------------------------------------------------------------
# Rules.
#-----------------------------------------------------------------------------

.PHONY: all
all: zlib ogg vorbis core engine fire xgame ipdrv uweb xinterface onslaught alaudio speex #mp3 #utv #editor
ifeq ($(TARGETTYPE),psx2)
all: psx2drv psx2render psx2audio psx2launch #psx2sceirxs #psx2netdrv
endif

ifeq ($(DOPIXO),true)
all: pixodrv
endif

ifeq ($(ISGNU),true)
all: ucc opengldrv sdldrv sdllaunch
endif

ifeq ($(ISWINDOWS),true)
all: ucc window opengldrv d3d9drv windrv launch
endif

run: all
	cmd /c run.bat

.PHONY: core
core: 
	@$(MAKE) $(ARGS) --directory=$(CORE_SRC)

.PHONY: engine
engine: core
	@$(MAKE) $(ARGS) --directory=$(ENGINE_SRC)

.PHONY: ipdrv
ipdrv: core engine
	@$(MAKE) $(ARGS) --directory=$(IPDRV_SRC)

.PHONY: fire
fire: core engine
	@$(MAKE) $(ARGS) --directory=$(FIRE_SRC)

.PHONY: mp3
mp3:
	@$(MAKE) $(ARGS) --directory=$(MP3_SRC)

.PHONY: alaudio
alaudio: core engine
	@$(MAKE) $(ARGS) --directory=$(ALAUDIO_SRC)

.PHONY: editor
editor: core engine
	@$(MAKE) $(ARGS) --directory=$(EDITOR_SRC)

.PHONY: speex
speex: core engine
	@$(MAKE) $(ARGS) --directory=$(SPEEX_SRC)

.PHONY: utv
utv: core engine
	@$(MAKE) $(ARGS) --directory=$(UTV_SRC)

.PHONY: ucc
ucc: zlib ogg vorbis core engine fire ipdrv uweb xgame xinterface onslaught #utv #psx2ilinkdrv
	@$(MAKE) $(ARGS) --directory=$(UCC_SRC)

.PHONY: xlaunch
xlaunch: core engine
	@$(MAKE) $(ARGS) --directory=$(XLAUNCH_SRC)

.PHONY: psx2launch
psx2launch: core engine
	@$(MAKE) $(ARGS) --directory=$(PSX2LAUNCH_SRC)

.PHONY: sdllaunch
sdllaunch: core engine
	@$(MAKE) $(ARGS) --directory=$(SDLLAUNCH_SRC)

.PHONY: launch
launch: core engine window
	@$(MAKE) $(ARGS) --directory=$(LAUNCH_SRC)

.PHONY: opengldrv
opengldrv: core engine
	@$(MAKE) $(ARGS) --directory=$(OPENGLDRV_SRC)

.PHONY: window
window: core engine
	@$(MAKE) $(ARGS) --directory=$(WINDOW_SRC)

.PHONY: windrv
windrv: core engine window
	@$(MAKE) $(ARGS) --directory=$(WINDRV_SRC)

.PHONY: d3d9drv
d3d9drv: core engine window
	@$(MAKE) $(ARGS) --directory=$(D3D9DRV_SRC)

.PHONY: pixodrv
pixodrv:
	@$(MAKE) $(ARGS) --directory=$(PIXODRV_SRC)

.PHONY: xdrv
xdrv: core engine
	@$(MAKE) $(ARGS) --directory=$(XDRV_SRC)

.PHONY: psx2drv
psx2drv: core engine
	@$(MAKE) $(ARGS) --directory=$(PSX2DRV_SRC)
	@$(MAKE) $(ARGS) --directory=$(PSX2DRV_IOPMODULE)

.PHONY: xmesagldrv
xmesagldrv: core engine render
	@$(MAKE) $(ARGS) --directory=$(XMESAGLDRV_SRC)

.PHONY: audio
audio:
	@$(MAKE) $(ARGS) --directory=$(AUDIO_SRC)

.PHONY: uweb
uweb:
	@$(MAKE) $(ARGS) --directory=$(UWEB_SRC)

.PHONY: psx2render
psx2render:
	@$(MAKE) $(ARGS) --directory=$(PSX2RENDER_SRC)

.PHONY: psx2netdrv
psx2netdrv:
	@$(MAKE) $(ARGS) --directory=$(PSX2NETDRV_SRC)

.PHONY: psx2audio
psx2audio:
	@$(MAKE) $(ARGS) --directory=$(PSX2AUDIO_SRC)
	@$(MAKE) $(ARGS) --directory=$(PSX2AUDIO_IOPMODULE)

.PHONY: psx2ilinkdrv
psx2ilinkdrv:
	@$(MAKE) $(ARGS) --directory=$(PSX2ILINKDRV_SRC)
	@$(MAKE) $(ARGS) --directory=$(PSX2ILINKDRV_IOPMODULE)

.PHONY: xgame
xgame: core engine
	@$(MAKE) $(ARGS) --directory=$(XGAME_SRC)

.PHONY: xinterface
xinterface: core engine ipdrv
	@$(MAKE) $(ARGS) --directory=$(XINTERFACE_SRC)

.PHONY: onslaught
onslaught: core engine
	@$(MAKE) $(ARGS) --directory=$(ONSLAUGHT_SRC)

.PHONY: zlib
zlib:
	@$(MAKE) $(ARGS) --directory=$(ZLIB_SRC)

.PHONY: ogg
ogg:
	@$(MAKE) $(ARGS) --directory=$(OGG_SRC)

.PHONY: vorbis
vorbis:
	@$(MAKE) $(ARGS) --directory=$(VORBIS_SRC)

.PHONY: sdldrv
sdldrv:
	@$(MAKE) $(ARGS) --directory=$(SDLDRV_SRC)

.PHONY: psx2sceirxs
psx2sceirxs: System/ilink.irx System/ilsock.irx System/libsd.irx System/sdrdrv.irx System/sio2man.irx System/padman.irx System/mtapman.irx System/usbd.irx System/psx2cda.irx System/mcman.irx System/mcserv.irx System/ioprp16.img System/system.cnf
System/%.irx: $(PSX2ROOT)/iop/modules/%.irx
	@cp $< $@
System/%.img: $(PSX2ROOT)/iop/modules/%.img
	@cp $< $@
System/%.cnf: PSX2Launch/Src/%.cnf
	@cp $< $@

#-----------------------------------------------------------------------------
# Maintenance.
#-----------------------------------------------------------------------------

# Pass custom targets to module makefiles.
.DEFAULT: 
	@$(MAKE) ARGS=$@
.PHONY: clean

#-----------------------------------------------------------------------------
# The End.
#-----------------------------------------------------------------------------
