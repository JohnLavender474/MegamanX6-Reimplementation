from zprobe import *
r = load(STATES[0])
def jt(base, n, off, plus):
    return [i+off for i in range(n) if u32(r, base+4*i) == plus]
print("buster plus (0x80):", jt(0x800110C4, 59, 0, 0x80031768))
print("part 0x2000000:", jt(0x800111B4, 54, 1, 0x8003182c))
print("part 0x200000:", jt(0x8001128C, 54, 1, 0x80031880))
print("jt 0x8001102c (id-26, 37):", [(i+26, hex(u32(r,0x8001102c+4*i))) for i in range(37)])
