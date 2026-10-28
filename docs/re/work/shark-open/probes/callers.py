import sys
sys.path.insert(0, '.')
from sp import *
ram = load(sys.argv[1])
for t in sys.argv[2:]:
    t = int(t, 16)
    for a in calls_to(ram, t, 0x80010000, 0x80200000):
        print(f'call {t:08x} from {a:08x}')
        print(dis(ram, a - 12, 5))
