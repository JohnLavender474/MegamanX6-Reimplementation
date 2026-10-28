import sys
sys.path.insert(0, __file__.rsplit("/",1)[0])
from objprobe import ram, s16, u8, u32, at
for name in sys.argv[1:]:
    r = ram(name)
    print(name)
    for k, base in enumerate((0x800971F8, 0x8009724C, 0x800972A0)):
        print(f' layer{k} x {s16(r,base+0xA)} y {s16(r,base+0xE)} lim {[s16(r,base+0x1C+2*i) for i in range(4)]} tgt {[s16(r,base+0x24+2*i) for i in range(4)]} mode {u8(r,base+4)} +40 {s16(r,base+0x40)} +42 {s16(r,base+0x42)} y16 {u32(r,base+0xC):#x}')
    mg = 0x800970A0
    print(' mega', s16(r,mg+0xA), s16(r,mg+0xE), 'step', u8(r,mg+5), 'E0', u8(r,mg+0xE0), 'E5', u8(r,mg+0xE5), 'E6', u8(r,mg+0xE6), '89', hex(u8(r,mg+0x89)), 'hp', u8(r,mg+0x5C))
    g = 0x800CCED0
    print(' stage', u8(r,g+0xC), u8(r,g+0xD), 'cp', u8(r,g+0x1D), 'g24', u8(r,g+0x24), 'g26', u8(r,g+0x26), 'hpmax', u8(r,g+0x8C), 'lvl', u8(r,g+0x8F), 'exit', hex(u8(r,g+0xF)), 'diff', u8(r,g+0x68))
