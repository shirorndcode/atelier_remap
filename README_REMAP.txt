== What is this ==
A way to remap the default keyboard layout of Atelier Sophie. I've included
a layout that I think is good, but you can make your own if you so please.

== Install ==
Unzip the following files in your Atelier folder (...\Steam\steamapps\common\Atelier Sophie The Alchemist of the Mysterious Book\)
- dinput8.dll
- remapper.exe
- atelier.remap
If it doesn't work make sure you have the redistributables for
Visual Studio 2015, these are probably the ones you need:
https://www.microsoft.com/en-us/download/details.aspx?id=48145

== Uninstall ==
Remove the 3 files listed in Install.

== Default Layout ==
Movement                   = W, A, S, D (unchanged)
Camera                     = Numpad Arrows (still 4, 8, 6, but not 2)
  Camera Tilt Down         = Numpad Clear (5)
Menu navigation            = Arrows (unchanged)
XBOX A / PS3 X             = E and Enter
XBOX B / PS3 Circle        = Space and Backspace
XBOX Y / PS3 Triangle      = Ctrl
XBOX X / PS3 Square        = Q
LB                         = 1
RB                         = 3

== How to make your own layout ==
You can add or change mappings of the already included layout. Just launch
remapper.exe. If you want to start on a new layout you need to delete
atelier.remap before launching remapper.exe

== Source ==
This project uses a proxy dinput8.dll that simply replaces the incoming
keypress/release before Atelier gets it. Most of the work lies in making that
base proxy dll which I took from
https://github.com/MrColdbird/SW42-SteamGamepadFix

The source can be found here: https://github.com/shirorndcode/atelier_remap
