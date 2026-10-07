# ⭐ Field Manual of the TovarishSatoshi Mining Station 🚩

Comrades of the home workshop, this manual records the assembly, commissioning,
and service of the TovarishSatoshi experimental Bitcoin solo miner. Proceed in
order: inspect the apparatus, build the firmware, connect the display, then
enlist the station on the local network and mining pool.

> **Property of the People’s Collective of Bedroom Miners.** The people own
> this project, the pool sets the difficulty, and the CRT has seniority. No
> committee can make GPIO25 appear on the wrong pin any faster; consult the
> board markings.

> **Purpose of the machine:** an ESP32 computes SHA-256 hashes and communicates
> with a Stratum V1 mining pool. A black-and-white NTSC television serves as
> the station's status board. This is an experimental educational project; a
> household ESP32 has a very small chance of finding a Bitcoin block.

## Contents of the manual

- [Station specifications](#station-specifications)
- [Equipment ledger and electrical orders](#equipment-ledger-and-electrical-orders)
- [Build, upload, and observe](#build-upload-and-observe)
- [Commission the station](#commission-the-station)
- [Read the status board](#read-the-status-board)
- [Firmware packages and documentation publication](#firmware-packages-and-documentation-publication)
- [Troubleshooting](#troubleshooting)
- [Care, privacy, and known limits](#care-privacy-and-known-limits)
- [Licenses and credits](#licenses-and-credits)

## Station specifications

| Department | Specification |
| --- | --- |
| Processor | ESP32, Arduino framework via PlatformIO |
| Supported boards | Generic ESP32 WROOM (`esp32dev`); WEMOS D1 R32 (`wemos_d1_uno32`) |
| Television output | NTSC composite, 256 × 240, ESP32 DAC on GPIO25 |
| Pool protocol | Stratum V1 over TCP |
| Network | Wi-Fi; captive configuration portal on first setup or when credentials fail |
| Serial reports | 115200 baud |
| Default pool | `solo.homeminingitalia.org:3340` |

The documented workshop assembly uses a WEMOS D1 R32. Board clones and
revisions may differ; identify the exact board before supplying power.
Unlike a five-year plan, this table describes targets that have actually been
tested in the project configuration. Your board clone may still introduce its
own glorious regional variation.

## Equipment ledger and electrical orders

Gather the following before assembly:

- WEMOS D1 R32 with ESP32-WROOM-32, or another supported ESP32 WROOM board
- CRT television or monitor with analog composite video input
- Regulated supply compatible with the board and its input jack
- Composite video lead and hookup wire
- USB cable for firmware upload and serial diagnostics
- Wi-Fi network, Bitcoin payout address, and Stratum pool details

### Wiring orders

1. Disconnect power from the board and display.
2. Connect board **GPIO25 / IO25** to the CRT's composite video input (RCA
   center contact).
3. Connect board **GND** to the composite cable shield and CRT video ground.
4. Supply the board through its rated input. Many D1 R32 revisions accept
   7–12 V at the barrel jack, but clones vary. Confirm the rating and polarity
   printed on your own board before using 12 V.
5. Power the CRT using its own rated input and supply. Never feed CRT power
   through the ESP32's 5 V, 3V3, or GPIO pins.

The ESP32 DAC drives the composite output directly. If the picture is unstable
or the signal level does not suit the display, use a suitable composite-video
interface circuit. Connect the ESP32 output to a display input only; never
connect a CRT video output to GPIO25. The firmware produces NTSC. A PAL-only
display may not lock to the signal.

**High-voltage order:** do not open a CRT. Dangerous voltage can remain inside
even after the set has been unplugged. Follow the display maker's instructions.

## Build, upload, and observe

Install PlatformIO Core, open a terminal in the repository root, and build the
D1 R32 station:

```sh
pio run -e wemos_d1_uno32
```

For a generic ESP32 WROOM development board, use its separate party line:

```sh
pio run -e esp32dev
```

Connect the board over USB. Replace `/dev/ttyUSB0` with the serial port assigned
by your system, then upload:

```sh
pio run -e wemos_d1_uno32 -t upload --upload-port /dev/ttyUSB0
```

Serial diagnostics are available at 115200 baud:

```sh
pio device monitor -b 115200 --port /dev/ttyUSB0
```

At startup, the CRT briefly displays the access point name and password. The
board then attempts to join Wi-Fi if valid credentials and a payout address
have already been saved. A fresh station opens its configuration portal.
The opening password is displayed to the room, a system of remarkable
transparency and questionable secrecy worthy of a very small ministry.

## Commission the station

### Join the configuration network

The access point is named `Tovarish_XXXX`, where the final characters come
from the board's MAC address. Its password is `minebitcoin`. Connect a phone or
computer to this network. Open the address shown on the CRT or in the serial
log; it is normally `http://192.168.4.1/`.

The portal opens when the station has no saved payout address or cannot join
the saved Wi-Fi network. The configuration access point is open to nearby
devices that know its shared password, so configure the machine on a network
you control.

### Fill in the miner's form

| Field | What to enter |
| --- | --- |
| Wi-Fi SSID and password | Credentials for the local wireless network |
| Bitcoin address | Address where the pool should direct any payout |
| Worker name | Optional identifier; blank uses the firmware's default miner name |
| Pool host | Hostname only, without `stratum+tcp://` |
| Pool port | Integer from 1 to 65535; default is `3340` |
| Pool password | Optional pool-specific value; default is `x` |

The default pool host is `solo.homeminingitalia.org`. A valid payout address,
pool host, and port are required to save mining parameters. Check the address
carefully before commissioning: this project does not custody or recover
Bitcoin.

Save the form. Mining settings are stored in ESP32 NVS; Wi-Fi credentials are
stored by WiFiManager. If the Wi-Fi connection fails or the address is missing,
the portal starts again. The device does not provide a separate browser-based
dashboard after configuration; its principal status display is the CRT.

## Read the status board

The television screen reports the station's network and mining state. Depending
on the current screen, it displays:

- configuration access point and password during startup
- connection state and local IP address
- current hash rate and total hashes
- accepted and rejected shares
- jobs received from the pool
- current pool difficulty and best difficulty observed
- uptime, pool, and firmware version

Pool difficulty sets the share target. A submitted share may be accepted or
rejected by the pool; ordinary hashing activity does not mean that a Bitcoin
block has been found. The machine's small hash rate makes block discovery
extremely unlikely.

The dashboard does not exaggerate the achievements of the collective. If the
hash rate is modest, the arithmetic is simply practicing socialist realism:
it depicts the situation as it is, only with fewer pixels.

## Firmware packages and documentation publication

The repository's GitHub Actions workflow builds all PlatformIO environments on
pushes to `main`, tag pushes, and manual runs. It packages the binaries and
uploads the `dist/` directory as the `tovarishsatoshi-firmware` workflow
artifact. The firmware version is the highest version-sorted tag available to
the checked-out commit; absent a matching tag it uses the seven-character
commit ID, or `dev` if Git cannot provide one.

To make the same package locally, build all boards and run the packager:

```sh
pio run
python3 scripts/package_firmware.py
```

For every PlatformIO board environment, the package includes four flash images
at addresses `0x1000`, `0x8000`, `0xE000`, and `0x10000`. The catalog is
`dist/firmwares-tovarishsatoshi.json`; board-specific images live below
`dist/assets/tovarishsatoshi/`. The catalog follows DIYFlasher's firmware
catalog fields. Copying these files to a DIYFlasher site does not add the
device to that site's interface: its `index.js` must load the catalog and
provide a TovarishSatoshi board picker.

Pushes to `main` also build and publish the Markdown in `docs/` to GitHub Pages.
The repository must have Pages configured to use **GitHub Actions**. Once the
first deployment succeeds, the published address appears in the repository's
Pages settings and in the workflow deployment summary.
The firmware factory is automated, but the People's Committee still recommends
reading the build log: even a planned economy can produce a failed dependency
download.

## Troubleshooting

| Symptom | Inspection order |
| --- | --- |
| No picture | Select the CRT's composite input; check GPIO25 to RCA center and shared ground; confirm NTSC support. |
| Picture rolls or looks distorted | Inspect the composite lead and ground; confirm the display accepts NTSC. This firmware does not select PAL. |
| Portal is not visible | Watch the CRT during startup; scan for the `Tovarish_XXXX` network; inspect serial output at 115200 baud. |
| Board restarts or USB disconnects | Check supply polarity, input rating, wiring, and current capacity. Keep CRT power separate from board GPIO and logic rails. |
| Wi-Fi does not connect | Re-enter the SSID and password through the portal; verify that the network is available at the miner's location. |
| Mining does not start | Confirm a payout address, pool hostname without scheme prefix, valid port, Wi-Fi connection, and pool availability. Read serial diagnostics. |
| Shares are rejected | Check pool hostname, port, payout address, worker and password requirements with the pool operator; inspect connection and pool logs. |

## Care, privacy, and known limits

- Keep the board and exposed connections away from conductive debris and
  moisture; disconnect power before changing wiring.
- The setup access point uses the fixed password `minebitcoin`. Use it only in
  a controlled environment and do not treat it as a private long-term network.
- Wi-Fi and mining parameters are stored on the board. Erasing or replacing
  firmware may require entering them again.
- The project is experimental firmware for supported ESP32 boards. Performance,
  display compatibility, pool behavior, and board clones may vary.
- Solo mining with an ESP32 is an educational experiment, not a dependable
  source of income or a practical way to obtain Bitcoin.

For clarity, the collective's economic forecast is: **one miner, many hashes,
and probably no block**. In the event of a miracle, please do not attribute it
to the Ministry of Luck; it was the silicon doing its best.

## Licenses and credits

The repository includes an MIT license. Some mining, Stratum, and board
configuration code derives from EasyMiner, BitsyMiner, and NerdMiner V2 and
retains GPLv3 notices. Those portions remain subject to their applicable
GPLv3 terms; dependencies retain their own licenses. Therefore the combined
firmware is not MIT-only. Consult the notices in the source and the repository
license files before redistributing a build.

⭐ **Knowledge shared is progress achieved. Peace, television, and hashes to all workers!** 🚩

*This manual was approved unanimously by the People's Collective, including the
one member who was asleep beside the CRT.*
