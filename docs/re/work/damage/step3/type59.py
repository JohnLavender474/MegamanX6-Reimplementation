import struct, sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
sys.path.insert(0, str(Path(__file__).parent))
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad
import ram as R
exe = R.EXE.read_bytes()
seen = {}
for p in sorted(R.DUMPS.glob("*.state.gz")):
    data = read_state(p)
    r = ram_from(data, exe, p)
    sp = scratchpad(data)
    tbl = struct.unpack_from("<I", sp, 0x80)[0]
    ents = [R.word(r, tbl + 4 * t) for t in range(77)]
    stage = r[0x0CCED0 + 0x26 - 0x26]  # placeholder
    e59 = ents[59]
    # count how many entries equal the stub (most common value)
    from collections import Counter
    stub = Counter(ents).most_common(1)[0][0]
    print(f"{p.name:40s} tbl {tbl:#x} t59 {e59:#x} stub {stub:#x} {'STUB' if e59 == stub else 'REAL'}")
