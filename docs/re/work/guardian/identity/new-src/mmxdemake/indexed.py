from mmxdemake.psxcolor import to_rgba


def unpack(data, bpp):
    if bpp == 8:
        return list(data)
    if bpp == 4:
        return [v for b in data for v in (b & 15, b >> 4)]
    raise ValueError(f"bpp {bpp} not supported (4 or 8)")


def dimensions(data_len, bpp, width):
    pixels = data_len * 8 // bpp
    if width <= 0 or pixels % width:
        raise ValueError(f"width {width} does not divide {pixels} pixels")
    return width, pixels // width


def to_rgba_rows(indices, clut, width, clut_offset=0):
    rows = []
    for start in range(0, len(indices), width):
        row = bytearray()
        for i in indices[start:start + width]:
            k = clut_offset + i
            if k >= len(clut):
                raise ValueError(f"index {k} is past the palette of {len(clut)} colours")
            row += bytes(to_rgba(clut[k]))
        rows.append(bytes(row))
    return rows
