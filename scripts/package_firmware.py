"""Package PlatformIO builds for DIYFlasher's remote firmware catalog."""

import configparser
import json
import os
from pathlib import Path
import shutil
import sys

from version import firmware_version


ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "dist"
CATALOG = "firmwares-tovarishsatoshi.json"


def main():
    version = firmware_version()
    config = configparser.ConfigParser(interpolation=None)
    config.read(ROOT / "platformio.ini")
    environments = [name for name in config.sections() if name.startswith("env:")]
    if not environments:
        raise RuntimeError("No PlatformIO environments found")

    core = Path(os.environ.get("PLATFORMIO_CORE_DIR", Path.home() / ".platformio"))
    ota_source = core / "packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin"
    records = []
    for section in environments:
        environment = section.removeprefix("env:")
        board = config[section].get("board", environment)
        build = ROOT / ".pio/build" / environment
        destination = OUTPUT / "assets/tovarishsatoshi" / f"{version}_{board}"
        sources = [
            ("0x1000", build / "bootloader.bin", "bootloader.bin"),
            ("0x8000", build / "partitions.bin", "partitions.bin"),
            ("0xE000", ota_source, "boot_app0.bin"),
            ("0x10000", build / "firmware.bin", "firmware.bin"),
        ]
        for _, source, _ in sources:
            if not source.is_file() or source.stat().st_size == 0:
                raise FileNotFoundError(f"Missing or empty build output: {source}")
        destination.mkdir(parents=True, exist_ok=True)
        files = []
        for address, source, name in sources:
            shutil.copyfile(source, destination / name)
            files.append({
                "address": address,
                "url": (destination / name).relative_to(OUTPUT).as_posix(),
                "name": name,
            })
        records.append({
            "value": f"{version}_{board}",
            "label": f"{board} ({version})",
            "firmwareVersion": version,
            "board": board,
            "variants": [],
            "baudrate": 115200,
            "files": files,
        })

    OUTPUT.mkdir(exist_ok=True)
    (OUTPUT / CATALOG).write_text(json.dumps(records, indent=2) + "\n")
    print(f"Packaged {len(records)} board(s) in {OUTPUT}")


if __name__ == "__main__":
    try:
        main()
    except (FileNotFoundError, RuntimeError, ValueError) as error:
        sys.exit(str(error))
