import struct,sys
sys.path.insert(0,'/home/johnlavender/IdeaProjects/mmx-demake-tools')
from pathlib import Path
from mmxdemake.savestate import read_ram
from mmxdemake.objects import read_list
exe=Path.home().joinpath('mmx-data/x6/extracted/retail/iso/SLUS_013.95').read_bytes()
ram=read_ram(Path.home()/'mmx-data/x6/dumps/turtloid-1.state.gz',exe)
d=read_list(ram,6)
for k in range(0,len(d),8):
    a,t,i,kd,x,y=struct.unpack('<BBBBHH',d[k:k+8])
    if t==14 and kd!=4: print(f"flags {a:#x} type {t} id {i:#x} kind {kd:#x} x {x} y {y}")
open('/tmp/claude-1000/-home-johnlavender-IdeaProjects-Megaman-Maverick/73ae0096-cea4-4539-aa70-cb25540db2c1/scratchpad/wa/ram.bin','wb').write(ram)
