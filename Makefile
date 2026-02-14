# Project name (also becomes the output ROM name)
TARGET := bangalore_love_quest

# Build dir
BUILD := build

# Source/include folders
SOURCES := src
INCLUDES := include

# Asset folders (your tools/gen_assets.py writes here)
GRAPHICS := assets/graphics
AUDIO := assets/audio
DMGAUDIO :=
DATA :=

# GBA header metadata (limits: ROMTITLE max 12 chars, ROMCODE 4 chars)
ROMTITLE := BLRLOVEQUEST
ROMCODE := BLQ1

# Butano path:
# In GitHub Actions you set BUTANO_PATH to the cloned butano repo root.
# The actual engine folder is: $(BUTANO_PATH)/butano
LIBBUTANO := $(BUTANO_PATH)/butano
LIBBUTANOABS := $(realpath $(LIBBUTANO))

# Ensure python command exists in your container
PYTHON := python3

# Include Butano build system
include $(LIBBUTANOABS)/butano.mak
