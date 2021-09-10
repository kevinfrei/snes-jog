# SNES-Jog: Making a UGS CNC Jog Controller from an SNES controller

Code and Instructions for using a "SNES Classic mini" gamepad as a CNC Jog
Controller with fabulously free & useful
[Universal GCode Sender](http://winder.github.io/ugs_website/). I learned
everything in the code from trial & error, plus reading through
[this file](https://github.com/mistydemeo/super_nes_classic_edition_oss/blob/master/clvcon-km-6fd0ec177b321206da5bc1b288fd6d7fdf5df7ae/clvcon-km-6fd0ec177b321206da5bc1b288fd6d7fdf5df7ae/clvcon.c).
Good luck! Feel free to open issues. I'm posting this to github before I've
tested it with my CNC, so who knows if it works yet (It looks like it sends the
keystrokes I want)

[Click here for the UGS stuff to verify before each use](#Step-8-Before-each-use-of-UGS)

To see a video of it in (one handed) action,
[click here](https://1drv.ms/v/s!Ap9iqQ7FiN9Qhswu_Yi-vvz0VaJNSQ)

# Incomplete instructions:

**How to make a Jog Controller for use with UGS from an SNES classic
controller**

The SNES Classic controller is a simple I2C peripheral. For folks who don't know
what that is, just trust that this makes it _super_ easy to talk to from just
about any Arduino device.

## Requirements

- Hardware

  - SNES Classic Mini controller (Wired)
    [Amazon link for the controller](https://www.amazon.com/Mario-Retro-SNES-MINI-Controller-Compatible/dp/B0779DS931/ref=sr_1_54_sspa?dchild=1&keywords=SNES&qid=1631203806&s=videogames&sr=1-54-spons&psc=1&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUExN1ZZMDk0UjdEVEc4JmVuY3J5cHRlZElkPUEzU0pFS1VYOVBXMkxBJmVuY3J5cHRlZEFkSWQ9QTA2MTE1NzYxNTBXNTRDMERJQ1lTJndpZGdldE5hbWU9c3BfbXRmJmFjdGlvbj1jbGlja1JlZGlyZWN0JmRvTm90TG9nQ2xpY2s9dHJ1ZQ==),
    [Amazon link for the whole package](https://www.amazon.com/Console-Classic-Built-Controllers-Birthday/dp/B08V4RJQPF/ref=sr_1_2_sspa?dchild=1&keywords=SNES+classic&qid=1631204003&s=videogames&sr=1-2-spons&psc=1&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUEzSUZEMTBaTDBSUFBOJmVuY3J5cHRlZElkPUEwMTczMzg0MllGV1hEMDNGR0Y5TSZlbmNyeXB0ZWRBZElkPUEwMjYwMDk0MjNWWUc2N0ZWOFFENSZ3aWRnZXROYW1lPXNwX2F0ZiZhY3Rpb249Y2xpY2tSZWRpcmVjdCZkb05vdExvZ0NsaWNrPXRydWU=)
  - An Arduino-compatible device that supports both USB keyboard protocol & I2C.
    Some possibilities:

    - [Keebio USB Elite-C ($18)](https://keeb.io/collections/diy-parts/products/elite-c-low-profile-version-usb-c-pro-micro-replacement-atmega32u4)
      - **This is what I'm using: Fully tested and working with my CNC**
    - [Teensy LC ($12)](https://www.pjrc.com/store/teensylc.html) /
      [Teensy 3.2 ($20)](https://www.pjrc.com/store/teensy32.html) /
      [Teensy 4 ($20)](https://www.pjrc.com/store/teensy40.html) ($10-$20)
      - **I tested the software first using a Teensy 3.2**
    - [SparkFun Pro Micro ($8)](https://keeb.io/collections/diy-parts/products/pro-micro-5v-16mhz-arduino-compatible-atmega32u4)
      - This is pretty cheap, but I hear the USB connector is prone to rip off,
        so be careful (That's a key point of upgrade for the Elite-C I used)
    - [SEEED Stuidio's 2 XIAO](https://www.seeedstudio.com/Seeeduino-XIAO-Arduino-Microcontroller-SAMD21-Cortex-M0+-p-4426.html)
      [class of devices](https://www.seeedstudio.com/XIAO-RP2040-v1-0-p-5026.html)
      looks like the smallest & cheapest ($6!) doodad that fits the bill.
      - I've got a couple around, but haven't tried them yet.
    - [Adafruit nRF52840 device ($20-$35)](https://www.adafruit.com/product/4481)
      or the [Adafruit Trinket M0 ($9)](https://www.adafruit.com/product/3500)
      - I've used these for a variety of keyboards, and can confirm that they
        fit the bill, though the Trinket you'll have to solder wires to _pads_
        which can be a PITA...
    - Basically, you can use any Arduino device that supports the Keyboard and
      Wire libraries (They need to have a USB port on them, though that may not
      mean they support behaving as a keyboard)
      - A _lot_ of devices fall under this guise. I have a variety of these
        laying around. I only have a single Arduino-brand device, though,
        because they're _really_ overpriced/underpowered compared to the
        alternatives

  - Optional:
    - A Nintendo Controller adapter.
      [Adafruit makes a simple one for $3](https://www.adafruit.com/product/4836)
      - I didn't bother with my initial build
      - This would enable you to NOT cut the wires for the controller, making
        the end result a bit 'cleaner'

- Software

  - Arduino Studio (available for Mac, PC, and Linux: I've used all 3
    successfully)
  - Depending on the device, you make need separate 'flashing' software.
    - Getting Arduino Studio and firmware flashing up & going is beyond the
      scope of these instructions

- Tools

  - Soldering Iron & Solder
  - Without the adapter, wire cutters
  - Optional
    - Hot glue gun & hot glue

- Skills

  - Soldering
  - Wire stripping
  - Ability to edit UGS key-mappings
  - Digging around on your Arduino-compatible device's pinout diagram to figure
    out which pins are: Power, Ground, SCL, and SDA.
  - Installing Arduino "stuff"
  - Using Arduino to build & flash your device
  - TODO: I could host builds for various devices, to eliminate most of the
    Arduino requirements

- Unnecessary skills
  - Programming: I've taken care of that part for you!

## Steps

### Without the Nintendo Controller Adapter plug

### Step 1 Cut the wires

If you didn't get a Nintendo Controller adapter plug, you'll need to cut the
plug off of your SNES Classic controller & strip the 4 wires so you can solder
them to your controller. The 4 wires in my controller are Red, White, Yellow,
and Green.

![cut-wire](./img/cut-wire.jpg)

### Step 2 Figure out pins

Identify which pins on your Arduino-ish device are

- Power
  - Often referred to as "VCC" in pinout diagrams. Some devices have 3V and 5V
    power available. Both seem to work with the SNES I have: Your Mileage May
    Vary)
- Ground
  - Typically "GND". Frequently there are multiple of them available.
- SCL
  - This is the I2C bus' "Clock" pin. It's probably labeled exactly SCL. It
    might also be labeled SCL0 or I2C_SCL or something.
- SDA
  - This is the I2C bus' "Data" pin. If your device has multiple I2C buses
    available, make sure you're using the same as the SCL pin (SCL0, then SDA0:
    If you're using SCL1, you probably need to make minor edits to the code and
    make sure you're using SDA1)

### Step 3 Strip the wires

Once you've identified which pins are which, strip & solder the wires from your
SNES controller to the Arduino-ish device like this:

- White to GND
- Red to Power
- Yellow to SCL
- Green to SDA

### Step 4 Soldering

**Ready for soldering**

![Start soldering](./img/start-solder.jpg)

**Soldering complete**

![Soldering complete front](./img/done-solder-front.jpg)

![Soldering complete back](./img/done-solder-back.jpg)

I know: I'm _amazing_ as soldering.

### Step 5 Hot glue

To make the thing hold together a little better, I hot glued the underside of
the Arduino-ish device (in the photos, it's a "USB Elite-C" Pro-Micro clone I
had laying around.)

![Before the glue](./img/hot-glue-before.jpg)

![During the glue](./img/hot-glue-during.jpg)

**Hardware is done!**

![Hot glue done](./img/done.jpg)

After you've got those 4 wires soldered, all you've got left is to get the
firmware flashed onto the Arduino device.

### Step 6 Flash the device

**TODO:** Instructions for building the thing in Arduino

Open the .ino file in Arduino. Select your board & port. Upload it.

I debugged the software using a Teensy 3.2, because it was an easier workflow),
then got it working on the Elite-C.

### Step 7 Remap the keys in UGS

To make all of this magic work, in the UGS Platform release you need to set the
keymappings for the actions in the "Machine" category as follows:

| Actions                          | Shortcut             |
| -------------------------------- | -------------------- |
| Jog: Multiply XY Step Size by 10 | Ctrl+Shift+F1        |
| Jog: Divide XY Step Size by 10   | Ctrl+Shift+F2        |
| Jog: Multiply Z Step Size by 10  | Ctrl+Shift+F5        |
| Jog: Divide Z Step Size by 10    | Ctrl+Shift+F6        |
| Jog: Y+                          | Ctrl+Shift+I         |
| Jog: X+                          | Ctrl+Shift+J         |
| Jog: X-                          | Ctrl+Shift+L         |
| Jog: Y-                          | Ctrl+Shift+Comma     |
| Jog: Z+                          | Ctrl+Shift+Y         |
| Jog: Z-                          | Ctrl+Shift+H         |
| Reset X Zero                     | Ctrl+Shift+P         |
| Reset Y Zero                     | Ctrl+Shift+Semicolon |
| Reset Z Zero                     | Ctrl+Shift+Slash     |
| Reset Zero                       | Ctrl+Shift+K         |
| Return to Zero                   | Ctrl+Shift+D         |
| Soft Reset                       | Ctrl+Shift+G         |
| Unlock                           | Ctrl+Shift+S         |
| Home Machine                     | Ctrl+Shift+T         |

I used the 'Ctrl+Shift' modifier because it's the same for any platform. If I'd
tried Alt or Command, things would have been much more complicated when working
across platforms. Trust me on this: I've made too many keyboards and too many
custom keyboard firmwares to address this exact problem.

These aren't working right now. To get diagonal motion working as expected, I'll
need to add input to output lag, because it's pretty darned hard to release them
at the same time. Maybe I should just detect that another button is held down
too and use that to decide to move diagonally. #FeatureRequest

| Actions    | Shortcut          |
| ---------- | ----------------- |
| Jog: X+ Y+ | Ctrl+Shift+U      |
| Jog: X- Y+ | Ctrl+Shift+O      |
| Jog: X+ Y- | Ctrl+Shift+M      |
| Jog: X- Y- | Ctrl+Shift+Period |

**TODO:** Add a screen shot or two or three

### Step 8 **_Before each use of UGS_**

TODO: Fix this. There's a "XY 10" and "Z 1" command: Use them before each
movement to make sure we're not using current values (which may be messed up)

**This is very important, as you _will_ crash your spindle if you're not paying
attention.**

Go to the UGS "Jog Controller" plug-in and make sure that your XY Step is set to
10mm, and the Z step is set to 1mm. The way the code I wrote moves between
small, medium, and large steps is by sending the appropriate number of "Multiply
XY/Z Step by 10" or "Divide XY/Z Step by 10" commands. So, if you want you large
X/Y step to be 100 mm, when you start UGS, set the XY step to 10mm. That will
also make the small X/Y step be 1mm. Same goes for Z: Set the Z step to 1mm,
then when you hold both bumpers for a "large" Z step, you'll get a 10mm shift, 1
bumper for medium gives you a 1mm movement, and a small one gives you 0.1mm.

If you prefer imperial units, I'm sorry that you had such a rough childhood. I
really love math, but for CNC (and lots of other things) I really prefer metric
over imperial. You could switch it to inches and get a 10 inch/1 inch/0.1 inch
XY step and and 1 inch/0.1 inch/0.01 inch Z step. For X/Y, that seems pretty
terrible, honestly, and my CNC has a workpiece size of 25 inches or so. You
could probalby use 1 inch/0.1 inch/0.01 inch for X if you really want...

### Step #9: PROFIT!

And now, it works!

### Final Step: Debugging

I used `carnac` on Windows to make sure the device was sending the right
keystrokes to the device.

## Mapping details

| Operation              | Controller Combo                    |
| ---------------------- | ----------------------------------- |
| Enable/Disable pendant | `Left Bumper` + `Start`             |
| Go Home                | `Both Bumpers` + `Select` + `Start` |
| Soft Reset             | `Right Bumper` + `Select`           |
| Unlock                 | `Right Bumper` + `Start`            |
| Return to zero         | `Left Bumper` + `Select`            |
|                        |                                     |
| Reset Zero All         | `Start` + `Select`                  |
| Reset Zero X           | `Both Bumpers` + `X`                |
| Reset Zero Y           | `Both Bumpers` + `Y`                |
| Reset Zero Z           | `Both Bumpers` + `Start`            |

**Keybindings for X/Y direction:** For X/Y Directions, press the DPad. Hold one
bumper to go the "medium" distance (10x normal). Hold _both_ bumpers to go the
"large" distance (100x normal)

**Keybindings for Z direction:** For the Z direction, use the A/B buttons for
up/down. Same as the DPad, hold one bumper for 'medium' and both for 'large'
distance.

| Direction | Controller |
| --------: | :--------- |
|    Z+ (🡹) | `A`        |
|    Z- (🡻) | `B`        |

## Implementation

![Alt text](./state-diagram.svg)

## Motivation

I've been running a
[CNC Machine](https://millrightcnc.com/product/millright-cnc-power-route-kit-bundle/)
for several years now. Unfortunately, it's kind of clunky to control using a
mouse or keyboard. There are a variety of devices one can purchase called "Jog
Controllers" that give you a dedicated device for moving around the CNC spindle.
The trouble is, they're expensive, and I have no idea if they'd work with my
little hobbyist setup, using the reasonably good UGS G-Code sender/CNC control
software.

# TODO

## Software

- Make the big switch into a declarative state machine of some kind
- Maybe break the code into better pieces to enable support for other
  controllers/protocols?

# Documentation

- Pictures
- General improvement

# Hardware

- Make a little 3D printed case?
