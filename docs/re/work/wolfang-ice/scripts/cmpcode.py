import re, sys
def ops(path):
    out=[]
    for line in open(path):
        m=re.match(r'([0-9a-f]{8}): ([0-9a-f]{8})  (\S+)', line)
        if m: out.append((int(m.group(1),16), int(m.group(2),16), m.group(3)))
    out.sort()
    return out
a, b = ops(sys.argv[1]), ops(sys.argv[2])
print(len(a), len(b))
diff=0
for (pa,wa,ma),(pb,wb,mb) in zip(a,b):
    if wa!=wb:
        if ma!=mb or ma not in ('j','jal','lui','addiu','lw','sw','lh','lb','lbu','lhu','sh','sb','beq','bne','bgez','bltz','blez','bgtz') or (wa>>16)!=(wb>>16) and ma not in ('j','jal'):
            diff+=1; print(hex(pa),hex(wa),ma,'|',hex(pb),hex(wb),mb)
print('differences not explained by address parts:', diff)
