import sys, json, struct, shutil, hashlib
from pathlib import Path
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.savestate import read_state, ram_from, ram_at
from mmxdemake.sprites import scratchpad

S = Path(sys.argv[1]); out = Path(sys.argv[2]); state = 'scaravich-10_section-4-sabre.state.gz'
path = Path('/home/johnlavender/mmx-data/x6/dumps') / state
data = read_state(path)
ram = ram_from(data, Path('/home/johnlavender/mmx-data/x6/extracted/retail/iso/SLUS_013.95').read_bytes(), path)
pal_base = struct.unpack_from('<I', scratchpad(data), 0x24)[0]
src = json.loads((S / 'sabre/pal3/players/zero/animations.json').read_text())
A = {a['index']: a for a in src['animations']}

def box(table, i):
    ox, oy, w, h = struct.unpack('<bbBB', ram_at(ram, table + 4 * i, 4))
    return {'ox': ox, 'oy': oy, 'w': w, 'h': h}

ids = {}
for i in range(25):
    anim, pal, extend, b3 = ram_at(ram, 0x801EE68C + 4 * i, 4)
    if anim:
        ids.setdefault(anim, []).append({'id': 24 + i, 'palette': pal, 'table_bytes': [anim, pal, extend, b3]})
body = {24: 88, 25: 89, 26: 90}

if out.exists():
    shutil.rmtree(out)
out.mkdir(parents=True)
anims, skipped = [], []
for k in sorted(ids):
    if k not in A:
        skipped.append({'index': k, 'ids': [e['id'] for e in ids[k]], 'reason': 'not in the Zero export (see its skipped list)'})
        continue
    pal = ids[k][0]['palette']
    variants = {pal: f'anim-{k}.png'}
    if pal == 3:
        variants[4] = f'anim-{k}-palette-4.png'
        variants[5] = f'anim-{k}-palette-5.png'
    if any(e['id'] == 26 for e in ids[k]):
        variants[8] = f'anim-{k}-palette-8.png'
    for n, name in variants.items():
        shutil.copy(S / f'sabre/pal{n}/players/zero/anim-{k}.png', out / name)
    frames = []
    for f in A[k]['frames']:
        fr = dict(f)
        if 0 < f['flag'] <= 90:
            fr['box'] = box(0x801EE414, f['flag'])
            fr['box_long'] = box(0x801EE580, f['flag']) if f['flag'] < 67 else None
        frames.append(fr)
    anims.append({'index': k, 'weapon_ids': [e['id'] for e in ids[k]], 'sabre_tbl': {str(e['id']): e['table_bytes'] for e in ids[k]},
                  'zero_animation': next((body[e['id']] for e in ids[k] if e['id'] in body), None),
                  'palette': pal, 'palette_files': {str(n): v for n, v in variants.items()},
                  'loop_to': A[k]['loop_to'], 'frames': frames})
doc = {'name': 'zero-sabre', 'fps': src['fps'], 'cell': src.get('cell'), 'origin': src.get('origin'),
       'note': 'Sabre hit objects (weapon ids 24-31, 35-42, 48). Each strip uses the same cell and origin as players/zero, '
               'so it lines up with the Zero animation of the same frame. A frame flag 1-90 is the box index in 0x801EE414 '
               '(box_long: 0x801EE580, with part 0x800000). The object draws with CLUT 0x7801; PaletteSlotLoad copies palette '
               '`palette` of the list into that slot. Palette 4: part 0x400000 or Black Zero; 5: part 0x1000000; 8: id 26 in Zero step 55.',
       'palette_list': f'{pal_base:#010x}',
       'palettes': {str(n): [f'{w:04x}' for w in struct.unpack('<16H', ram_at(ram, pal_base + 32 * n, 32))] for n in range(3, 9)},
       'animations': anims, 'skipped': skipped,
       'source': {'state': state, 'state_sha1': hashlib.sha1(path.read_bytes()).hexdigest(),
                  'live': {'zero_step': 48, 'zero_animation': 88, 'weapon_id': 24, 'object_animation': 112,
                           'frame_flag': 6, 'box_ptr': '0x801ee42c', 'clut': '0x7801'}}}
(out / 'animations.json').write_text(json.dumps(doc, indent=1))
print(len(anims), 'animations,', len(skipped), 'skipped:', skipped)
for a in anims: print(a['index'], a['weapon_ids'], 'pal', a['palette'], list(a['palette_files']))
