

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
