# Don'r use this code

use https://github.com/debauchee/barrier instead

# portaclip
A portable clipboard sharing utility for Linux and Windows

I am using [Synergy](https://symless.com/synergy) for keyboard and mouse sharing between my Linux and Windows computers. This also provided me with clipboard sharing. However, after upgrading to Kubuntu 17.04 I could no longer get clipboard sharing to work with Synergy.

As a temporary fix, portaclip was created. Portaclip is a small dialog based application that observes the mouse location (inside/outside local computer screen) and sends the clipboard text as an UDP message in UTF8 format to the other computer when the mouse leaves the local screen. On the other computer, portaclip receives the UDP message and puts the UTF8 text in the local clipboard.

Portaclip is limited to text only, no images or other clipboard formats.

When starting up for the first time, enter the IP address of the machine you are sending to. By default, the mouse based auto copy is enabled, but if you don't run Synergy, you can disable the auto-copy and press the "Force Copy" button instead.

Linux | Windows
------------ | -------------
![portaclip_linux](/images/portaclip_linux.png)| ![portaclip_windows](/images/portaclip_windows.png)

# Compiled binaries
For compiled binaries, check releases at https://github.com/arnholm/portaclip/releases
