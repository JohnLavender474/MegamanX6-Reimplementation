import re,sys
SEQ={0:[0,14,12,10,8,6,4,2],1:[1,15,13,11,9,7,5,3],2:[0,12,8,4]*2,3:[1,13,9,5]*2,4:[2,14,10,6]*2,5:[3,15,11,7]*2,6:[11,10,9,8,7,8,9,10],7:[8,6,4,2,0,14,10,9],8:[15,12,7,3,15,10,7,3]}
SET={0:(3,2),1:(1,1),2:(2,1),3:(6,0),4:(2,0),5:(1,4),6:(2,0),7:(0,4),8:(0,0),9:(5,0),10:(7,4),11:(8,1),12:(0,0)}
D8=['right','up-right','up','up-left','left','down-left','down','down-right']
for line in open(sys.argv[1]):
    m=re.search(r'\[(\d+)\] (\S+): flags(\S+) type13 id(\d+) kind(\S+) x(-?\d+) y(-?\d+)',line)
    if not m: continue
    i,a,f,v,k,x,y=m.groups(); v=int(v); hi,n=v>>4,v&15
    if hi==0: role=f'fixed beamer, dir {n} ({D8[n&7]})'
    elif hi==1: role=f'aiming beamer, start dir {n} ({D8[n&7]})'
    elif hi==2: role=f'relay beamer, start dir {n} ({D8[n&7]})'
    elif hi==3: role=f'fixed reflector, r {n}'
    elif hi==4:
        s,st=SET[n]; sq=SEQ[s]; role=f'turning reflector n{n}: seq {s} from index {st}: r = '+','.join(map(str,sq[st:]+sq[:st]))
    elif hi==5: role=f'gate {n}'
    else: role='?'
    print(f'| {i} | {x}, {y} | `{v:#04x}` | {k} | {role} |')
