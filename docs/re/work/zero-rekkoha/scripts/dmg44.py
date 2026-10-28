import re
from zprobe import *
r=load("heatnix-3_rekkoha")
doc=open("/home/johnlavender/IdeaProjects/mmx-demake-tools/docs/engine.md").read()
seen=set()
for name,addr in re.findall(r"`(dmg_tbl_\w+)` \(`(0x[0-9A-Fa-f]{8})`", doc):
    if name in seen: continue
    seen.add(name); a=int(addr,16)
    c,d=u8(r,a+2*44),u8(r,a+2*44+1)
    s={0:str(d),2:"–",0xFF:"D",7:"absorbed"}.get(c,f"class {c} dmg {d}")
    print(f"{name:28s} {addr}  id44: {s}")
