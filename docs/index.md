---
title: TovarishSatoshi Field Manual
---

# ⭐ TovarishSatoshi 🚩

**Technical documentation for the experimental ESP32 Bitcoin solo miner.**

Comrades, welcome to the workshop archive. The station joins a Stratum V1 pool
over Wi-Fi, computes SHA-256 on an ESP32, and reports its condition on an NTSC
television. This archive gives the assembly and operation orders, together with
the firmware supply plan.

> **Ownership decree:** the project is the common property of the People’s
> Collective of Bedroom Miners. The people own the repository; the pool owns
> the difficulty; the CRT owns the best seat in the workshop.

## Field manuals

- [Complete field manual](SETUP.md): specifications, electrical safety,
  wiring, builds, first boot, pool configuration, screen reports, releases,
  troubleshooting, and license notes.

The repository's `README.md` provides a short project overview and quick links
for developers building the firmware.

## Quick facts from headquarters

| Item | Specification |
| --- | --- |
| Standard workshop board | WEMOS D1 R32 (ESP32) |
| Alternate board | Generic ESP32 WROOM development board |
| Video output | NTSC composite through GPIO25 |
| Network and mining | Wi-Fi and Stratum V1 |
| Default pool | `solo.homeminingitalia.org:3340` |
| Firmware build | PlatformIO |

In keeping with the finest traditions of Soviet planning, the release date is
known in advance, the compile time is not, and the final hash rate will be
reported by a television older than the miner.

The published manual is built from this `docs/` directory by GitHub Actions on
pushes to `main`. The repository must have GitHub Pages configured to use
**GitHub Actions**. Find the public address under the repository's Pages
settings after deployment.

⭐ **Knowledge shared is progress achieved.** 🚩 The collective is thanked for
its patience while the microcontroller completes its five-year plan, one nonce
at a time.
