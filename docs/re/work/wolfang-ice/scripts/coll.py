import re, sys, collections
t=open('/home/johnlavender/mmx-data/x6/tmx/m096-blizzard-wolfang-section-1/blizzard-wolfang-section-1.tmx').read()
W=int(re.search(r'<map[^>]* width="(\d+)"',t).group(1)); H=int(re.search(r'<map[^>]* height="(\d+)"',t).group(1))
ts=re.findall(r'<tileset firstgid="(\d+)"[^>]*name="([^"]+)"',t)
print(W,H,ts)
fg=dict((n,int(g)) for g,n in ts)
m=re.search(r'<layer[^>]*name="collision".*?<data encoding="csv">(.*?)</data>',t,re.S)
vals=[int(v) for v in m.group(1).replace('\n','').split(',')]
g0=fg['collision']
grid=[[ (v-g0 if v else -1) for v in vals[y*W:(y+1)*W]] for y in range(H)]
c=collections.Counter(v for r in grid for v in r); print(sorted(c.items()))
def show(x0,x1,y0,y1):
    for y in range(y0//16,y1//16):
        print(f"{y*16:5d} "+' '.join(f"{grid[y][x]:2d}" if grid[y][x]>=0 else ' .' for x in range(x0//16,x1//16)))
if len(sys.argv)>1: show(*map(int,sys.argv[1:5]))
