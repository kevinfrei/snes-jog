# Flashing the device

- Software

  - Arduino Studio (available for Mac, PC, and Linux: I've used all 3
    successfully)
  - Depending on the device, you make need separate 'flashing' software.
    - Getting Arduino Studio and firmware flashing up & going is beyond the
      scope of these instructions

- Skills

  - Installing Arduino "stuff"
  - Using Arduino to build & flash your device
  - TODO: I could host builds for various devices, to eliminate most of the
    Arduino requirements

- Unnecessary skills
  - Programming: I've taken care of that part for you!

**TODO:** Flesh this out. A lot. Alternatively, upload .HEX files for flashing manually.

Instructions for building the thing in Arduino

Open the .ino file in Arduino. Select your board & the port (You may need to install drivers to get this working). Upload it to the device.

I debugged the software using a Teensy 3.2, because it was an easier workflow),
then got it working on the Elite-C.
