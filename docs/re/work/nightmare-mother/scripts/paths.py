import sys; sys.path.insert(0, sys.argv[0].rsplit('/',1)[0])
from peek import *
r=load()
q={0:'TL',1:'TR',2:'BL',3:'BR'}
sel={}
for q2 in range(4):
    for q1 in range(4):
        v=s8(r,0x800fd4e4+q1+q2*4)
        if v>=0: sel[v]=(q[q1],q[q2])
def d(x,y):
    s={(1,0):'R',(-1,0):'L',(0,1):'U',(0,-1):'D',(0,0):'-'}
    return s.get((x,y),f'{x},{y}')
for name,base in (('var1',0x800fd040),('var2',0x800fd280)):
    print(name)
    for row in range(12):
        cells=[d(s32(r,base+row*48+m*8),s32(r,base+row*48+m*8+4)) for m in range(6)]
        print(f' row {row:2} (var1 {sel[row][0]}, var2 {sel[row][1]}):', ' '.join(cells))
print('fd4f4 x',[s32(r,0x800fd4f4+i*4)/65536 for i in range(8)])
print('fd514 y',[s32(r,0x800fd514+i*4)/65536 for i in range(8)])
