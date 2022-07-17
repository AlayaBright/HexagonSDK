include $(HEXAGON_SDK_ROOT)/build/make.d/android_vs.min
include $(HEXAGON_SDK_ROOT)/build/make.d/UbuntuARM_vs.min
include android_deps.min
include UbuntuARM_deps.min


SUPPORT1_VS = $(foreach d, $(SUPPORTED_VS), $(subst Debug,Release, $d))
SUPPORT2_VS = $(foreach d, $(SUPPORT1_VS), $(subst ReleaseG,Release, $d))
SUPPORT3_VS =$(foreach d, $(SUPPORT2_VS), $(subst Release,ReleaseG, $d))
SUPPORT_VS += $(filter $(SUPPORT3_VS), $(SUPPORTED_VS))
SUPPORT_VS +=  \
      hexagon_ReleaseG_toolv83_v65 \
      hexagon_ReleaseG_toolv84_v65 \
      hexagon_ReleaseG_toolv82_v65 \
      hexagon_ReleaseG_toolv81_v65
SUPPORT_VS_CLEAN = $(foreach d,$(SUPPORT_VS),$(d)_CLEAN)

.PHONEY: tree tree_clean $(SUPPORT_VS) $(SUPPORT_VS_CLEAN)

tree: $(SUPPORT_VS)

tree_clean: $(SUPPORT_VS_CLEAN)

$(SUPPORT_VS):
	"mkdir" -p prebuilt
	$(MAKE) V=$(@) tree
	"mkdir" prebuilt/$(subst _ReleaseG,,$(@))/
	mv $(@)/ship/* prebuilt/$(subst _ReleaseG,,$(@))/

$(SUPPORT_VS_CLEAN):
	$(MAKE) V=$(patsubst %_CLEAN,%,$(@)) tree_clean
	rm -rf prebuilt
