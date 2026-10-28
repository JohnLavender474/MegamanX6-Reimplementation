"""Find loads and stores of a RAM address (absolute imm) or of offset OFS from a register.
Usage: find_field_refs.py <state.gz> <abs_lo16_hex> [<ofs_dec>]"""
import struct, sys
from mmxdemake.savestate import read_state, RAM_OFFSET, RAM_SIZE
st = read_state(sys.argv[1]); ram = st[RAM_OFFSET:RAM_OFFSET + RAM_SIZE]
lo = int(sys.argv[2], 16)
ops = {0x20:'lb',0x24:'lbu',0x21:'lh',0x25:'lhu',0x23:'lw',0x28:'sb',0x29:'sh',0x2B:'sw'}
for a in range(0x10000, len(ram), 4):
    w = struct.unpack_from('<I', ram, a)[0]
    op = w >> 26
    if op in ops and (w & 0xFFFF) == lo:
        print(hex(0x80000000 + a), ops[op], 'rs', (w >> 21) & 31, 'rt', (w >> 16) & 31)
