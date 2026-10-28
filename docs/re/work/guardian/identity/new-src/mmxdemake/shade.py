def grey(values, bits, palette):
    scale = 17 if bits == 4 else 1
    return b"".join(bytes((v * scale, v * scale, v * scale, 255)) for v in values)
