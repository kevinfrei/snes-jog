# Configuring the keys in UGS

To make all of this magic work you need to set up particular keystrokes in the
UGS Platform release. They are all actions in the "Machine" category. Here they
are, in a nice table:

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
| XY 10                            | Ctrl+Shift+PgUp      |
| Z 1                              | Ctrl+Shift+PgDn      |

I used the 'Ctrl+Shift' modifier because it's the same for any platform. If I'd
used Alt or Command, things would have gotten much more complicated when working
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

# Configuration **_before each use of UGS_**

Update: There's a "XY 10" and "Z 1" command: I now use them before each
movement to make sure we're not using current values (which may be messed up)
so you don't have to worry about this anymore!

> **This was very important, as you _would_ crash your spindle if you're not paying
attention.** But I fixed that :D

> Go to the UGS "Jog Controller" plug-in and make sure that your XY Step is set to
10mm, and the Z step is set to 1mm. The way the code I wrote moves between
small, medium, and large steps is by sending the appropriate number of "Multiply
XY/Z Step by 10" or "Divide XY/Z Step by 10" commands. So, if you want you large
X/Y step to be 100 mm, when you start UGS, set the XY step to 10mm. That will
also make the small X/Y step be 1mm. Same goes for Z: Set the Z step to 1mm,
then when you hold both bumpers for a "large" Z step, you'll get a 10mm shift, 1
bumper for medium gives you a 1mm movement, and a small one gives you 0.1mm.

> If you prefer imperial units, I'm sorry that you had such a rough childhood. I
really love math, but for CNC (and lots of other things) I really prefer metric
over imperial. You could switch it to inches and get a 10 inch/1 inch/0.1 inch
XY step and and 1 inch/0.1 inch/0.01 inch Z step. For X/Y, that seems pretty
terrible, honestly, and my CNC has a workpiece size of 25 inches or so. You
could probalby use 1 inch/0.1 inch/0.01 inch for X if you really want...
