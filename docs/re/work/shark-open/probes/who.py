import sys
sys.path.insert(0, '.')
from sp import *
ram = load(sys.argv[1])
ids = [int(x) for x in sys.argv[2].split(',')]
print('ovl same at ED2A8?', ram[0x1ED2A8:0x1ED2C8].hex())
for a in range(0x80010000, 0x80200000, 4):
    word = w(ram, a)
    if (word >> 26) == 9 and (word & 0xFFFF) in ids and ((word >> 21) & 31) == 0:
        print(dis(ram, a - 8, 6)); print('--')
