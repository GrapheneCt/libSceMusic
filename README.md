# libSceMusic

Reimplementation of the internal music player library for PSP2.
libSceMusic is the result of the reverse engineering Sony's static library that is used in system applications and Music application to play audio files via SceShell's MusicCore interface.

## Features:
1. Easy to use.
2. Doesn't use any of the resources allocated to the application.
3. Supports EQ, ALC, different repeat modes, shuffle mode (for DbQuery only).
4. When using MusicPlayerService API, volume can be controlled with "Music" slider from the quick menu.

## Supported codecs:
1. MP3 (.mp3)
2. AAC (.m4a, .aac)
3. ATRAC9 (.at9)
4. WAV (.wav)
