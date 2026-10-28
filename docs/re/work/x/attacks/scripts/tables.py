from probe import *
r=load(STATES[0])
print("spawn 0x80073f74:")
for i in range(64): print(f" {i}:{u32(r,0x80073f74+4*i):08x}", end="\n" if i%8==7 else "")
print("types 0x80071220:")
for i in range(64): print(f" {i}:{u32(r,0x80071220+4*i):08x}", end="\n" if i%8==7 else "")
for name,a,n in [("pose_tbl 741b4",0x800741b4,64),("max 74074",0x80074074,64),("max_part 740b4",0x800740b4,64),("one_at_a_time 740f4",0x800740f4,64),("count98 74134",0x80074134,64),("count99 74174",0x80074174,64),("e40",0x80073e40,64),("e00",0x80073e00,64)]:
    print(name, list(bytes_at(r,a,n)))
t=bytes_at(r,0x80073efc,128)
print("timers efc (+91,+92):", [(i,t[2*i],t[2*i+1]) for i in range(64)])
print("ed8", list(bytes_at(r,0x80073ed8,12)), "ee4", list(bytes_at(r,0x80073ee4,12)), "ef0", list(bytes_at(r,0x80073ef0,12)))
