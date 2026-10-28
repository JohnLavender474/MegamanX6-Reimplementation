import json, sys
path = sys.argv[1]
doc = json.load(open(path))
F = {
 "0x8001750C": ("StopSound", "StopSound(bank, id): stop a sound; (255, 0) stops all (High Max's kill)"),
 "0x8002CFB0": ("PlayerMoveAccel", "move by speed (x 86/100 in a head wind facing right), then accel_x in the facing direction and gravity; speed_y >= mega.fall_max"),
 "0x80039BC4": ("TryLand", "dash input, jump, or a direction (walk); used by land and the dash slide"),
 "0x80039C40": ("TryDashJump", "dash input (StartDash) or jump pressed (StartJump)"),
 "0x80039C9C": ("TryCrouch", "down held and game+0xC9 bit 3: StartCrouch"),
 "0x80039CFC": ("TryDirection", "right/left held and not blocked: flip and speed_x = +-mega+0xF0 (walk start)"),
 "0x80039D7C": ("TryDirectionWalk", "as TryDirection with mega+0xF4 (walk)"),
 "0x80039EB0": ("TryDashInput", "dash pressed (or mega+0x87): face the held side, speed_x = +-mega+0x104"),
 "0x80039FFC": ("DashStep", "dash move and timer; 1 = end (wall, timer 0, released, opposite side)"),
 "0x8003A3DC": ("TryLadder", "up on ladder tile 32 or down on ladder top 33"),
 "0x8003A870": ("TrySuperJump", "up + jump: step 31 at 2 x jump speed (X form 2, Zero skill bit 1)"),
 "0x8003A9C4": ("StartDash", "step 12 (dash), or a dash jump (step 6) when jump is pressed too"),
 "0x8003AD18": ("ToStand", "clear the dash flags, step 2, the stand animation (94 when shooting)"),
 "0x8003ADB4": ("Stop", "X: ToStand; Zero: animation 9, step 5"),
 "0x8003AE8C": ("ToWalkStart", "animation 7, step 3"),
 "0x8003AED4": ("ToWalk", "animation 8, step 4"),
 "0x8003AF1C": ("StartJump", "step 6 (part 2b)"),
 "0x8003B054": ("ToLand", "land sound, then a ground action or animation 12, step 8"),
 "0x8003B218": ("DashEnd", "animation 17, stop sound 5, speed = dash minimum, dash sub-step 2"),
 "0x8003B9AC": ("StartCrouch", "animation 81 (109 with the dash flag), step 26"),
 "0x8003CD00": ("DashTerrainBox", "terrain box 0x80073D58 (X) / 0x80073D5C (Zero), none when mega+0xD5 < 0"),
 "0x8003CD9C": ("ClearDashFlags", "mega+0x84 = +0x86 = 0; +0x8C = -1 when it was > 0"),
 "0x8003D10C": ("StandAnime", "animation 5, or 6 when HP < max HP / 3"),
 "0x8003D598": ("PlayerPhysicsDefaults", "the base speed constants mega+0xF0..+0x134 (docs/players/movement.md)"),
 "0x8003D630": ("PlayerPhysicsParts", "defaults, then the bonuses of the part bits mega+0xEC"),
 "0x8003D718": ("PlayerLoadForm", "mega+0xE7 = game+0x5E, mega+0xEC = game+0x74 + 4 x var, ..."),
 "0x8003F13C": ("WeaponPose", "X only: the special-weapon pose (animation 88 for weapons 5, 14)"),
 "0x8003F4C4": ("PlayerSetAnimeShoot", "PlayerSetAnime with n + 112 while X is in the shooting pose (mega+0x8E)"),
 "0x8003F508": ("PlayerSyncShootAnime", "switch between animation n and n + 112 at the same frame and time"),
}
G = {
 "0x80073C04": ("dash_steps", "3 sub-steps of step 12 (start, run, slide)"),
 "0x80073C10": ("air_dash_steps", "3 sub-steps of step 13"),
 "0x80073C1C": ("ladder_steps", "sub-steps of step 14"),
 "0x8009711C": ("mega.held", "+0x7C buttons held: 1 right, 2 left, 4 up, 8 down, 0x100 dash"),
 "0x80097120": ("mega.pressed", "+0x80 buttons pressed: 0x80 jump, 0x100 dash"),
 "0x80097125": ("mega.dash_timer", "+0x85 dash updates left (30, or 15 with part bit 0x10)"),
 "0x80097129": ("mega.blocked", "+0x89 blocked sides: 1 right, 2 left"),
 "0x8009712C": ("mega.dash_flag", "+0x8C 1 while dashing, -1 after"),
 "0x8009712E": ("mega.shot_pose", "+0x8E shooting pose (animations + 112)"),
 "0x80097131": ("mega.shot_timer", "+0x91 shot pose timer"),
 "0x80097173": ("mega.lock", "+0xD3 event, exit or gate lock: the try functions do nothing"),
 "0x8009718C": ("mega.parts", "+0xEC part bits of the character (game+0x74 + 4 x var)"),
}
for sec, add in (("functions", F), ("globals", G)):
    for k, (n, note) in add.items():
        old = doc[sec].get(k)
        if old and old["name"] not in (n, "BossKillSound", "mega.armor"):
            print("keep existing", k, old["name"]); continue
        doc[sec][k] = {"name": n, "note": note}
    doc[sec] = dict(sorted(doc[sec].items(), key=lambda kv: int(kv[0], 16)))
out = ["{"]
secs = list(doc)
for i, sec in enumerate(secs):
    out.append(f'  "{sec}": {{')
    items = list(doc[sec].items())
    for j, (k, v) in enumerate(items):
        out.append(f'    "{k}": {json.dumps(v, ensure_ascii=False)}' + ("," if j < len(items) - 1 else ""))
    out.append("  }" + ("," if i < len(secs) - 1 else ""))
out.append("}")
open(path, "w").write("\n".join(out) + "\n")
