"""Embed the same version that the packaging step publishes."""

import sys
from pathlib import Path

Import("env")

sys.path.insert(0, str(Path(env.subst("$PROJECT_DIR")) / "scripts"))
from version import firmware_version

version = firmware_version()
env.Append(CPPDEFINES=[("APP_VERSION", '\\"' + version + '\\"')])
env.Append(CPPDEFINES=[("AUTO_VERSION", '\\"' + version + '\\"')])
print(f"Firmware version: {version}")
