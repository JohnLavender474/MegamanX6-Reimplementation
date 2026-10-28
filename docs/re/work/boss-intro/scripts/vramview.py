import numpy as np
from PIL import Image
from common import *
SP = '/tmp/claude-1000/-home-johnlavender-mmx-data/2ad6db8a-32e9-4897-bc53-bda0c68a0213/scratchpad/'
def rgb16(v):
    r = (v & 31) << 3; g = ((v >> 5) & 31) << 3; b = ((v >> 10) & 31) << 3
    return np.stack([r, g, b], -1).astype(np.uint8)
if __name__ == '__main__':
    ram, vram = load()
    v = np.frombuffer(vram, '<u2').reshape(512, 1024)
    Image.fromarray(rgb16(v)).save(SP + 'vram16.png')
    b = np.frombuffer(vram, np.uint8).reshape(512, 2048)
    Image.fromarray(b).save(SP + 'vram8grey.png')
