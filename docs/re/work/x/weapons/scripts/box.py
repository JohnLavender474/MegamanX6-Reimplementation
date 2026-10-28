from wp import *
r=load()
for a in sys.argv[1:]:
    a=int(a,16); print(hex(a), [s8(r,a+i) for i in range(4)])
