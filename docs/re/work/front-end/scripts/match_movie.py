import sys
from pathlib import Path
import numpy as np
from PIL import Image
a = np.asarray(Image.open(sys.argv[1]).convert("RGB")).astype(int)
best = []
for p in sorted(Path(sys.argv[2]).glob("frame-*.png")):
    b = np.asarray(Image.open(p).convert("RGB")).astype(int)
    best.append((np.abs(a - b).mean(), p.name))
best.sort()
print(best[:5], "of", len(best))
