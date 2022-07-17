ifeq ($(OS),Windows_NT)
	SHELL:=$(ComSpec)
	SCRIPT_EXT= cmd
	MKDIR = mkdir
	QHL_HVX_DIR := $(HEXAGON_SDK_ROOT)\libs\qhl_hvx
else
	SHELL=/bin/bash
	SCRIPT_EXT= bash
	MKDIR = mkdir -p
	QHL_HVX_DIR := $(HEXAGON_SDK_ROOT)/libs/qhl_hvx
endif

TOOLV83_PATH = $(HEXAGON_SDK_ROOT)/tools/HEXAGON_TOOLS/8.3.07#

SUPPORT_VS += hexagon_ReleaseG_toolv83_v66 \
              hexagon_ReleaseG_toolv84_v66 \
              hexagon_ReleaseG_toolv84_v68

SUPPORT_VS_CLEAN = $(foreach d,$(SUPPORT_VS),${d}_CLEAN)

.PHONY: tree tree_clean $(SUPPORT_VS) $(SUPPORT_VS_CLEAN)

tree: $(SUPPORT_VS)

tree_clean: $(SUPPORT_VS_CLEAN)

$(SUPPORT_VS):
	$(eval V := $(subst _, ,$(@)))
	$(eval HOST := $(word 1, $(V)))
	$(eval VARIANT := $(word 2, $(V)))
	$(eval TOOLS_VER := $(word 3, $(V)))
	$(eval HEXAGON_ARCH := $(word 4,$(V)))

ifeq ($(OS),Windows_NT)
	@if $(TOOLS_VER)==toolv83 (set HEXAGON_TOOLS_ROOT=$(TOOLV83_PATH) && cd $(QHL_HVX_DIR) && cmake_build.$(SCRIPT_EXT) $(HOST) DSP_ARCH=$(HEXAGON_ARCH)) \
         else (set HEXAGON_TOOLS_ROOT=$(DEFAULT_HEXAGON_TOOLS_ROOT) && cd $(QHL_HVX_DIR) && cmake_build.$(SCRIPT_EXT) $(HOST) DSP_ARCH=$(HEXAGON_ARCH))

	@if not exist $(QHL_HVX_DIR)\prebuilt (${MKDIR} $(QHL_HVX_DIR)\prebuilt)
	${MKDIR} $(QHL_HVX_DIR)\prebuilt\$(subst _ReleaseG,,$(@))
	@mv $(QHL_HVX_DIR)\$(@)\libqh* $(QHL_HVX_DIR)\prebuilt\$(subst _ReleaseG,,$(@))
else
	@if [ "$(TOOLS_VER)" = "toolv83" ]; then \
            cd $(QHL_HVX_DIR) && HEXAGON_TOOLS_ROOT=$(TOOLV83_PATH) ./cmake_build.$(SCRIPT_EXT) $(HOST) DSP_ARCH=$(HEXAGON_ARCH); \
            else cd $(QHL_HVX_DIR) && HEXAGON_TOOLS_ROOT=$(DEFAULT_HEXAGON_TOOLS_ROOT) ./cmake_build.$(SCRIPT_EXT) $(HOST) DSP_ARCH=$(HEXAGON_ARCH); \
        fi

	${MKDIR} $(QHL_HVX_DIR)/prebuilt
	${MKDIR} $(QHL_HVX_DIR)/prebuilt/$(subst _ReleaseG,,$(@))/
	@mv $(QHL_HVX_DIR)/$(@)/libqh* $(QHL_HVX_DIR)/prebuilt/$(subst _ReleaseG,,$(@))/
endif

$(SUPPORT_VS_CLEAN):
	$(eval V := $(subst _, ,$(patsubst %_CLEAN,%,$(@))))
	$(eval HOST := $(word 1, $(V)))
	$(eval VARIANT := $(word 2, $(V)))
	$(eval TOOLS_VER := $(word 3, $(V)))
	$(eval HEXAGON_ARCH := $(word 4,$(V)))

ifeq ($(OS),Windows_NT)
	@if $(TOOLS_VER)==toolv83 (set HEXAGON_TOOLS_ROOT=$(TOOLV83_PATH) &&  cd $(QHL_HVX_DIR) && cmake_build.$(SCRIPT_EXT) $(HOST)_clean DSP_ARCH=$(HEXAGON_ARCH)) \
         else (set HEXAGON_TOOLS_ROOT=$(DEFAULT_HEXAGON_TOOLS_ROOT) && cd $(QHL_HVX_DIR) && cmake_build.$(SCRIPT_EXT) $(HOST)_clean DSP_ARCH=$(HEXAGON_ARCH))
else
	@if [ "$(TOOLS_VER)" = "toolv83" ]; then \
             cd $(QHL_HVX_DIR) && HEXAGON_TOOLS_ROOT=$(TOOLV83_PATH) ./cmake_build.$(SCRIPT_EXT) $(HOST)_clean DSP_ARCH=$(HEXAGON_ARCH); \
            else  cd $(QHL_HVX_DIR) && HEXAGON_TOOLS_ROOT=$(DEFAULT_HEXAGON_TOOLS_ROOT) ./cmake_build.$(SCRIPT_EXT) $(HOST)_clean DSP_ARCH=$(HEXAGON_ARCH); \
        fi
endif
	@rm -rf $(QHL_HVX_DIR)/prebuilt
