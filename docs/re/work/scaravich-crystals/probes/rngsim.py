import sys
from ramdis import ram, u16, u32

def step(s):
    v1 = ((3 * s) & 0xFFFF) >> 8
    lo = (s + v1) & 0xFF
    return lo | (v1 << 8), lo

def period(s0):
    s, n = s0, 0
    while True:
        s, _ = step(s); n += 1
        if s == s0 or n > 70000: return n

if __name__ == '__main__':
    st = {}
    for name in sys.argv[1:]:
        r = ram(name)
        st[name] = (u16(r, 0x80090E70), u32(r, 0x80097420))
        print(name, f'rng {st[name][0]:#06x} frame_count {st[name][1]}')
    a, b = st[sys.argv[1]][0], st[sys.argv[2]][0]
    s, n = a, 0
    while s != b and n < 70000:
        s, _ = step(s); n += 1
    print('calls from first to second:', n if s == b else 'not reached')
    print('period from first:', period(a), ' from boot seed 3383:', period(3383))
    s, n = 3383, 0
    while s != a and n < 70000:
        s, _ = step(s); n += 1
    print('calls from boot seed to first:', n if s == a else 'not on the boot cycle')
