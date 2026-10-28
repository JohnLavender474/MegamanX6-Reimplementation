import re
exec(open('coll.py').read().split("def show")[0].replace("print(W,H,ts)","").replace("c=collections.Counter(v for r in grid for v in r); print(sorted(c.items()))",""))
prof={25:(15,8),26:(7,0),27:(0,7),28:(8,15)}
def surface(x, y):
    for cy in range(y//16, H):
        v=grid[cy][x//16]
        if v in prof:
            a,b=prof[v]; return cy*16 + a + (b-a)*(x%16)/15, v
        if v in (60,57,56,58,33,34,62,63): return cy*16, v
        if v==16: continue
    return None, None
tbl=[(864,1024),(1104,944),(1600,752),(2224,496),(2384,480),(2656,400),(3320,224),(3568,224),(3856,224),(4000,224),(4720,224),(5080,224),(4720,432),(4720,648),(4720,848)]
for i,(x,y) in enumerate(tbl):
    s,v=surface(x,y-40 if i<6 else y)
    print(i, x, y, 'ceiling row above?', [grid[cy][x//16] for cy in range(max(0,y//16-2), y//16+1)], 'first floor below', s, v, 'gap', None if s is None else s-y)
