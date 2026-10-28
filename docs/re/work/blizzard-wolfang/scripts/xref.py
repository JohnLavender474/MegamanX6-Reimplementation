import sys, struct
from pathlib import Path
exe = (Path.home()/"mmx-data/x6/extracted/retail/iso/SLUS_013.95").read_bytes()
base = 0x80010000 - 0x800
for target in sys.argv[1:]:
    t = int(target, 16)
    hi = (t + 0x8000) >> 16; lo = t & 0xFFFF
    print("==", target)
    for off in range(0x800, len(exe) - 4, 4):
        w = struct.unpack_from("<I", exe, off)[0]
        op = w >> 26
        if (w & 0xFFFF) == lo and op in (0x20,0x21,0x23,0x24,0x25,0x28,0x29,0x2B,0x09):
            # check a lui with hi within 8 previous instructions
            for k in range(1, 9):
                pw = struct.unpack_from("<I", exe, off - 4*k)[0]
                if pw >> 26 == 0x0F and (pw & 0xFFFF) == hi:
                    print(hex(base + off), hex(w), {0x20:"lb",0x21:"lh",0x23:"lw",0x24:"lbu",0x25:"lhu",0x28:"sb",0x29:"sh",0x2B:"sw",0x09:"addiu"}[op]); break
