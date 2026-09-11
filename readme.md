# Flipdot driver circuitry

KiCad designs and software for driving Hanover flipdot displays with a resolution of 13 by 28, as used in the [FLIP fluid on Flipdots project](https://mitxela.com/projects/flipflip)

- `drivers-full` are the main driver panels doing 13x8
- `drivers-half` are truncated to 13x4 for the end
- `decode` are boards with a ch32v003 to drive the shift registers
- `decode-sw` is firmware for the ch32, plus python scripts to show test patterns and video via a USB-RS485 adapter
