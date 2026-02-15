# Bangalore Love Quest (Butano + devkitARM)

A complete GBA homebrew collectathon built with **Butano**.

> Note: binary PNG/WAV outputs are intentionally not committed. They are generated locally by `tools/gen_assets.py` to avoid PR systems that reject binary files.

## Features

- Title -> Character Select -> Game -> Ending -> Credits -> Restart loop
- Choose **Kush** or **Manna**
- Top-down Bangalore-street map with movement boundaries
- Collect: momos, tea, manchow soup, roses, sunflowers
- UI overlay with tiny item icons and progress counter
- Meeting spot unlock after all collectibles
- Ending scene with opposite character and heart sequence:
  - red -> light blue -> purple
  - pop animation + SFX per heart
- Gift moment + dialog, then credits
- Programmatic asset pipeline (`tools/gen_assets.py`) for all PNG+WAV content

---

## Prerequisites

- Python 3.8+
- `git`, `make`
- devkitPro + devkitARM
- Butano checkout

## Install devkitPro / devkitARM

### Linux (official pacman wrapper)

```bash
sudo dkp-pacman -Syu
sudo dkp-pacman -S gba-dev
```

### Windows

Install devkitPro via the official installer, then install `gba-dev` from MSYS2 shell:

```bash
dkp-pacman -Syu
dkp-pacman -S gba-dev
```

---

## Build

1. Clone Butano somewhere on disk (or use an existing checkout):

```bash
git clone https://github.com/GValiente/butano.git ~/butano
```

_Optional helper target:_

```bash
make setup-butano BUTANO_PATH=~/butano
```

2. Export the path:

```bash
export BUTANO_PATH=~/butano
```

`BUTANO_PATH` must point to the **Butano repository root** (the folder that contains `tools/makefile`).

(Windows MSYS2 example)

```bash
export BUTANO_PATH=/c/path/to/butano
```

3. Generate game assets:

```bash
python3 tools/gen_assets.py
```

4. Build ROM:

```bash
make
```

(If assets are missing, run `python3 tools/gen_assets.py` first.)

Output ROM:

```text
build/bangalore_love_quest.gba
```

---

## CI artifact (GitHub Actions)

The workflow `.github/workflows/build-gba.yml` builds the ROM in a devkitARM container and uploads a downloadable artifact named **`bangalore-love-quest-gba`**.

If you trigger it manually (`Actions -> Build GBA ROM -> Run workflow`), download `bangalore_love_quest.gba` from the workflow run artifacts.

---

## Smoke Test Checklist

1. Boot ROM in Pizza Boy GBA.
2. Title appears with `Press START`.
3. Character select lets you switch between Kush/Manna and confirm.
4. In game scene:
   - D-pad movement works and remains in boundaries.
   - Collect all 5 item types.
   - UI count increments and icons remain visible.
5. After collecting all, marker appears and prompts movement to meeting spot.
6. Ending shows opposite character.
7. Hearts pop in exact order: red -> light blue -> purple with SFX.
8. Gift/dialog moment appears.
9. Credits appear; START restarts to title.

---

## Notes / Fallback options

If local setup is difficult, you can build using WSL (Windows) with devkitPro packages installed there.

