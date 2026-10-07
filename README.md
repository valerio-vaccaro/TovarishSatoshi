# ⭐ TovarishSatoshi 🚩

Workers of the home workshop, welcome! TovarishSatoshi is an experimental
Bitcoin solo miner for the ESP32, presenting its reports on an NTSC television
through GPIO25. It puts the Stratum and SHA-256 mining code of the sibling
EasyMiner project to work for the common cause.

**Ownership decree:** this project belongs to the People’s Collective of
Bedroom Miners. The collective owns the source, the CRT owns the workbench, and
the pool owns the difficulty. Any resemblance to a five-year plan is purely
voluntary and may be delayed by PlatformIO downloads.

Two firmware variants are available:

| Worker unit | PlatformIO environment | Television signal post |
| --- | --- | --- |
| Generic ESP32 WROOM development board | `esp32dev` | GPIO25 (usually marked `25`) |
| WEMOS D1 R32 | `wemos_d1_uno32` | IO25 / GPIO25 |

Connect the display's composite video signal to that pin and its ground to an
ESP32 GND pin. Both units employ the internal DAC on GPIO25. At each boot, the
screen announces the configuration access point and password for 2.5 seconds,
before the march toward the Wi-Fi network begins.

The screen is black and white because the color budget was reassigned to more
important matters: additional hashes and one very serious red flag.

## ⭐ Equipment of the household miner

The documented build uses a WEMOS D1 R32, a black-and-white CRT with composite
video input, and a regulated 12 V DC power supply. Consult the [field manual](docs/SETUP.md)
for wiring, firmware configuration, and first boot orders. GPIO25 carries the
composite video signal; connect it to the CRT's composite input and share ground.

## 🚩 Establish the connection

On the first boot, or when the stored Wi-Fi credentials fail to report for
duty, the board establishes the `Tovarish_XXXX` access point. The portal also
opens if no Bitcoin address has been entered. Join with password `minebitcoin`
and visit the address displayed on the CRT (normally `http://192.168.4.1/`).
Enter Wi-Fi credentials, a Bitcoin address, a pool hostname and port, and, if
desired, a worker name and pool password. Do not include `stratum+tcp://` in
the hostname. Mining settings are kept in ESP32 NVS; Wi-Fi credentials are
kept by WiFiManager.

The password is shared among the comrades, which is another way of saying it
is not a secret. Keep the access point among trusted devices; the Ministry of
Network Security is a joke, but nearby networks are not.

The command screen reports connection state, current hash rate, total hashes,
accepted and rejected shares, job count, pool difficulty, best difficulty,
uptime, pool, IP address, and firmware version. The default pool is
`solo.homeminingitalia.org:3340`, but a Bitcoin address is required before
mining starts.

## ⭐ Build the worker units

The GitHub Actions workshop builds every environment in `platformio.ini` on
pushes to `main`, on tag pushes, and when started manually. It uploads a `dist`
firmware artifact. Pushes to `main` also publish this manual as a GitHub Pages
site; find its address in the repository's Pages settings.
The firmware version is the highest version-sorted tag on the checked-out
commit. If no tag points at that commit, its seven-character commit ID is used.
If Git cannot provide a commit ID, the fallback is `dev`.

To produce the same files locally, run:

```sh
pio run
python3 scripts/package_firmware.py
```

The output contains `dist/assets/tovarishsatoshi/<version>_esp32dev/` and
`dist/assets/tovarishsatoshi/<version>_wemos_d1_uno32/`, each with four flash
binaries, plus `dist/firmwares-tovarishsatoshi.json`. The JSON uses DIYFlasher's
firmware catalog fields and ESP32 flash addresses. Copy the contents of `dist`
to the root of a DIYFlasher site; its `index.js` must also load the new catalog
and provide a TovarishSatoshi picker. The generated JSON alone does not add a
new device to the site's interface.

To flash a connected board directly, use its environment name, for example
`pio run -e wemos_d1_uno32 -t upload --upload-port /dev/ttyUSB1`.

## 🚩 License and credits

The project license is MIT; see [LICENSE](LICENSE). Some mining, Stratum, and
board configuration files include code derived from EasyMiner, BitsyMiner, and
NerdMiner V2, with GPLv3 notices retained in the source. Those portions remain
under their applicable GPLv3 terms, and dependencies keep their own licenses.
As a result, the complete combined firmware is not MIT-only.

The People’s Collective may fork, inspect, and improve the project, subject to
the licenses above. The project treasury currently contains zero Bitcoin and
one (1) aging television.

⭐ **Peace, television, and hashes to all workers! From each according to their
ESP32, to each according to their available GPIO pins.**
