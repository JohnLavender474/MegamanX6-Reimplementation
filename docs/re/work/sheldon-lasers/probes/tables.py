import sys, struct, os
sys.path.insert(0, os.path.expanduser('~/IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import read_ram
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'),'rb').read()
st = sys.argv[1] if len(sys.argv) > 1 else 'sheldon-7_lasers.state.gz'
r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/'+st), EXE)
def u8(a): return r[a&0x1fffff]
def s8(a): return struct.unpack_from('<b',r,a&0x1fffff)[0]
def s16(a): return struct.unpack_from('<h',r,a&0x1fffff)[0]
def u16(a): return struct.unpack_from('<H',r,a&0x1fffff)[0]
def u32(a): return struct.unpack_from('<I',r,a&0x1fffff)[0]
def box(a): return (s8(a),s8(a+1),u8(a+2),u8(a+3))
print('laser attack box 0x800fbf9c', box(0x800fbf9c))
print('beamer 0x2_ hurt box', box(0x800fb964), 'gate hurt boxes', box(0x800fb9a8), box(0x800fb9ac))
for i in range(16): print(' reflector dir', i, 'hurt box', box(0x800fb968+4*i))
print('reflection table [reflector dir][incoming dir] -> (out dir, dx, dy) or stop')
for rd in range(16):
    row=[]
    for d in range(8):
        idx=u8(0x800fbcc0+rd*8+d)
        if idx==255: row.append('  stop      ')
        else:
            e=0x800fbd40+idx*6
            row.append(f'{u8(e):3d}({s16(e+2):+d},{s16(e+4):+d})')
    print(f' r{rd:2d}: '+' '.join(f'{x:13s}' for x in row))
print('rotating reflector set-up 0x800fbae0 (n: seq, start index)')
for n in range(16): print(' ', n, u8(0x800fbae0+n*8), u8(0x800fbae0+n*8+4))
print('sequences 0x800fbb60')
for s in range(12): print(' ', s, [u32(0x800fbb60+s*32+4*i) for i in range(8)])
print('gate spawn rects 0x800fbef8 (x0,y0,x1,y1)')
for n in range(5): print(' ', n, [s16(0x800fbef8+n*8+2*i) for i in range(4)])
print('gate delete rects 0x800fbec8.. (x0,y0,x1,y1)')
for a in range(0x800fbec8,0x800fbef8,8): print(f'  {a:#x}', [s16(a+2*i) for i in range(4)])
print('copy srcs 0x800fbe90..'); 
for a in range(0x800fbe90,0x800fbec8,8): print(f'  {a:#x}', [s16(a+2*i) for i in range(4)])
print('sprite tables head 0x800fbca0, start 0x800fbcb0, 0x800fbc80, 0x800fbc90 per dir (sprite, flip)')
for t in (0x800fbca0,0x800fbcb0,0x800fbc80,0x800fbc90): print(f'  {t:#x}', [(u8(t+2*d),hex(u8(t+2*d+1))) for d in range(8)])
print('shot id counter 0x800fcfec', u16(0x800fcfec))
print('weapon type table entry 79', hex(u32(0x80071220+79*4)), 'entry 78', hex(u32(0x80071220+78*4)))
for i,(base,sz) in enumerate([(0x8008EF48,0x9C)]*5):
    a=base+i*sz
    print(f'main[{i}] var {u8(a+2):#x} st{u8(a+4)} step{u8(a+5)} s2 {u8(a+6)} xy {s16(a+10)},{s16(a+14)} 7C {u16(a+0x7c)} 7E {u16(a+0x7e)} 84 {u16(a+0x84)} 86 {u8(a+0x86)} 87 {u8(a+0x87)} 88 {u8(a+0x88)} 89 {u8(a+0x89)} 8A {u8(a+0x8a)} 8B {u8(a+0x8b)} 8C {u8(a+0x8c)} hurt {u32(a+0x54):#x} dmg {u32(a+0x58):#x} anim {u8(a+0x47)}')
for i in range(4):
    a=0x80093C98+i*0xA0
    print(f'visual[{i}] st{u8(a+4)} xy {s16(a+10)},{s16(a+14)} spd {struct.unpack_from("<i",r,(a+0x20)&0x1fffff)[0]/65536},{struct.unpack_from("<i",r,(a+0x24)&0x1fffff)[0]/65536} parent {u32(a+0x7c):#x} dir {u32(a+0x84)} 88 {u16(a+0x88)} 8A {u16(a+0x8a)} wpn {u32(a+0x8c):#x} id {u16(a+0x90)} start {s16(a+0x92)},{s16(a+0x94)} 96 {u16(a+0x96)} 98 {u8(a+0x98)} cd {u8(a+0x60)} 62 {u8(a+0x62)} atk {u32(a+0x50):#x}')
for i in range(4):
    a=0x800950A0+i*0xA0
    print(f'weapon[{i}] st{u8(a+4)} step{u8(a+5)} xy {s16(a+10)},{s16(a+14)} parent {u32(a+0x7c):#x} dir {u32(a+0x84)} 8C {u16(a+0x8c)} 98 {u8(a+0x98)} atk {u32(a+0x50):#x} box {box(u32(a+0x50)) if u32(a+0x50) else None} 60 {u8(a+0x60)} raw50-70 {r[(a+0x50)&0x1fffff:(a+0x70)&0x1fffff].hex()}')
