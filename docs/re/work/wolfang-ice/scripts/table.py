import sys, struct
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.savestate import read_state, RAM_OFFSET, RAM_SIZE
st = read_state(sys.argv[1]); ram = st[RAM_OFFSET:RAM_OFFSET+RAM_SIZE]
names={0:'avalanche L (var 4)',1:'avalanche R (var 5)',2:'falling ice (var 6)',3:'bouncing block (var 7)'}
ylim={12:448,13:688,14:848}
for i in range(16):
    a=(0x800f6e08+8*i)&0x1FFFFF
    x0,x,y,v,p=struct.unpack_from('<hhhBB',ram,a)
    print(f"{i:2d} trigger {x0}<mega.x<{x0+96}{'  mega.y>='+str(ylim[i]) if i in ylim else ''}  spawner x={x} y={y} var={v} pad={p}  -> {names.get(v,'?')}")
