import sys, re, difflib
def norm(path):
    out=[]
    for line in open(path):
        m=re.match(r'^([0-9a-f]{8}): ([0-9a-f]{8})  (\S+)', line)
        if not m: continue
        w=int(m.group(2),16); op=w>>26
        if op in (2,3): w&=0xfc000000
        elif op!=0: w&=0xffff0000
        out.append(w)
    return out
a=norm(sys.argv[1]); b=norm(sys.argv[2])
sm=difflib.SequenceMatcher(None,a,b,autojunk=False)
print(len(a),len(b),'ratio',round(sm.ratio(),4))
for tag,i1,i2,j1,j2 in sm.get_opcodes():
    if tag!='equal': print(tag,i1,i2,j1,j2)
