import gzip, sys
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.png import write_png
d = gzip.open(sys.argv[1]).read()
px = d[0x25:0x25+128*96*3]
big=[]
for y in range(96):
    row=bytearray()
    for x in range(128):
        b,g,r = px[(y*128+x)*3:(y*128+x)*3+3]
        row += bytes((r,g,b,255))*3
    big += [bytes(row)]*3
write_png(sys.argv[2],384,288,big)
