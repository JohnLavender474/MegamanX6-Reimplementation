import sys
from img import *
ram=image()
for pc in range(0x80010000,0x80080000,4):
    d=dis(w(ram,pc),pc)
    if (d.startswith('lbu ') or d.startswith('lb ')) and d.endswith('-12592(v0)') or d.endswith('-12592(v1)') or d.endswith('-12592(a0)'):
        # look ahead for jr to a register (table jump) within 12
        for k in range(1,14):
            e=dis(w(ram,pc+4*k),pc+4*k)
            if e.startswith('jr ') and e!='jr ra' or e.startswith('jalr'):
                print(hex(pc),d,'->',hex(pc+4*k),e);break
