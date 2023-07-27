#!/usr/bin/python3

import subprocess
from pathlib import Path
import shutil

from collections.abc import Sequence
from typing import Optional


_BASE_DIR = Path(__file__).parent.absolute()
_SOURCE_BASE_NAME = "app_window_icon"


def run_convert(input_files: Sequence[str], output_file: str, nargs: Sequence[str] = ()):
    args = [
        "magick",
        "convert",
    ]

    args.extend(nargs)

    args.extend(input_files)
    args.append(output_file)

    p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    _, se = p.communicate()

    if p.returncode != 0:
        raise RuntimeError(f"unable to convert file - {se.decode('utf-8')}")

def convert_file_to_svg(input_file: Path, output_file: Path, size: int, density: Optional[int] = None):
    args = [
        "-background",
        "none",
        "-resize",
        f"{size}x{size}!",
        "-units",
        "PixelsPerInch",
    ]

    if density is not None:
        args.extend(["-density", str(density)])

    run_convert([input_file], output_file, args)


def create_mac_iconset(input_file: Path, output_file: Path, source_dir: Path):
    DEFAULT_DENSITY = 72
    SCALE_FACTORS = [1, 2]
    SIZES = [16, 32, 128, 256, 512]

    iconset_dir = _BASE_DIR / "macos.iconset"
    if iconset_dir.exists():
        shutil.rmtree(iconset_dir)
    iconset_dir.mkdir()

    files = list()

    largest = None
    cpp_write_file = None

    for s in SIZES:
        for sf in SCALE_FACTORS:
            scale_result = "@2x" if sf != 1 else ""
            f = iconset_dir / f"icon_{s}x{s}{scale_result}.png"
            convert_file_to_svg(input_file=input_file, output_file=f, density=DEFAULT_DENSITY * sf, size=s * sf)
            files.append(f)

            if largest is None or s * sf > largest[0]:
                largest = (s * sf, f)

            if s * sf == 128 and cpp_write_file is None:
                cpp_write_file = f

    args = [
        "icnsify",
        "-i",
        largest[1],
        "-o",
        output_file,
    ]

    p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    _, stderr = p.communicate()

    if p.returncode != 0:
        raise RuntimeError(f"Unable to run icnsify - {stderr.decode('utf-8')}")

    if cpp_write_file is None:
        raise RuntimeError("cannot create icon file cpp results")

    write_icon_file(source_dir, "macos", cpp_write_file)

    shutil.rmtree(iconset_dir)


def create_windows_icon(input_file: Path, output_file: Path, source_dir: Path):
    iconset_dir = _BASE_DIR / "windows.iconset"
    if iconset_dir.exists():
        shutil.rmtree(iconset_dir)
    iconset_dir.mkdir()

    files = []

    for s in [16, 32, 256]:
        f = iconset_dir / "icon-{s}.png"
        files.append(f)
        convert_file_to_svg(input_file=input_file, output_file=f, size=s)

    run_convert(input_files=files, output_file=output_file)

    with (output_file.parent / f"{output_file.stem}.rc").open("w") as f:
        f.writelines([f"IDI_ICON1               ICON    \"{output_file.name}\""])

    write_icon_file(source_dir, "windows", files[-1])

    shutil.rmtree(iconset_dir)


def create_default_icon(input_file: Path, output_file: Path, source_dir: Path):
    convert_file_to_svg(input_file=input_file, output_file=output_file, size=128, density=72)
    write_icon_file(source_dir, "default", output_file)

def write_icon_file_header(source_dir: Path):
    header_file = source_dir / f"{_SOURCE_BASE_NAME}.h"

    with header_file.open("w") as f:
        f.write("\n".join([
            "// SPDX-License-Identifier: GPL-3.0-only",
            "",
            "#ifndef TEASIM_APPLICATION_ICON_H",
            "#define TEASIM_APPLICATION_ICON_H",
            "",
            "#include <cstddef>",
            "#include <cstdint>",
            "",
            "namespace tsim_ui",
            "{",
            "extern const size_t icon_png_len;",
            "extern const uint8_t icon_png[];",
            "}",
            "",
            "#endif // TEASIM_APPLICATION_ICON_H",
            "",
        ]))

def write_icon_file(source_dir: Path, icon_type: str, load_file: Path):
    source_file = source_dir / f"{_SOURCE_BASE_NAME}_{icon_type}.cpp"

    png_data = load_file.read_bytes()

    with source_file.open("w") as f:
        f.write("\n".join([
            "// SPDX-License-Identifier: GPL-3.0-only",
            "",
            f"#include \"{_SOURCE_BASE_NAME}.h\"",
            "",
            f"const size_t tsim_ui::icon_png_len = {len(png_data)};",
            "",
            "const uint8_t tsim_ui::icon_png[] = {",
            "",
        ]))

        i = 0
        TAKE = 12
        while i < len(png_data):
            data = png_data[i:(i+TAKE)]
            f.write("    ")
            f.write(", ".join([f"0x{d:02x}" for d in data]))
            f.write(',\n')
            i += TAKE

        f.write("\n".join([
            "};",
            "",
        ]))

def main():
    build_dir = _BASE_DIR / "resources"
    if build_dir.exists():
        shutil.rmtree(build_dir)
    build_dir.mkdir()

    icon_base = _BASE_DIR / "icon.svg"

    source_dir = _BASE_DIR.parent / "src" / "ui"

    create_mac_iconset(_BASE_DIR / "icon-macos.svg", build_dir / "macos.icns", source_dir)
    create_windows_icon(_BASE_DIR / "icon-windows.svg", build_dir / "windows.ico", source_dir)
    create_default_icon(_BASE_DIR / "icon-linux.svg", build_dir / "app_icon.png", source_dir)

    write_icon_file_header(source_dir)


if __name__ == "__main__":
    main()
