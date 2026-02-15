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

DEFAULT_BUTANO_PATH := $(firstword $(wildcard $(CURDIR)/.butano $(HOME)/butano /opt/butano))
BUTANO_PATH ?= $(DEFAULT_BUTANO_PATH)

.PHONY: setup-butano
setup-butano:
	@if [ -z "$(BUTANO_PATH)" ]; then \
		echo "Set BUTANO_PATH before running setup-butano (for example: BUTANO_PATH=$(CURDIR)/.butano)"; \
		exit 1; \
	fi
	@if [ -d "$(BUTANO_PATH)/.git" ]; then \
		echo "Butano already exists at $(BUTANO_PATH); refreshing"; \
		git -C "$(BUTANO_PATH)" fetch --depth 1 origin; \
		git -C "$(BUTANO_PATH)" reset --hard FETCH_HEAD; \
	else \
		git clone --depth 1 https://github.com/GValiente/butano.git "$(BUTANO_PATH)"; \
	fi

ifneq ($(filter assets setup-butano,$(MAKECMDGOALS)),)
# Skip Butano include for utility targets.
else
ifeq ($(strip $(BUTANO_PATH)),)
$(error BUTANO_PATH is not set. Set it explicitly or run `make setup-butano BUTANO_PATH=/path/to/butano`)
endif

ifeq ($(wildcard $(BUTANO_PATH)/tools/makefile),)
$(error BUTANO_PATH is invalid: $(BUTANO_PATH). Expected $(BUTANO_PATH)/tools/makefile)
endif

include $(BUTANO_PATH)/tools/makefile
endif
