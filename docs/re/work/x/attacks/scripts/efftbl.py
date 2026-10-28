from probe import *
for s in STATES:
    r=load(s)
    st=s8(r,0x800CCEDC); ar=s8(r,0x800CCEDD)
    p=u32(r,0x8006dddc+st*8+ar*4)
    print(s, "stage",st,"area",ar,"effect table",hex(p), [hex(u32(r,p+4*i)) for i in range(6)])
    for k in range(96):
        o=0x80091298+k*0x70
        if u8(r,o): print("  eff",k,hex(o),bytes_at(r,o,0x70).hex(" "))
    for k in range(32):
        o=0x800950A0+k*0xA0
        if u8(r,o): print("  wpn",k,hex(o),bytes_at(r,o,0xA0).hex(" "))
