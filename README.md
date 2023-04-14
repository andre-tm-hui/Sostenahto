# Sustenato Pedal
<p align="center"><image src="rc/sustenato_pedal.png" height="500px"></image></p>
     
A VST Plugin that provides infinite piano-like sustain, using transient detection, autocorrelation, a phase vocoder and more.

## Features:
- Infinite sustain - let your notes and chords ring out for as long as you like.
- Fade In/Out - adjust the speed at which the sustain fades in/out.
- Wet/Dry - adjust your mix.
- Layering - select however many layers you want to stack sustained notes and chords.
- Compatible with any Keyboard-emulating foot-switch/pedal - control the sustain with your feet, as you would an FX pedal.
- ***INCONSISTENT*** Works with trills and vibrato
- Force Period - Uses a phase vocoder to stretch recorded samples to the set period. Wait less time before pressing the pedal.
  - Not compatible with trills and vibrato

### Recommended Usage:
- Best used as a live tool, whether for a live performance or recording
- Having a foot-switch/pedal is not necessary, but highly recommended (as you use your hands to play the instrument)
- Not tested as a post-processing tool
- Easiest to set it up in a VST Host

### Usage Instructions:
- Place the VST plugin anywhere in your plugin chain.
- Open the plugin's editor window and ensure it is in focus whenever you want to use it
  - If the window is not in focus, it will not receive keyboard events, hence you won't be able to toggle the sustainer using a keyboard key/foot-switch/pedal
- Upon playing any note/chord, press your corresponding sustain keybind
  - If you press it too quickly after playing, the sustained sound may not be clear/smooth. This can be used intentionally to create different effects, or can be reset quickly by re-triggering the keybind, given no new notes are played.

Designed for use with Electric Guitars and Amp Sims.

## Compatibility:
Works with Windows. Not tested, but it should work on Linux/Mac as well.
### Working:
- Cantabile 4
### Not Working:
- Waveform DAW - plugin not verified
