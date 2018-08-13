I'm using an Arduino Uno R3 to drive the LEDs in my Hyperion setup.

The Arduino also controls a relay, to power them on/off at the transformer to save power.
The lights flash 3 times in acknowledgement (green for on and red for off) like car hazard lights do when you set the alarm.

This has been used on an RPi3 and a Vero4k, both running OSMC at the time.

The repo includes:
1. The Arduino sketch
2. The control program I use to send the on/off signals.
3. Perhaps it should include a circuit diagram for the board/relay.  But who has the time?

`lights` usage:
lights 0 (lights off)
lights 1 (light on)
