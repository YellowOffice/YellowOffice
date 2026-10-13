#!/usr/bin/env python3
"""Generate YellowDocs icons.

Outputs (all under apps/YellowDocs/resources/):
  - app_icon_{16,32,64,128,256}.png     RGBA app icon
  - document_icon_{16,32,64}.png        RGBA document icon
  - YellowDocs.rdef                     resource file with an embedded
                                        HVIF vector icon ('BEOS:ICON' 'VICN')

No third-party dependencies; PNGs are written with zlib only.
The HVIF blob is round-trip parsed by this script as a sanity check
(self-consistency of the encoder, run before writing the rdef).
"""

import os
import struct
import sys
import zlib

RES_DIR = os.path.join(os.path.dirname(__file__), "..", "apps", "YellowDocs", "resources")

# ---------------------------------------------------------------------------
# Geometry, defined once in a 64x64 coordinate space, shared by the PNG
# rasterizer and the HVIF encoder.
# ---------------------------------------------------------------------------
CANVAS = 64

# Colors: (r, g, b, a)
WHITE = (255, 255, 255, 255)
EDGE = (210, 214, 220, 255)       # page fold / outline accent
FILL = (248, 249, 251, 255)       # page body
LINE = (120, 128, 140, 255)       # text lines
YELLOW = (255, 200, 0, 255)       # Haiku-ish yellow

# Each polygon: (color, [(x, y), ...]) -- non-self-intersecting, implicitly
# closed. Subpaths are filled independently (union).
PAGE = [
    FILL,
    [(10, 6), (42, 6), (54, 18), (54, 58), (10, 58), (10, 6)],
]
FOLD = [
    EDGE,
    [(42, 6), (54, 18), (42, 18), (42, 6)],
]
LINE1 = [LINE, [(16, 26), (48, 26), (48, 29), (16, 29), (16, 26)]]
LINE2 = [LINE, [(16, 34), (48, 34), (48, 37), (16, 37), (16, 34)]]
LINE3 = [LINE, [(16, 42), (38, 42), (38, 45), (16, 45), (16, 42)]]

# Yellow "Y" glyph (two subpaths in one polygon list entry pair)
Y_STROKES = [
    YELLOW,
    # left arm + right arm
    [(22, 20), (32, 34), (42, 20), (36, 20), (32, 28), (28, 20), (22, 20)],
    # stem
    [(30, 32), (34, 32), (34, 50), (30, 50), (30, 32)],
]

APP_SHAPES = [PAGE, FOLD, Y_STROKES]
DOC_SHAPES = [PAGE, FOLD, LINE1, LINE2, LINE3]


# ---------------------------------------------------------------------------
# Minimal RGBA PNG writer (no dependencies beyond zlib/struct)
# ---------------------------------------------------------------------------
def write_png(path, width, height, rgba_rows):
    def chunk(tag, data):
        c = struct.pack(">I", len(data)) + tag + data
        c += struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)
        return c

    raw = b"".join(b"\x00" + bytes(row) for row in rgba_rows)
    ihdr = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    png = (
        b"\x89PNG\r\n\x1a\n"
        + chunk(b"IHDR", ihdr)
        + chunk(b"IDAT", zlib.compress(raw, 9))
        + chunk(b"IEND", b"")
    )
    with open(path, "wb") as f:
        f.write(png)


def rasterize(shapes, size):
    """Scanline-fill the polygons into an RGBA image of size x size."""
    scale = size / CANVAS
    img = [[0, 0, 0, 0] for _ in range(size * size)]

    def fill_polygon(color, pts):
        ys = [p[1] for p in pts]
        y0 = max(0, int(min(ys) * scale))
        y1 = min(size, int(max(ys) * scale) + 1)
        r, g, b, a = color
        for y in range(y0, y1):
            cy = (y + 0.5) / scale
            xs = []
            n = len(pts) - 1 if pts[0] == pts[-1] else len(pts)
            for i in range(n):
                x1, y1p = pts[i]
                x2, y2p = pts[(i + 1) % len(pts)]
                if (y1p <= cy < y2p) or (y2p <= cy < y1p):
                    t = (cy - y1p) / (y2p - y1p)
                    xs.append((x1 + t * (x2 - x1)) * scale)
            xs.sort()
            for i in range(0, len(xs) - 1, 2):
                xa = max(0, int(xs[i] + 0.5))
                xb = min(size, int(xs[i + 1] + 0.5))
                for x in range(xa, xb):
                    img[y * size + x] = [r, g, b, a]

    for shape in shapes:
        color, *subpaths = shape
        for poly in subpaths:
            fill_polygon(color, poly)

    rows = []
    for y in range(size):
        row = []
        for x in range(size):
            row.extend(img[y * size + x])
        rows.append(row)
    return rows


# ---------------------------------------------------------------------------
# HVIF (Haiku Vector Icon Format) encoder + self-check parser
# ---------------------------------------------------------------------------
def _f32(v):
    return struct.pack(">f", float(v))


def parse_hvif_self_check(data, expected_shapes):
    """Parse an HVIF blob written by encode_hvif and verify basic structure."""
    if data[:4] != b"ncif":
        raise ValueError("bad magic")
    i = 4
    sections = {}
    while i < len(data):
        marker = data[i]
        i += 1
        count = data[i]
        i += 1
        items = []
        if marker == ord("C"):
            for _ in range(count):
                flags = data[i]
                i += 1
                if flags & 0x40:
                    i += 4
                    items.append("cmyk")
                elif flags & 0x02:
                    # XRGB / XRGBA: 4 bytes, alpha first (0xff when opaque)
                    a, r, g, b = data[i:i + 4]
                    i += 4
                    items.append((r, g, b, a))
                else:
                    i += 1  # grayscale
                    items.append("gray")
        elif marker == ord("S"):
            for _ in range(count):
                flags = data[i]
                i += 1
                if flags & 0x07:
                    i += 16
                    n = data[i]
                    i += 1 + n
                    items.append("gradient")
                else:
                    items.append(("solid", data[i]))
                    i += 1
        elif marker == ord("P"):
            for _ in range(count):
                ncmd = data[i]
                npt = data[i + 1]
                i += 2
                cmds = data[i:i + ncmd]
                i += ncmd
                pts = []
                for _ in range(npt):
                    flags = data[i]
                    i += 1
                    x, y = struct.unpack(">ff", data[i:i + 8])
                    i += 8
                    pts.append((flags, x, y))
                items.append((ncmd, npt, cmds, pts))
        elif marker == ord("H"):
            for _ in range(count):
                style = data[i]
                pc = data[i + 1]
                i += 2
                has_transform = bool(pc & 0x40)
                hinting = bool(pc & 0x80)
                npaths = pc & 0x3F
                if has_transform:
                    i += 24
                pids = list(data[i:i + npaths])
                i += npaths
                items.append((style, npaths, pids, has_transform, hinting))
        else:
            raise ValueError("unknown section 0x%02x" % marker)
        sections[chr(marker)] = items

    for req in ("C", "S", "P", "H"):
        if req not in sections:
            raise ValueError("missing section %s" % req)
    if len(sections["H"]) != len(expected_shapes):
        raise ValueError("shape count mismatch")
    for style, npaths, pids, *_ in sections["H"]:
        if style >= len(sections["S"]):
            raise ValueError("style index out of range")
        for pid in pids:
            if pid >= len(sections["P"]):
                raise ValueError("path index out of range")
    return sections


# ---------------------------------------------------------------------------
# HVIF icon build: one style + one color per distinct shape color
# ---------------------------------------------------------------------------
def build_hvif_icon(shapes):
    """shapes: list of PAGE-like entries [(color, poly, ...), ...]"""
    style_for_color = {}
    rebuilt = []
    for shape in shapes:
        color, *subpaths = shape
        if color not in style_for_color:
            style_for_color[color] = len(style_for_color)
        rebuilt.append((style_for_color[color], subpaths, color))

    # encode with styles deduped
    out = bytearray(b"ncif")
    colors = list(style_for_color.keys())
    out += b"C" + bytes([len(colors)])
    for r, g, b, a in colors:
        out += bytes([0x03, a, r, g, b])
    out += b"S" + bytes([len(colors)])
    for i in range(len(colors)):
        out += bytes([0x00, i])

    paths = []
    for _, subpaths, _ in rebuilt:
        for poly in subpaths:
            pts = poly[:-1] if poly[0] == poly[-1] else poly
            paths.append(pts)
    out += b"P" + bytes([len(paths)])
    for pts in paths:
        cmds = [0] + [1] * len(pts)
        points = list(pts) + [pts[0]]
        out += bytes([len(cmds), len(points)])
        out += bytes(cmds)
        for x, y in points:
            out += bytes([0])
            out += _f32(x)
            out += _f32(y)

    out += b"H" + bytes([len(rebuilt)])
    pid = 0
    for style_index, subpaths, _ in rebuilt:
        n = len(subpaths)
        out += bytes([style_index, n])
        out += bytes(range(pid, pid + n))
        pid += n
    return bytes(out)


# ---------------------------------------------------------------------------
# rdef generation
# ---------------------------------------------------------------------------
def hex_lines(data, indent="        ", per_line=12):
    lines = []
    for i in range(0, len(data), per_line):
        row = ", ".join("0x%02x" % b for b in data[i:i + per_line])
        lines.append(indent + row + ",")
    return "\n".join(lines)


RDEF_TEMPLATE = """\
/*
 * YellowDocs resource definition.
 *
 * Generated by scripts/generate-icons.py -- do not edit the icon data by
 * hand; re-run the script instead. Build with:
 *   rc -o YellowDocs.rsrc YellowDocs.rdef
 *   xres -o YellowDocs YellowDocs YellowDocs.rsrc
 */

resource app_signature "application/x-vnd.yellowdocs";

resource app_version {
    major = 0,
    middle = 1,
    minor = 0,
    variety = B_APPV_FINAL,
    internal = 0,
    short_info = "YellowDocs",
    long_info = "YellowOffice word processor and document editor"
};

/* B_SINGLE_LAUNCH: only one instance runs at a time. */
resource app_flags B_SINGLE_LAUNCH;

/* File types handled by YellowDocs. */
resource BEOS:FILE_TYPES {
    {
        type = "application/x-vnd.yellowdocs.ydf",
        name = "Yellow Document Format",
        app = "application/x-vnd.yellowdocs",
        short_description = "YellowDocs document",
        long_description = "YellowOffice native document format",
        preferred_app = "application/x-vnd.yellowdocs",
        extensions = { "ydf" }
    },
    {
        type = "text/x-markdown",
        name = "Markdown",
        app = "application/x-vnd.yellowdocs",
        short_description = "Markdown document",
        long_description = "Markdown text document",
        preferred_app = "application/x-vnd.yellowdocs",
        extensions = { "md", "markdown" }
    },
    {
        type = "application/vnd.oasis.opendocument.text",
        name = "OpenDocument Text",
        app = "application/x-vnd.yellowdocs",
        short_description = "ODF text document",
        long_description = "OpenDocument Format text document",
        preferred_app = "application/x-vnd.yellowdocs",
        extensions = { "odt" }
    },
    {
        type = "text/plain",
        name = "Plain Text",
        app = "application/x-vnd.yellowdocs",
        short_description = "Text document",
        long_description = "Plain text document",
        preferred_app = "application/x-vnd.yellowdocs",
        extensions = { "txt" }
    }
};

/* Application icon (HVIF vector icon, 64x64 native). */
resource("BEOS:ICON") 'VICN' {
    data = {
__HVIF_DATA__
    };
};
"""


def main():
    os.makedirs(RES_DIR, exist_ok=True)

    for size in (16, 32, 64, 128, 256):
        path = os.path.join(RES_DIR, "app_icon_%d.png" % size)
        write_png(path, size, size, rasterize(APP_SHAPES, size))
        print("wrote", path)

    for size in (16, 32, 64):
        path = os.path.join(RES_DIR, "document_icon_%d.png" % size)
        write_png(path, size, size, rasterize(DOC_SHAPES, size))
        print("wrote", path)

    hvif = build_hvif_icon(APP_SHAPES)
    sections = parse_hvif_self_check(hvif, APP_SHAPES)
    print("HVIF self-check ok: %d colors, %d styles, %d paths, %d shapes"
          % (len(sections["C"]), len(sections["S"]), len(sections["P"]),
             len(sections["H"])))

    rdef = RDEF_TEMPLATE.replace("__HVIF_DATA__", hex_lines(hvif))
    rdef_path = os.path.join(RES_DIR, "YellowDocs.rdef")
    with open(rdef_path, "w") as f:
        f.write(rdef)
    print("wrote", rdef_path, "(%d HVIF bytes)" % len(hvif))


if __name__ == "__main__":
    sys.exit(main())
