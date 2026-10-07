# Hardware and configuration

This guide describes the project setup with a WEMOS D1 R32 (ESP32), a
black-and-white CRT with composite video input, and a 12 V DC supply.

## Parts

- WEMOS D1 R32 board with ESP32-WROOM-32
- CRT television or monitor that accepts analog composite video (RCA)
- Regulated 12 V DC supply suitable for the board and display loads
- Composite lead and hookup wire
- USB cable for firmware upload

Check the markings and documentation for your particular D1 R32 revision.
Many versions accept 7–12 V through the barrel jack ([board reference](https://www.espboards.dev/esp32/d1-uno32/)),
but board clones can vary.
Use 12 V at the board's DC jack only if its rating permits it. Never apply 12 V
to the 5 V, 3V3, or GPIO pins. The CRT needs its own input voltage and current
rating; power it as its manufacturer specifies. Do not open a CRT: it can retain
dangerous high voltage even when unplugged.

## Wiring

1. With power disconnected, connect D1 R32 **GPIO25 / IO25 / D3** to the CRT's
   composite video input (RCA center contact).
2. Connect D1 R32 **GND** to the composite cable shield / CRT video ground.
3. Power the board through its DC barrel jack with the 12 V supply, after
   confirming the jack polarity and input rating for your board revision.
4. Power the CRT using its rated power input. Do not connect the CRT's power
   input to the board's 5 V or 3V3 pins.

GPIO25 is the ESP32 DAC output used by the composite-video library. This project
drives it directly; if the CRT does not show a stable image or the signal level
is unsuitable for the display, use a composite-video interface circuit matched
to the CRT and board rather than connecting the CRT's video output to the ESP32.
The image is NTSC, 256 × 240. A PAL-only display may not lock to this signal.

## Build and flash

Install PlatformIO Core, then from the project root build the D1 R32 target:

```sh
pio run -e wemos_d1_uno32
```

Connect the board over USB and upload (replace the port with the one used by
your computer):

```sh
pio run -e wemos_d1_uno32 -t upload --upload-port /dev/ttyUSB0
```

To view serial diagnostics at 115200 baud:

```sh
pio device monitor -b 115200 --port /dev/ttyUSB0
```

The `esp32dev` environment is for a generic ESP32 development board. The
`wemos_d1_uno32` environment selects the D1 R32 board definition.

## First boot and miner settings

At startup, the CRT briefly displays the configuration access point name and
password. The default password is `minebitcoin`. The access point name begins
with `Tovarish_` and ends with the last bytes of the board's MAC address.

1. Connect a phone or computer to that access point.
2. Open the address shown in the display or serial log; it is normally
   `http://192.168.4.1/`.
3. Enter the local Wi-Fi network name and password, your Bitcoin payout
   address, pool host, and pool port. The default pool is
   `solo.homeminingitalia.org` on port `3340`.
4. Optionally set a worker name and pool password. Enter the pool hostname
   without `stratum+tcp://`.
5. Save the form. The board joins Wi-Fi and begins mining when it has a valid
   payout address and pool settings.

The pool and payout settings are stored in ESP32 NVS. Wi-Fi credentials are
stored by WiFiManager. If Wi-Fi cannot connect, or no payout address has been
saved, the configuration portal starts again. The screen reports connection
state, hash rate, share counts, pool, IP address, uptime, and firmware version.

## Troubleshooting

- **No picture:** Confirm GPIO25-to-video-center and common ground, select the
  CRT's composite input, and confirm that it supports NTSC.
- **Picture rolls or is distorted:** Check the ground connection and cable.
  Confirm the display accepts NTSC composite; this firmware does not select PAL.
- **Portal does not appear:** Watch the display during startup, then check the
  serial monitor at 115200 baud for the AP name and address.
- **Board restarts or USB disconnects:** Check the supply polarity, board input
  rating, wiring, and current capacity. Avoid powering the CRT from the board.
- **Mining does not start:** Reopen the portal and verify the payout address,
  pool hostname, port, and Wi-Fi connection.
