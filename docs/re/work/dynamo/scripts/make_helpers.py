import json, re
from pathlib import Path
O = 45
F = {
 "0x800FA028": ("DynamoInit", "type 63 state 0: boss_hp_max = (u8)(58 + {-8, 0, 4, 8}[boss level]) = 50/58/62/66, contact 4, dmg_tbl_dynamo, room points to world, PlayerEvent(20, 64), camera +0x48 = 1, step 2 (INTRO)"),
 "0x800FA23C": ("DynamoMain", "type 63 state 1: boxes by sprite; ContactWeapon/ContactMega; 30 updates of dmg_tbl_deflect after a hit (2 for ids 24, 25); ids 6, 15, 42, 46 start STUN; steps 0x800FCD7C"),
 "0x800FA454": ("DynamoVolleyFire", "CENTRE VOLLEY sub 2: every 50 updates a blade (visual 34 var 2 for X, 3 for Zero), {6, 7, 8, 8}[level]; then wait for no blades, animation 9"),
 "0x800FA5A0": ("DynamoPillarJump", "PILLARS sub 0: jump (x speed = distance / 42, 3 up, gravity 0x2666) to the room side away from the player, or charge at once within 32 px"),
 "0x800FA6EC": ("DynamoPillarStrike", "PILLARS sub 2: 80 updates of charge, animation 7; flag 2 Rumble(5), flag 1 eight pillars (visual 34 var 4) 36 px apart"),
 "0x800FA864": ("DynamoStunShake", "STUN sub 1: 80 updates of shaking; a hit ends it and drops main 15 var 0x32 (200 souls) while game+0x33 < 3; then the escape jump"),
 "0x800FA984": ("DynamoBladeInit", "visual 34 var 0-3 set-up: animation 13, contact 6, HP 100, dmg_tbl_dynamo_blade, 4.5 px/update (var 0 straight, 1-3 aimed); Dynamo +0x07 += 1"),
 "0x800FABC0": ("DynamoBladeMain", "visual 34 var 0-3 main (switch 0x800FA014): out, wait (180 / 120), back to Dynamo or at the player, knocked away by a hit; ends when dynamo_dead or dynamo_stunned"),
 "0x800FB214": ("Dynamo", "boss overlay 45: main type 63 entry; states 0x800FCD70 (set-up, main, death); CollideCheck"),
 "0x800FB27C": ("DynamoIntro", "step 2 INTRO: sub-steps 0x800FCDA4 (warning, beam-in, text 2 once, boss music, HP fill)"),
 "0x800FB350": ("DynamoIntroBeamIn", "INTRO sub 1: camera target min x = max x - 16 (old in +0x94); when the WARNING is gone sound 2/71, animation 0, 8 px/update down"),
 "0x800FB414": ("DynamoIntroLand", "INTRO sub 2: terrain box at world y >= 300; at the end of animation 0 TextOpen(2) once per area (TextSeen bit 2)"),
 "0x800FB54C": ("DynamoIntroHpFill", "INTRO sub 4: camera min x restored; HP + 1 per update with tick 0/14 every 3; then camera +0x48 = 8, PlayerEventEnd, floor y to 0x800FCE64, THINK"),
 "0x800FB74C": ("DynamoThink", "step 4: GetRNG & 15; HP > max/2: < 10 pattern 0 (boomerang) else 1 (pillars); else < 6 pattern 2 (pillars) else 3 (centre volley, leaping slash)"),
 "0x800FB824": ("DynamoBoomerang", "step 5: {3, 3, 4, 5}[level] throws (visual 34 var 0, then 1); waits until +0x07 (live blades) = 0"),
 "0x800FBA6C": ("DynamoCentreVolley", "step 6: jump to the room centre, then DynamoVolleyFire"),
 "0x800FBCF8": ("DynamoLeapingSlash", "step 7: animation 4 flags: 1 jump at the player, 2 slash sounds, 3 second hop; sprite 52 has the slash box (-100, -4, 140, 68)"),
 "0x800FC038": ("DynamoPillars", "step 8: sub-steps 0x800FCDF0 (jump, land, strike)"),
 "0x800FC13C": ("DynamoStun", "step 9: sub-steps 0x800FCDFC (hurt, shake, escape jump to the other side, land; dynamo_stunned = 0)"),
 "0x800FC3C0": ("DynamoDeath", "type 63 state 2: steps 0x800FCE0C: flash 127 updates, BossExplosion, stand, TextOpen(3), beam-out, exit_req 0x10"),
 "0x800FC74C": ("DynamoLeave", "death last step: at the animation 12 flag sound 2/72 and 8 px/update up; at its end exit_req = 0x10, DeleteObject3"),
 "0x800FC7D8": ("DynamoShot", "visual type 34 entry: var 0-3 blades (states 0x800FCE30), var 4 pillars (states 0x800FCE3C)"),
 "0x800FC9AC": ("DynamoPillarMain", "visual 34 var 4 main: ContactMega; attack box (-11, -96, 22, 192) from update 20; end at 60; contact 8"),
}
D = {
 "0x800FA004": ("dynamo_hp_bonus", "s16 x 4: -8, 0, 4, 8 by boss level (read as bytes)"),
 "0x800FA00C": ("dynamo_throws", "u8 x 4: boomerang throws by boss level: 3, 3, 4, 5"),
 "0x800FA010": ("dynamo_volley", "u8 x 4: centre-volley blades by boss level: 6, 7, 8, 8"),
 "0x800FCA84": ("dynamo_terrain_box", "(0, 0, 12, 32): 24 x 64 px"),
 "0x800FCA88": ("dynamo_box_body", "(-6, -22, 12, 50): attack and hurt box of most sprites (0x800FCA94 the same)"),
 "0x800FCA90": ("dynamo_box_slash", "(-100, -4, 140, 68): attack box of sprite 52"),
 "0x800FCCD4": ("dynamo_anims", "15 animations: 0 beam-in, 1 stand, 2 throw, 3 catch, 4 leaping slash, 5 jump, 6 charge, 7 pillar strike, 8 volley, 9 volley end, 10 hurt, 11 stand, 12 beam-out, 13 blade, 14 pillar"),
 "0x800FCD10": ("dynamo_patterns", "5 x 6 bytes: step lists ended by THINK (4)"),
 "0x800FCE48": ("dynamo_room_points", "7 halfwords, room values made world at the set-up: spawn (256, -16), x 48, 304, 176, y 16, 176"),
 "0x800FCE58": ("dynamo_dead", "1 after Dynamo's kill; his shots end"),
 "0x800FCE5C": ("dynamo_stunned", "1 while Dynamo is stunned; his shots end"),
 "0x800FCE60": ("dynamo_obj", "Dynamo's object (the shots read it)"),
 "0x800FCE64": ("dynamo_floor_y", "Dynamo's y at the end of the intro; the death puts him back there"),
}
G = {
 "0x80075A5C": ("dmg_tbl_dynamo", "Dynamo (main type 63): lemon 1, full charge parts 4, ids 6/15/42/46 = 5/8/8/4 (they stun him), 17/22/47/57 = 8, 60/62 = 15, 21/23/44/64-78 deflected"),
 "0x80075AFC": ("dmg_tbl_dynamo_blade", "Dynamo blades: all deflected except ids 24-31, 33, 37-42, 47, 56, 57 (1); such a hit bats the blade away"),
}
cat = json.load(open("/home/johnlavender/IdeaProjects/mmx-demake-tools/catalog/helpers.json"))
print("catalog sections", list(cat.keys()))
allnames = {}
for sec, ent in cat.items():
    if isinstance(ent, dict):
        for a, e in ent.items():
            if isinstance(e, dict) and "name" in e:
                allnames.setdefault(e["name"], []).append((sec, a))
                for alt in e.get("alt", []):
                    allnames.setdefault(alt["name"], []).append((sec, a))
out = {"functions": {}, "globals": {}}
alts = []
for sec, src in (("functions", F), ("globals", {**D, **G})):
    for a, (n, note) in sorted(src.items()):
        if n in allnames: print("NAME CLASH", n, allnames[n])
        e = {"name": n, "note": note}
        if a not in G: e["overlay"] = O
        existing = None
        for s2, ent in cat.items():
            if isinstance(ent, dict) and a in ent: existing = (s2, ent[a])
        if existing:
            print("ADDRESS EXISTS", a, existing[0], existing[1].get("name"), existing[1].get("overlay"))
            alts.append((a, existing, e))
        else:
            out[sec][a] = e
out["alt_additions"] = {a: {"section": ex[0], "existing_name": ex[1]["name"], "existing_overlay": ex[1].get("overlay"), "add_alt": {"overlay": O, "name": e["name"], "note": e["note"]}} for a, ex, e in alts}
Path("/home/johnlavender/mmx-data/x6/re/dynamo/helpers-proposed.json").write_text(json.dumps(out, indent=2) + "\n")
print(len(out["functions"]), len(out["globals"]), len(alts))
