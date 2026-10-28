# compact function disassembly with helper names; usage: dz.py <state> addr...
import json, re
from wp import *
H=json.load(open("/home/johnlavender/IdeaProjects/mmx-demake-tools/catalog/helpers.json"))
N={int(k,16):v["name"] for k,v in H["functions"].items()}
G={int(k,16):v["name"] for k,v in H.get("fields",{}).items()}
r=load(sys.argv[1])
for a in sys.argv[2:]:
    a=int(a,16); print(f"; ---- {a:08x}")
    for l in dis_fn(r,a,1500):
        pc,w,t=l[:8],l[10:18],l[20:]
        if t.strip()=="nop": continue
        m=re.search(r"0x([0-9a-f]{8})$",t)
        if m and (t.startswith("jal") or t.startswith("j ")):
            x=int(m.group(1),16)
            if x in N: t+=f"  ; {N[x]}"
        print(pc[2:], t)
