# Pseudo code: stage 07 laser puzzle

## Dispatch tables

| table | entries |
|---|---|
| `0x800FBF20` type 13 states | 0 set-up `0x800EA13C`, 1 beamer `0x800EC9B8`, 2 reflector `0x800EA4CC`, 3 door `0x800ECA5C`, 4 delete `0x800ECB00` |
| `0x800FBF34` beamer steps | 0 HitReactionDispatch, 1 nop, 2 nop (relay idle), 3 fixed `0x800ECB3C`, 4 aim `0x800ECC38` |
| `0x800FBF68` fixed subs | `0x800ECB78`, `0x800ECB90`, `0x800ECBE4`, `0x800ECC10` |
| `0x800FBF78` aim subs | `0x800ECC74`, `0x800ECD40`, `0x800ECDB0` |
| `0x800FBF48` reflector steps | 0 HitReactionDispatch, 1 nop, 2 re-fire `0x800ECDF4` |
| `0x800FBF54` door steps | 0 HitReactionDispatch, 1 nop, 2 nop, 3 `0x800ECE54`, 4 `0x800ECEA0` |
| `0x800FBF84` door step 3 | `0x800EA650` open, `0x800ECE90` -> step 4 |
| `0x800FBF8C` door step 4 | `0x800EA7C8` passed, `0x800ECEDC` nop |
| `0x800FBFA0` visual 11 | `0x800EAF28` set-up, `0x800EB0F0` main, `0x800EB734` end, `0x800ED290` delete |
| `0x800FBFB0` misc 7 | `0x800ED2B0` draw once, `0x800ED304` delete |
| `0x8007614C` weapon 79 | `0x800444B0` set-up, `0x800444EC` follow, `0x80044534` delete |

### set-up — 0x800EA13C (state 0)
```
hp=127; atk=1; anime_tbl=0x800FBA58; in_dir=64; flip=0; pri=7; dmg_tbl=0x8007451C; boxes=0; speeds=0
shot_id = ctr++
switch var>>4:
 0: dir=n; anim 8+n; state1 step3          1: dir=n; anim 8+n; state1 step4
 2: dir=n; hurt (-8,-8,16,16); anim n; state1 step2
 3: dir=n; hurt box[n]; anim 16+n; state2 step2
 4: ix=setup[n].start; var=0x40|setup[n].list; dir=list[var&15][ix]; hurt box[dir]; anim 16+dir; state2 step2
 5: anim (n in 4,5 ? 32 : 33); hurt (n in 4,5 ? (24,-16,16,16) : (-16,-40,16,16));
    SpawnInRect(rect for n; none for n=3); state3 step2
step2=0
```
### beamer main — 0x800EC9B8 (state 1)
```
prev=pos; AnimeTick; step()
if var>>4==2 and Catch() and step!=4: step=4, sub=0
DisplayCheck(16,16)
```
### fixed: 0x800ECB78 / ECB90 / ECBE4 / ECC10
```
0: timer=3; sub++     1: if --timer==0: Fire(); sub++
2: if link==2: link=0; timer=120; sub++     3: if --timer==0: sub=0
```
### aim: 0x800ECC74 / ECD40 / ECDB0
```
0: if link==2 link=0; if link!=0 return
   if |mega.x-x|<=144: dir = AimStep()      ; 0x800ED0AC: one 45° step toward aim8[AimAt(me,mega)]
   SetAnime(var>>4==1 ? 8+dir : dir); sub++
1: if not (in_dir == dir±4): Fire(); timer=302; sub++
2: if link==2 link=0; if --timer==0 sub=0
```
### Fire — 0x800EABD8
```
lp=GetVisualObject(); if none return; sound(2,19)
lp: 0x41, type 11, parent=me, dir=me.dir, copies parent, sprite source = bank entry +0x40
lp.pos = me.pos + 16 px in dir (12,12 for diagonals)
lp.id = ctr; me.shot_id = ctr++; me.link = 1
```
### reflector main — 0x800EA4CC (state 2)
```
prev=pos; AnimeTick; step()          ; step2 = re-fire
DisplayCheck(16,16); hp=127
if ContactWeapon():
   wait=33; dmg_tbl=0x800745BC
   if var>>4==4:
      if link==1: fire_req=1; link=0
      sound(2,78); ix=(ix+1)&7; dir=list[var&15][ix]; hurt box[dir]; SetAnime(16+dir)
if Catch(): if link==0: fire_req=1
else link=0
if wait and --wait==0: dmg_tbl=0x8007451C
```
### re-fire — 0x800ECDF4
```
if link==0: if fire_req==1: Reflect(); fire_req=0
elif link==2: link=0; repin=1
```
### Reflect — 0x800EADA8
```
ix = reflect_ix[dir][in_dir]; if ix==255 return
e = reflect_out[ix]; if e.dir==255 return
lp=GetVisualObject(); sound(2,18); as Fire but dir=e.dir, pos=me.pos+(e.dx,e.dy)
```
### Catch — 0x800EAA00
```
for w in weapon slots, type 79:
   if w.id == me.shot_id: continue
   if not BoxOverlap(w, me): continue
   if w.pin != 0: return 0
   if me.repin: w.pin=1; me.repin=0
   else:
      d=(0,0)
      reflector: ix=reflect_ix[me.dir][w.dir]; if ix!=255: d=(e.dx,e.dy)
      door: d = n in 4,5 ? (30,-8) : (-8,-30)
      w.pin=2; w.pos = me.pos + d
   me.in_dir = w.dir; return 1
return 0
```
### door main — 0x800ECA5C (state 3)
```
prev=pos; step(); DisplayCheck(48,48)
if Catch(): if step==2: sound(2,32); step=3; sub=0
else link=0
```
### door open — 0x800EA650
```
AnimeTick; if anime.flag != 1 return
n: 0..3 cam0.max_x = {3488,3824,4480,4832}; 4,5 cam0.max_y = {512,768}
CopyTiles(0, dx, dy, src[n]); sub++
```
### door passed — 0x800EA7C8
```
n 0..3: if cam0.x < x+16 return; DeleteInRect(rect[n]); cam0.min_x = x+16
n 4,5:  if cam0.y < y+16 return; DeleteInRect(rect[n]); CopyTiles(0, n==4?4688:3888, n==4?448:704, 0x800FBEC0)
        n==4: cam0.min_x=3840; cam0.min_y=cam0.max_y=y+32
sub++
```
### DeleteInRect — 0x800ED1CC
```
for each main object, type 13, origin in rect: work.7E=2; state=4
```
### delete — 0x800ECB00 (state 4)
```
if --work.7E == 0: DeleteObject3
```
### laser set-up — 0x800EAF28
```
if not Alive(): state=2; return       ; Alive 0x800ED06C: parent.state!=4 and parent.link!=0
pri=6; life=182; atk=1; hp=0; step3=0; anm5=0; atk box (-8,-8,16,16); no hurt/terrain box; state=1
start = pos
w = GetWeaponObject(): 0x41, type 79, parent=me, dir, id=me.id; me.probe=w
speed by dir: 16 px/move on x and/or y
```
### laser main — 0x800EB0F0
```
if Alive():
   pin = probe.pin; if pin==2: pos=probe.pos; probe.pin=0    ; 0x800ECFD0
   ContactMega()
   if life==0 or --life != 0:
      if not Wall(pos) and step3==0 and pin!=2: MoveObject; clamp x,y >= 0
      Draw3(); step3=(step3+1)%3; anm5=(anm5+1)%5; return
   parent.link = 2
state++; Draw3()
```
### laser end — 0x800EB734
```
if sub==0: sub=1; t=13; if frame odd: Draw3(); return
if frame odd: Draw3()
if --t == 0: state++
```
### weapon 79 follow — 0x800444EC
```
if laser.state != 1: atk box=0; pin=0; state++  else pos = laser.pos
```

## Engine helpers called

| address | name | use |
|---|---|---|
| `0x800179A4` | SetAnime | poses, door animation |
| `0x80017A04` | AnimeTick | beamers, reflectors, door opening |
| `0x80016C48` | PlaySound | bank 2: 18, 19, 32, 78 |
| `0x80027BA0` | CopyTiles | door tiles |
| `0x80029F38` | SpawnInRect | next room |
| `0x8002C32C` | GetWeaponObject (new name) | laser probe |
| `0x8002C458` | GetVisualObject | laser |
| `0x8002C530` | GetMiscObject | draw pieces |
| `0x8002C5F0` | GetMiscObject2 walk (new) | body pieces |
| `0x8002C9B0` | DeleteObject3 | all ends |
| `0x8002CCB0` | DisplayCheck | |
| `0x8002D2B0` | MoveObject | laser head |
| `0x8002D380` | AimAt | aiming beamer |
| `0x8002D654` | BoxOverlap | catch test |
| `0x80030234` | TileCollisionAt (new) | laser wall test |
| `0x80030ECC` | ContactMega | laser head |
| `0x80031474` | ContactWeapon | reflector turn |

## Object struct (the fields used here)

| offset | type 13 | laser (visual 11) | probe (weapon 79) |
|---|---|---|---|
| `+0x7C` | timer (u16) | parent | parent (laser) |
| `+0x7E` | delete timer | – | – |
| `+0x84` | own shot id (u16) | direction (u32) | direction |
| `+0x86` | last caught direction | – | – |
| `+0x87` | turn list index | – | – |
| `+0x88` | direction | move counter mod 3 | – |
| `+0x89` | link: 0/1/2 | – | – |
| `+0x8A` | re-pin flag | life (u16) | – |
| `+0x8B` | wait-table timer | – | – |
| `+0x8C` | fire request | probe pointer | laser's shot id (u16) |
| `+0x90` | – | shot id | – |
| `+0x92`, `+0x94` | – | start x, y | – |
| `+0x96` | – | sprite counter mod 5 | – |
| `+0x98` | – | pin copy | pin: 0, 1, 2 |
