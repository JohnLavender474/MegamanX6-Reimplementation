import sys, os
sys.path.insert(0, os.path.expanduser('~/IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import read_ram
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'),'rb').read()
for st in sorted(os.listdir(os.path.expanduser('~/mmx-data/x6/dumps'))):
    r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/'+st), EXE)
    print(f"{st:40s} st{r[0xCCEDC]:2d} ne{r[0xCD30A]} arr", ' '.join(f'{b}' for b in r[0xCD038:0xCD038+19]), ' r9697:', hex(r[0xCCFA8+12*4]), hex(r[0xCCFA8+12*4+1]))
