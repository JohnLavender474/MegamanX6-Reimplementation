import struct
R=['zero','at','v0','v1','a0','a1','a2','a3','t0','t1','t2','t3','t4','t5','t6','t7','s0','s1','s2','s3','s4','s5','s6','s7','t8','t9','k0','k1','gp','sp','fp','ra']
OP={0x08:'addi',0x09:'addiu',0x0a:'slti',0x0b:'sltiu',0x0c:'andi',0x0d:'ori',0x0e:'xori',0x0f:'lui',
0x20:'lb',0x21:'lh',0x23:'lw',0x24:'lbu',0x25:'lhu',0x28:'sb',0x29:'sh',0x2b:'sw',0x22:'lwl',0x26:'lwr',0x2a:'swl',0x2e:'swr',0x32:'lwc2',0x3a:'swc2'}
FN={0x00:'sll',0x02:'srl',0x03:'sra',0x04:'sllv',0x06:'srlv',0x07:'srav',0x08:'jr',0x09:'jalr',0x0c:'syscall',0x0d:'break',0x10:'mfhi',0x11:'mthi',0x12:'mflo',0x13:'mtlo',0x18:'mult',0x19:'multu',0x1a:'div',0x1b:'divu',0x20:'add',0x21:'addu',0x22:'sub',0x23:'subu',0x24:'and',0x25:'or',0x26:'xor',0x27:'nor',0x2a:'slt',0x2b:'sltu'}
def dis(w,pc):
    op=w>>26; rs=(w>>21)&31; rt=(w>>16)&31; rd=(w>>11)&31; sh=(w>>6)&31; fn=w&63
    imm=w&0xffff; simm=imm-0x10000 if imm&0x8000 else imm
    if w==0: return 'nop'
    if op==0:
        n=FN.get(fn,'?sp%x'%fn)
        if n in('sll','srl','sra'): return f'{n} {R[rd]},{R[rt]},{sh}'
        if n=='jr': return f'jr {R[rs]}'
        if n=='jalr': return f'jalr {R[rd]},{R[rs]}'
        if n in('mfhi','mflo'): return f'{n} {R[rd]}'
        if n in('mult','multu','div','divu'): return f'{n} {R[rs]},{R[rt]}'
        if n in ('sllv','srlv','srav'): return f'{n} {R[rd]},{R[rt]},{R[rs]}'
        return f'{n} {R[rd]},{R[rs]},{R[rt]}'
    if op==1:
        n={0:'bltz',1:'bgez',16:'bltzal',17:'bgezal'}.get(rt,'?regimm')
        return f'{n} {R[rs]},{pc+4+simm*4:#x}'
    if op in(2,3): return f"{'j' if op==2 else 'jal'} {((pc+4)&0xf0000000)|((w&0x3ffffff)<<2):#x}"
    if op in(4,5): return f"{'beq' if op==4 else 'bne'} {R[rs]},{R[rt]},{pc+4+simm*4:#x}"
    if op in(6,7): return f"{'blez' if op==6 else 'bgtz'} {R[rs]},{pc+4+simm*4:#x}"
    if op==0x12: return f'cop2 {w:#x}'
    if op==0x10: return f'cop0 {w:#x}'
    n=OP.get(op,'?op%x'%op)
    if n=='lui': return f'lui {R[rt]},{imm:#x}'
    if n in('andi','ori','xori'): return f'{n} {R[rt]},{R[rs]},{imm:#x}'
    if op>=0x20: return f'{n} {R[rt]},{simm}({R[rs]})'
    return f'{n} {R[rt]},{R[rs]},{simm}'
class Img:
    def __init__(s,ram): s.ram=ram
    def w(s,a): o=a&0x1fffff; return struct.unpack_from('<I',s.ram,o)[0]
    def dump(s,a,n):
        for i in range(n):
            pc=a+4*i; w=s.w(pc); print(f'{pc:08x}: {w:08x}  {dis(w,pc)}')
def load_image():
    import os
    ram=bytearray(0x200000)
    exe=open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'),'rb').read()
    body=exe[0x800:]; ram[0x10000:0x10000+len(body)]=body
    ov=open(os.path.expanduser('~/mmx-data/x6/extracted/retail/rock_bin/02.bin'),'rb').read()
    ram[0xE9860:0xE9860+len(ov)]=ov
    return ram, len(ov)
