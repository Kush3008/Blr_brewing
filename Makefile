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


ifndef BUTANO_PATH
$(error BUTANO_PATH is not set. Point it to the Butano repo root containing tools/makefile)
endif

ifeq ($(wildcard $(BUTANO_PATH)/tools/makefile),)
$(error BUTANO_PATH is invalid: $(BUTANO_PATH). Expected $(BUTANO_PATH)/tools/makefile)
endif

include $(BUTANO_PATH)/tools/makefile
