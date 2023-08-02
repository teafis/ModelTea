#!/usr/bin/python3

import subprocess
from pathlib import Path
import shutil

from collections.abc import Sequence
from typing import Optional


_BASE_DIR = Path(__file__).parent.absolute()


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

def convert_svg_to_png(input_file: Path, output_file: Path, size: int, density: Optional[int] = None):
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


def create_mac_iconset(input_file: Path, icns_file: Path, png_file: Path):
    DEFAULT_DENSITY = 72
    SCALE_FACTORS = [1, 2]
    SIZES = [16, 32, 128, 256, 512]

    iconset_dir = _BASE_DIR / "macos.iconset"
    if iconset_dir.exists():
        shutil.rmtree(iconset_dir)
    iconset_dir.mkdir()

    files = list()

    largest = None
    app_icon_file = None

    for s in SIZES:
        for sf in SCALE_FACTORS:
            scale_result = "@2x" if sf != 1 else ""
            f = iconset_dir / f"icon_{s}x{s}{scale_result}.png"
            convert_svg_to_png(input_file=input_file, output_file=f, density=DEFAULT_DENSITY * sf, size=s * sf)
            files.append(f)

            if largest is None or s * sf > largest[0]:
                largest = (s * sf, f)

            if s * sf == 128 and app_icon_file is None:
                app_icon_file = f

    args = [
        "icnsify",
        "-i",
        largest[1],
        "-o",
        icns_file,
    ]

    p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    _, stderr = p.communicate()

    if p.returncode != 0:
        raise RuntimeError(f"Unable to run icnsify - {stderr.decode('utf-8')}")

    if app_icon_file is None:
        raise RuntimeError("cannot create icon file")
    else:
        shutil.copy2(app_icon_file, png_file)

    shutil.rmtree(iconset_dir)


def create_windows_icon(input_file: Path, output_file: Path):
    iconset_dir = _BASE_DIR / "windows.iconset"
    if iconset_dir.exists():
        shutil.rmtree(iconset_dir)
    iconset_dir.mkdir()

    files = []

    for s in [16, 32, 256]:
        f = iconset_dir / "icon-{s}.png"
        files.append(f)
        convert_svg_to_png(input_file=input_file, output_file=f, size=s)

    run_convert(input_files=files, output_file=output_file)

    with (output_file.parent / f"{output_file.stem}.rc").open("w") as f:
        f.writelines([f"IDI_ICON1               ICON    \"{output_file.name}\""])

    shutil.rmtree(iconset_dir)


def create_default_icon(input_file: Path, output_file: Path):
    convert_svg_to_png(input_file=input_file, output_file=output_file, size=512, density=72)

def main():
    icon_resource_dir = _BASE_DIR.parent / "icons"
    if icon_resource_dir.exists():
        shutil.rmtree(icon_resource_dir)
    icon_resource_dir.mkdir()

    create_mac_iconset(_BASE_DIR / "icon-macos.svg", icon_resource_dir / "macos.icns", icon_resource_dir / "macos.png")
    create_windows_icon(_BASE_DIR / "icon-windows.svg", icon_resource_dir / "windows.ico")
    create_default_icon(_BASE_DIR / "icon-linux.svg", icon_resource_dir / "icon.png")


if __name__ == "__main__":
    main()
