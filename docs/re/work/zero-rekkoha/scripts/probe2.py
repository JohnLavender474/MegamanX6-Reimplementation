from zprobe import *
r=load("heatnix-3_rekkoha")
SP=0x1F800000
import struct
# scratchpad is in the state? try ram_from scratch
print("cam layer0 x,y", u16(r,0x800971F8+0xA), u16(r,0x800971F8+0xE))
print("frame_count", u32(r,0x80097420))
print("box 0x801ee3dc", struct.unpack_from("<hhhh", r, 0x801ee3dc&0x1fffff), bytes_at(r,0x801ee3dc,16).hex(" "))
tab=u32(r,0x8006DDDC+3*8+0*4); print("effect tbl", hex(tab))
for t in (3,4,17,26): print(" effect type",t,hex(u32(r,tab+4*t)))
# fade channel 1 live
print("fade_mode", bytes_at(r,0x800CF850,3).hex(), "fade_r",[u16(r,0x800CF854+2*i) for i in range(3)], "fade_g",[hex(u16(r,0x80097430+2*i)) for i in range(3)],"fade_b",[hex(u16(r,0x80090D5C+2*i)) for i in range(3)])
print("sub_g", bytes_at(r,0x8009742C,3).hex(), "sub_b", bytes_at(r,0x80090D56,3).hex())
print("mask1", bytes_at(r,0x80090C90+0x40,0x40).hex())
print("saved", bytes_at(r,0x801EF974,0x40).hex())
print("bg_refresh", u8(r,0x800C4560), "player_lock", u8(r,0x800CCEEC), "no_hit", u8(r,0x800970A0+0x7A),"D3",u8(r,0x800970A0+0xD3), "E0",u8(r,0x800970A0+0xE0))
