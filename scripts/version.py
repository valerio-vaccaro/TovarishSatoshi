"""Select the firmware version from HEAD's tag or abbreviated commit ID."""

import re
import subprocess


def firmware_version():
    result = subprocess.run(
        ["git", "tag", "--points-at", "HEAD", "--sort=-version:refname"],
        capture_output=True,
        text=True,
        check=False,
    )
    if result.returncode == 0 and result.stdout.strip():
        version = result.stdout.splitlines()[0]
    else:
        commit = subprocess.run(
            ["git", "rev-parse", "--short=7", "HEAD"],
            capture_output=True,
            text=True,
            check=False,
        )
        version = commit.stdout.strip() if commit.returncode == 0 else "dev"
    if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9._-]*", version):
        raise ValueError(f"Tag {version!r} cannot be used as a firmware folder name")
    return version
