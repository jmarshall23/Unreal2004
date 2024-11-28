# Assuming GNU make here!

#------------------------------------------------------------------------------
#Platform independent Makefile
#------------------------------------------------------------------------------

OUT_DIR=../release
SRC_DIR=../src

IMAGES		:= images
IMAGES2		:= Welcome_pics
COLLISION_REF	:= collision_ref
DYNAMICS_REF	:= dynamics_ref
SIMULATION_REF	:= simulation_ref


default:
	@echo "use: make <all|clean>"


all: $(OUT_DIR)
	doxygen McdDoxyfile
	doxygen MdtDoxyfile
	doxygen MstDoxyfile
	-cp -p -f $(SRC_DIR)/* $(OUT_DIR)
	-cp -p -f -R $(SRC_DIR)/$(IMAGES)  $(OUT_DIR) 
	-cp -p -f -R $(SRC_DIR)/$(IMAGES2) $(OUT_DIR) 
	-rm -r -f $(OUT_DIR)/$(IMAGES)/CVS
	-rm -r -f $(OUT_DIR)/$(IMAGES2)/CVS
	doxygen AssetDoxyfile
	doxygen UtilitiesDoxyfile

release: all
	@echo "finished release build!"

debug: all
	@echo "finished debug build!"


clobber: clean
	@echo "clobbered!"

clean:
	-rm -fr $(OUT_DIR)
	@echo "cleaned!"

cleanish:
	@echo "intermediate files removed!"


depend:
	mkdir -p $(OUT_DIR)

$(OUT_DIR):
	mkdir -p $@
