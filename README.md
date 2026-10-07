# TovarishSatoshi

TovarishSatoshi is an experimental Bitcoin solo miner for an ESP32 with an
NTSC composite-video output on GPIO25. It uses the Stratum and SHA-256 mining
code from the sibling EasyMiner project.

Two firmware variants are available:

| Variant | PlatformIO environment | Composite signal pin |
| --- | --- | --- |
| Generic ESP32 WROOM development board | `esp32dev` | GPIO25 (usually marked `25`) |
| WEMOS D1 R32 | `wemos_d1_uno32` | IO25 / GPIO25 |

Connect the display's composite video signal to that pin and its ground to an
ESP32 GND pin. Both variants use the ESP32's internal DAC on GPIO25.
At every boot, the video screen shows the configuration AP name and password
for 2.5 seconds before Wi-Fi connection starts.

## Hardware

The documented build uses a WEMOS D1 R32, a black-and-white CRT with composite
video input, and a regulated 12 V DC power supply. See [docs/SETUP.md](docs/SETUP.md)
for wiring, firmware configuration, and first boot steps. GPIO25 carries the
composite video signal; connect it to the CRT's composite input and share ground.

## Configure and use

On first boot, or when stored Wi-Fi credentials no longer connect, the board
starts a `Tovarish_XXXX` access point. It also opens the portal when no Bitcoin
address has been saved. Connect with password `minebitcoin` and open the IP
shown on the composite-video screen (normally `http://192.168.4.1/`). Enter
Wi-Fi credentials, a Bitcoin address, a pool hostname and port, and optionally
a worker name and pool password. The pool hostname must not include
`stratum+tcp://`. Mining settings persist in ESP32 NVS; Wi-Fi credentials are
stored by the WiFiManager library.

The screen shows connection state, current hash rate, total hashes, accepted
and rejected shares, job count, pool difficulty, best difficulty, uptime, pool,
IP address, and firmware version. The default pool is
`solo.homeminingitalia.org:3340`, but a Bitcoin address is required before
mining starts.

## Build and package firmware

The GitHub Actions workflow builds every environment in `platformio.ini` on pushes
to `main`, on tag pushes, and when started manually. It uploads a `dist` artifact.
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

## License and attribution

The project license is MIT; see [LICENSE](LICENSE). Some mining, Stratum, and
board configuration files include code derived from EasyMiner, BitsyMiner, and
NerdMiner V2, with GPLv3 notices retained in the source. Those portions remain
under their applicable GPLv3 terms, and dependencies keep their own licenses.
As a result, the complete combined firmware is not MIT-only.
