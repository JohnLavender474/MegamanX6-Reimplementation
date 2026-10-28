import sys
sys.path.insert(0, '.')
from sp import *
ram = load(sys.argv[1])
for i in range(83):
    print(i, hex(w(ram, 0x80071220 + 4*i)))
