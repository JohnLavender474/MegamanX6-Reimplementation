import sys, struct, gzip, hashlib
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_state, ram_from

EXE = (Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95").read_bytes()
DUMPS = Path.home() / "mmx-data/x6/dumps"
OUT = Path(sys.argv[1]) if len(sys.argv) > 1 else None
NAMES = ["title-1_capcom-logo", "title-2_loading", "title-3_anime-intro", "title-4_press-start", "title-5_main-menu"]

def u8(r, a): return r[a & 0x1FFFFF]
def u16(r, a): return struct.unpack_from("<H", r, a & 0x1FFFFF)[0]
def s16(r, a): return struct.unpack_from("<h", r, a & 0x1FFFFF)[0]
def u32(r, a): return struct.unpack_from("<I", r, a & 0x1FFFFF)[0]
def hx(r, a, n): return r[a & 0x1FFFFF:(a & 0x1FFFFF) + n].hex(" ")

def preview(state, path):
    w, h = 128, 96
    px = state[0x25:0x25 + w * h * 3]
    rows = b"".join(b"\0" + bytes(c for i in range(w) for c in px[(y*w+i)*3:(y*w+i)*3+3][::-1]) for y in range(h))
    import zlib
    def chunk(t, d): return struct.pack(">I", len(d)) + t + d + struct.pack(">I", zlib.crc32(t + d))
    path.write_bytes(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0)) + chunk(b"IDAT", zlib.compress(rows)) + chunk(b"IEND", b""))

for n in NAMES:
    p = DUMPS / f"{n}.state.gz"
    st = read_state(p)
    r = ram_from(st, EXE, p)
    print("=" * 20, n, hashlib.sha1(p.read_bytes()).hexdigest()[:12])
    if OUT: preview(st, OUT / f"{n}.png")
    G = 0x800CCED0
    print("game +00..+1f:", hx(r, G, 0x20))
    print("game stage/area", u8(r, G + 0x0C), u8(r, G + 0x0D), "mode", u8(r, G), "sub", u8(r, G + 1))
    FE = 0x800CD3F8
    print("fe:", hx(r, FE, 0x18), "state", u8(r, FE), "sub", u8(r, FE + 1), "row", u8(r, FE + 2), "timer", u16(r, FE + 4))
    print("tasks:", [(u16(r, 0x801F8100 + i * 0x80), u16(r, 0x801F8102 + i * 0x80), hex(u32(r, 0x801F8100 + i*0x80 + 0x10))) for i in range(4)], "cur", hex(u32(r, 0x801F8300)))
    print("display_mode", u8(r, 0x80090D6A), "screen_width", u16(r, 0x8009B79C), "fade_dir", u8(r, 0x80097424),
          "arc_status", hex(u8(r, 0x8009509C)), "upload_pending", u8(r, 0x8008EC08), "reset_enable", u8(r, 0x8008EC0C))
    print("opts 0x8006D5F0:", hx(r, 0x8006D5F0, 0x20))
    print("movie row 8/9:", hx(r, 0x80070CA4 + 8 * 12, 24), "row0/1:", hx(r, 0x80070CA4, 24))
    for base, cnt, name in ((0x800CD410, 96, "misc"), (0x800964A0, 32, "misc2")):
        for i in range(cnt):
            a = base + i * 0x60
            if u8(r, a):
                print(f"  {name}[{i}] @{a:08X} fl {u8(r,a):02x} type {u8(r,a+1)} var {u8(r,a+2)} disp {u8(r,a+3)} st {u8(r,a+4)} step {u8(r,a+5)} {u8(r,a+6)} {u8(r,a+7)} x {s16(r,a+0xA)} y {s16(r,a+0xE)} layer {s16(r,a+0x14)&0xff} prio {u8(r,a+0x16)} anim {u8(r,a+0x17)} sprite {u8(r,a+0x47)} tex {u16(r,a+0x40):04x} clut {u16(r,a+0x42):04x} sd {u32(r,a+0x3C):08x} at {u32(r,a+0x30):08x}")
                print("     raw", hx(r, a, 0x60))
    for base, cnt, sz, name in ((0x8008EF48, 48, 0x9C, "main"), (0x80093C98, 32, 0xA0, "visual"), (0x80091298, 96, 0x70, "effect"), (0x800C1220, 80, 0xA4, "stageobj"), (0x8009B7B0, 96, 0x30, "logic"), (0x8009C9B0, 64, 0x60, "ray")):
        live = [(i, u8(r, base + i*sz + 1), u8(r, base + i*sz + 2)) for i in range(cnt) if u8(r, base + i*sz)]
        if live: print(f"  {name}: {live}")
    print("mega flags", hex(u8(r, 0x800970A0)))
    sp = 0x1F800000
