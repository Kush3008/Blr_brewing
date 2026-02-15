PROJECT := bangalore_love_quest
BUILD := build
SOURCES := src
INCLUDES := include
ASSETS := assets

GAME_TITLE := Bangalore Love Quest
GAME_CODE := BLQX
GAME_MAKER := 00

ASSET_STAMP := assets/.generated

$(ASSET_STAMP): tools/gen_assets.py
	python3 tools/gen_assets.py
	@mkdir -p assets
	@touch $(ASSET_STAMP)

assets: $(ASSET_STAMP)

ifdef BUTANO_MAKEFILE_PATH
BUTANO_MAKEFILE := $(BUTANO_MAKEFILE_PATH)
else
ifndef BUTANO_PATH
$(error BUTANO_PATH is not set. Point it to the Butano repo root)
endif

BUTANO_MAKEFILE :=
ifneq ($(wildcard $(BUTANO_PATH)/tools/makefile),)
BUTANO_MAKEFILE := $(BUTANO_PATH)/tools/makefile
endif
ifneq ($(wildcard $(BUTANO_PATH)/tools/Makefile),)
BUTANO_MAKEFILE := $(BUTANO_PATH)/tools/Makefile
endif
ifneq ($(wildcard $(BUTANO_PATH)/butano/tools/makefile),)
BUTANO_MAKEFILE := $(BUTANO_PATH)/butano/tools/makefile
endif
ifneq ($(wildcard $(BUTANO_PATH)/butano/tools/Makefile),)
BUTANO_MAKEFILE := $(BUTANO_PATH)/butano/tools/Makefile
endif
endif

ifeq ($(BUTANO_MAKEFILE),)
$(error Butano makefile not found. Set BUTANO_MAKEFILE_PATH directly or set BUTANO_PATH to a repo containing tools/makefile)
endif

include $(BUTANO_MAKEFILE)
