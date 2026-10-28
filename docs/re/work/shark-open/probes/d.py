import sys
sys.path.insert(0, '.')
from sp import *
ram = load(sys.argv[1])
print(dis(ram, int(sys.argv[2], 16), int(sys.argv[3])))
