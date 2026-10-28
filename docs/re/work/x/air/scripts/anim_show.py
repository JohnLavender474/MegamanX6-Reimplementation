import json, sys
d = json.load(open(sys.argv[1]))
by = {a['index']: a for a in d['animations']}
for n in sys.argv[2:]:
    n = int(n)
    a = by.get(n)
    if a is None:
        print(n, 'MISSING'); continue
    fr = ' '.join(f"{f['ticks']}{'/'+hex(f['flag']) if f['flag'] else ''}" for f in a['frames'])
    extra = {k: v for k, v in a.items() if k not in ('frames', 'index')}
    print(n, len(a['frames']), 'frames:', fr, extra)
