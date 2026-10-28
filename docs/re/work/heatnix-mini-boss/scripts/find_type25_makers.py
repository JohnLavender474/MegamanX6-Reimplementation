"""Find code that writes 25 (main type) into byte +1 of an object, in RAM of a save state."""
import struct, sys
from mmxdemake.savestate import read_state, RAM_OFFSET, RAM_SIZE
ram = read_state(sys.argv[1])[RAM_OFFSET:RAM_OFFSET + RAM_SIZE]
val = int(sys.argv[2]) if len(sys.argv) > 2 else 25
for a in range(0x10000, 0x110000, 4):
    ins = struct.unpack_from('<I', ram, a)[0]
    if ins >> 26 == 9 and (ins >> 21) & 31 == 0 and ins & 0xFFFF == val:
        rt = (ins >> 16) & 31
        for k in range(1, 10):
            j = struct.unpack_from('<I', ram, a + 4 * k)[0]
            if j >> 26 == 0x28 and (j >> 16) & 31 == rt and j & 0xFFFF == 1:
                print(hex(0x80000000 + a), 'li', val, '-> sb +1 at', hex(0x80000000 + a + 4 * k)); break
