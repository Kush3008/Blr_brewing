#!/usr/bin/env python3
from __future__ import annotations

import math
import struct
import wave
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GFX = ROOT / "assets" / "graphics"
AUDIO = ROOT / "assets" / "audio"


def ensure_dirs() -> None:
    GFX.mkdir(parents=True, exist_ok=True)
    AUDIO.mkdir(parents=True, exist_ok=True)


def blank(w: int, h: int, c=(0, 0, 0, 0)):
    return [[list(c) for _ in range(w)] for _ in range(h)]


def set_px(img, x, y, c):
    if 0 <= y < len(img) and 0 <= x < len(img[0]):
        img[y][x] = list(c)


def rect(img, x0, y0, x1, y1, c):
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            set_px(img, x, y, c)


def ellipse(img, x0, y0, x1, y1, c):
    cx = (x0 + x1) / 2
    cy = (y0 + y1) / 2
    rx = max(1, (x1 - x0) / 2)
    ry = max(1, (y1 - y0) / 2)
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            if ((x - cx) / rx) ** 2 + ((y - cy) / ry) ** 2 <= 1:
                set_px(img, x, y, c)


def write_png(path: Path, img) -> None:
    h = len(img)
    w = len(img[0])
    raw = bytearray()
    for row in img:
        raw.append(0)
        for r, g, b, a in row:
            raw.extend((r, g, b, a))

    def chunk(tag: bytes, data: bytes) -> bytes:
        return struct.pack('>I', len(data)) + tag + data + struct.pack('>I', zlib.crc32(tag + data) & 0xFFFFFFFF)

    png = bytearray(b'\x89PNG\r\n\x1a\n')
    png.extend(chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 6, 0, 0, 0)))
    png.extend(chunk(b'IDAT', zlib.compress(bytes(raw), 9)))
    png.extend(chunk(b'IEND', b''))
    path.write_bytes(png)


def tone(path: Path, hz: float, sec: float):
    sample_rate = 22050
    frames = int(sample_rate * sec)
    with wave.open(str(path), 'wb') as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(sample_rate)
        for i in range(frames):
            env = min(1.0, i / 1000)
            v = int(12000 * env * math.sin(2 * math.pi * hz * i / sample_rate))
            wav.writeframes(struct.pack('<h', v))


def gen_bg():
    sky = (145, 196, 255, 255)
    img = blank(256, 256, sky)
    rect(img, 0, 64, 255, 190, (86, 92, 104, 255))
    rect(img, 0, 52, 255, 63, (150, 150, 140, 255))
    rect(img, 0, 191, 255, 202, (150, 150, 140, 255))
    for x in range(0, 256, 32):
        rect(img, x + 4, 12, x + 28, 50, (208, 176, 112, 255))
        rect(img, x + 10, 26, x + 22, 50, (110, 90, 75, 255))
    for x in range(8, 250, 32):
        rect(img, x, 125, x + 14, 129, (240, 230, 185, 255))
    write_png(GFX / 'bangalore_street.png', img)


def gen_character(name, body):
    img = blank(32, 16)
    for f in range(2):
        ox = f * 16
        rect(img, ox + 5, 3, ox + 10, 14, body)
        rect(img, ox + 5, 1, ox + 10, 4, (245, 210, 180, 255))
    write_png(GFX / f'{name}.png', img)


def gen_item(name, color):
    img = blank(32, 16)
    for f in range(2):
        ox = f * 16
        shift = 0 if f == 0 else -1
        ellipse(img, ox + 3, 6 + shift, ox + 12, 13 + shift, color)
    write_png(GFX / f'{name}.png', img)


def gen_icon(name, color):
    img = blank(8, 8)
    rect(img, 1, 1, 6, 6, color)
    write_png(GFX / f'icon_{name}.png', img)


def gen_misc():
    m = blank(16, 16)
    for i in range(16):
        set_px(m, 8, i, (248, 204, 96, 255))
        set_px(m, i, 8, (248, 204, 96, 255))
    write_png(GFX / 'meeting_marker.png', m)

    for name, col in [('red', (225, 66, 88, 255)), ('blue', (128, 220, 245, 255)), ('purple', (167, 100, 226, 255))]:
        h = blank(16, 16)
        ellipse(h, 3, 2, 8, 7, col)
        ellipse(h, 7, 2, 12, 7, col)
        for y in range(6, 15):
            w = 7 - (y - 6)
            for x in range(8 - w, 8 + w + 1):
                set_px(h, x, y, col)
        write_png(GFX / f'heart_{name}.png', h)

    g = blank(32, 16)
    rect(g, 2, 6, 10, 14, (167, 100, 226, 255))
    rect(g, 13, 2, 18, 8, (225, 66, 88, 255))
    rect(g, 22, 3, 29, 11, (248, 204, 96, 255))
    write_png(GFX / 'gift_icons.png', g)


def gen_audio():
    tone(AUDIO / 'confirm.wav', 880, 0.12)
    tone(AUDIO / 'cancel.wav', 240, 0.12)
    tone(AUDIO / 'collect.wav', 660, 0.10)
    tone(AUDIO / 'heart_pop.wav', 520, 0.15)


def main():
    ensure_dirs()
    gen_bg()
    gen_character('kush', (78, 146, 240, 255))
    gen_character('manna', (238, 120, 156, 255))
    gen_item('momos', (225, 66, 88, 255))
    gen_item('tea', (189, 123, 52, 255))
    gen_item('manchow', (204, 142, 78, 255))
    gen_item('roses', (215, 50, 85, 255))
    gen_item('sunflowers', (248, 204, 96, 255))
    gen_icon('momos', (225, 66, 88, 255))
    gen_icon('tea', (189, 123, 52, 255))
    gen_icon('manchow', (204, 142, 78, 255))
    gen_icon('roses', (215, 50, 85, 255))
    gen_icon('sunflowers', (248, 204, 96, 255))
    gen_misc()
    gen_audio()
    print('generated assets')


if __name__ == '__main__':
    main()
