F = 65536
def fx(v): return int(round(v * F))

JUMP, GRAV, GRAV_J8, FALLMAX = fx(5.8125), fx(0.2578125), fx(0.2578125) - 0x1080, fx(-6.5)
KICK = fx(5.625)

def rise(v0, g):
    v, y, n = v0, 0, 0
    while True:
        y += v; n += 1; v -= g
        if v < FALLMAX: v = FALLMAX
        if v <= 0: return y / F, n, v / F

def rise_after_fixed(v0, g, fixed):
    v, y, n = v0, 0, 0
    for _ in range(fixed):
        y += v; n += 1; v -= g
    y_fixed = y
    while v > 0:
        y += v; n += 1; v -= g
    return y_fixed / F, y / F, n

def fall(g, h):
    v, y, n = 0, 0, 0
    while y < h * F:
        y -= v; n += 1; v -= g
        if v < FALLMAX: v = FALLMAX
    return n

def dash(start, minimum, acc, timer):
    s, d, speeds = start, 0, []
    for _ in range(timer):
        d += s; speeds.append(s / F); s += acc
        if s < minimum: s = minimum
    return d / F, speeds

print("jump", rise(JUMP, GRAV))
print("jump jumper part", rise(JUMP, GRAV_J8))
print("kick fixed10, full", rise_after_fixed(KICK, GRAV, 10))
print("kick jumper", rise_after_fixed(KICK, GRAV_J8, 10))
v = 0; n = 0
while v > FALLMAX: v -= GRAV; n += 1
print("updates to fall_max", n)
for h in (16, 32, 64, 68.46):
    print("fall", h, "px in", fall(GRAV, h))
print("air dash", dash(fx(6.5), fx(4.125), fx(-0.1875), 18))
print("air dash part", dash(fx(6.5 + 1.625), fx(4.125 + 1.03125), fx(-0.1875 + 0.046875), 10))
print("ground dash", dash(fx(6.5), fx(4.125), fx(-0.1875), 30)[0])
print("super jump", 11.625 * 13, "px in 13 updates")
