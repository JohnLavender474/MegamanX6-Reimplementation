import sys
sys.path.insert(0, '.')
from sp import *
ram = load(sys.argv[1])
off = int(sys.argv[2], 0)
lo = int(sys.argv[3], 16); hi = int(sys.argv[4], 16)
for a in range(lo, hi, 4):
    word = w(ram, a)
    if (word >> 26) == 0x28 and (word & 0xFFFF) == off:
        print(dis(ram, a - 12, 5)); print('--')
