import sys
from pathlib import Path
from PIL import Image
d = Path(__file__).parent / (sys.argv[1] if len(sys.argv) > 1 else 'ora') / 'data'
L = [Image.open(d / f'layer{i}.png') for i in (2, 3, 4)]
views = {'14': [(4495, 256), (2247, 448), (561, 448)], '15': [(4480, 496), (2240, 568), (560, 448)], '13': [(2961, 256), (1480, 128), (740, 64)], '1': [(0, 256), (0, 128), (0, 64)]}
for name, cams in views.items():
    row = []
    for li, (x, y) in enumerate(cams):
        row.append(L[li].crop((x, y, x + 320, y + 240)))
    comp = Image.new('RGBA', (320, 240), (0, 0, 0, 255))
    for im in reversed(row):
        comp.alpha_composite(im)
    out = Image.new('RGBA', (320 * 4 + 12, 240), (255, 0, 255, 255))
    for i, im in enumerate(row + [comp]):
        out.paste(im, (i * 324, 0))
    out.save(Path(__file__).parent / f'view-{name}-{sys.argv[1] if len(sys.argv) > 1 else "ora"}.png')
