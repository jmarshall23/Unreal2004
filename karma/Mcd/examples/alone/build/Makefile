# Assuming GNU make here!

#----------------------------------------------------------------------------
#Platform independent Makefile
#------------------------------------------------------------------------------

#The order of these includes may seem strange but don't change it; 'Make' is sometimes strange!
include ./Makefile.sources
include ../../build/Makefile.paths
include ../../build/Makefile.flags


#set up the source search path, remembering to add the environment source path
VPATH+=:$(VPATH_SEARCH_DIR):$(PLATFORM)/$(MAKECMDGOALS):$(SOURCES_PATH)

OBJECTS_REL:=$(addprefix $(PLATFORM)/$(MAKECMDGOALS)/,$(OBJECTS))

ifeq '$(PLATFORM)' 'ps2'
#include the 'c' runtime
	OBJECTS_REL+=crt0.o
	OBJECTS+=crt0.o
endif

#examples may link to the collision library, order may be important!
LIB_COMMON:=$(LIB_MCD) $(LIB_MCD_BRIDGE) $(LIB_COMMON)
LIB_COMMON_DEBUG:=$(LIB_MCD_DEBUG) $(LIB_MCD_BRIDGE_DEBUG) $(LIB_COMMON_DEBUG)



default: $(MAKECMDGOALS)
	@echo "usage: make PLATFORM=x <release|debug|clean|cleanish>, where x is irix,linux,ps2,win32"


release: check depend $(OUTPUT_FILE_RELEASE) common
	mv $(PLATFORM)/release/$(OUTPUT_FILE_RELEASE) $(OUT_DIR)/$(PLATFORM)
	@echo "finished release build!"

$(OUTPUT_FILE_RELEASE): $(OBJECTS)
	$(CC) -o $(PLATFORM)/$(MAKECMDGOALS)/$(@) $(OBJECTS_REL) $(CCLINK_FLAGS) $(LIB_DIR) $(LIB_COMMON)


debug: check depend $(OUTPUT_FILE_DEBUG) common
	mv $(PLATFORM)/debug/$(OUTPUT_FILE_DEBUG) $(OUT_DIR)/$(PLATFORM)
	@echo "finished debug build!"

$(OUTPUT_FILE_DEBUG): $(OBJECTS)
	$(CC) -o $(PLATFORM)/$(MAKECMDGOALS)/$(@) $(OBJECTS_REL) $(CCLINK_FLAGS) $(LIB_DIR) $(LIB_COMMON_DEBUG)


common:
	-cp $(SRC_DIR)/*.bmp $(OUT_DIR)/$(PLATFORM)
	-cp $(SRC_DIR)/*.tif $(OUT_DIR)/$(PLATFORM)


#Need this because $(LCFILE) can't deal with the relative path
crt0.o: crt0.s
	$(AS) $(AS_FLAGS) $(INC_DIR) -o $@ $< > $*.lst

%.o : %.s
	$(AS) $(AS_FLAGS) $(INC_DIR) -o $(PLATFORM)/$(MAKECMDGOALS)/$@ $< > $(PLATFORM)/$(MAKECMDGOALS)/$*.lst

%.o : %.dsm
	$(DVPASM) $(DVPASM_FLAGS_PS2) $(INC_DIR) -o $(PLATFORM)/$(MAKECMDGOALS)/$@ $< > $(PLATFORM)/$(MAKECMDGOALS)/$*.lst

%.o : %.c
	$(CC) $(CC_FLAGS) $(COMPILER_PREFIX)$(PLATFORM)/$(MAKECMDGOALS)/$(@) $(INC_DIR) > $(PLATFORM)/$(MAKECMDGOALS)/$*.lst -c $<

%.o : %.cpp
	$(CC) $(CC_FLAGS) $(COMPILER_PREFIX)$(PLATFORM)/$(MAKECMDGOALS)/$(@) $(INC_DIR) > $(PLATFORM)/$(MAKECMDGOALS)/$*.lst -c $<


clobber: clean
	-rm -f -r $(OUT_DIR)
	@echo "clobbered!"


clean: cleanish
	-rm -f -r $(OUT_DIR)/$(PLATFORM)
	@echo "cleaned!"

cleanish: check
	-rm -f -r $(PLATFORM)
	-rm -f *.pdb
	-rm -f *.idb
	-rm -f *.map
	-rm -f crt0.lst
	-rm -f crt0.o
	@echo "intermediate files removed!"


depend: check
	mkdir -p $(PLATFORM)/$(MAKECMDGOALS)
	mkdir -p $(OUT_DIR)/$(PLATFORM)

ifneq '$(PLATFORM)' 'win32'
	$(DEPEND) $(DEPEND_FLAGS) $(INC_DIR) $(SOURCES_REL) > $(PLATFORM)/Dependencies
endif


check:
	if [ x$(PLATFORM) = x -o x$(PLATFORM) != xirix -a x$(PLATFORM) != xlinux -a x$(PLATFORM) != xps2 -a x$(PLATFORM) != xwin32 ] ; \
	then \
		echo "usage: make PLATFORM=x <release|debug|clean|cleanish>, where x is irix,linux,ps2,win32" ; \
		false ; \
	fi


ifneq '$(PLATFORM)' 'win32'
-include $(PLATFORM)/Dependencies
endif
