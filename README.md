# Zeddy Video Jukebox

Play video using a ZX81 and a ZXpand classic or plus.<br>

## General info

The stream is black/white in a resolution of 128x96, with about 20 frames per second on both 50Hz and 60Hz TVs. The playback frame rate depends a bit on the SD-card used, some cards can do 25fps, some 16,6fps. The software tries really hard to get the best from the HW used!<br>
The player plays all the files in order and loops. Pressing BREAK jumps to the next file.<br>

## Changing the playlist

The playlist can be changed easily to accomodate your own video files. The project has only 2 files and builds using PokeMons utility ZX-IDE available at http://sinclairzxworld.com/viewtopic.php?f=6&t=1064<br>
The small changes needed are in the zvj.asm file.<br>
You can also choose between 50 and 60Hz in this file.<br>

## Transcoding

The required video files can be created in several ways, as long as the result is a binary file with 16*96 bytes per frame, in sequence. No header or other fancy data :)<br>
Remember that the ZX81 has inverted colours: a 1-bit is black, a 0-bit is white which is the reverse from many other computers.<br>
<br>
I use ffmpeg to do the actual transcoding. The program is open source and available for Windows, Mac and Linux and REALLY good at accepting most video files. If you want to use something else I am sure there are lots of options you can try for yourself.<br>

### Transcoding example 1:

ffmpeg -i caminandes\_01.mkv -an -vf "eq=contrast=1.05" -s 128x96 -sws\_dither ed -pix\_fmt monow -r 20 -f rawvideo cam01.zvj<br>
<br>
-i caminandes01.mkv: this is the input file<br>
-an: ignore audio<br>
-vf "eq=contrast=1.05": a bit more contrast<br>
-s 128x96: the resulting video resolution, DO NOT CHANGE!<br>
-sws\_dither ed: the dither type used. Choose between ed and bayer.<br>
-pix\_fmt monow: invert the colours, DO NOT CHANGE!<br>
-r 20: resulting frame rate. 16.6 or 25 might be better for YOUR system, test it.<br>
-f rawvideo: create a raw video file, DO NOT CHANGE!<br>
cam01.zvj: the output filename.<br>

### Transcoding example 2:

ffmpeg -i BadApple.mkv -an -s 128x96 -sws\_dither bayer -pix\_fmt monow -r 20 -f rawvideo bad.zvj<br>
<br>
Same as example 1, but using bayer dithering and no contrast change.<br>

## Emulators

The very fine emulator Zesarux plays the files at 50 or 60 frames per second, so the emulated SD-card is very fast.<br>
<br>
My trusty go-to emulator EightyOne v1.29 failed to play the files, regardless of the settings I tested.<br>

## History

I started coding this many years ago, but forgot about it when I did another video player for the ZX81 that use redefinable characters and full screen. A friend queried about the old project, so I rediscovered it and just had to do an updated and proper release. This is it.<br>
The old project was released at the time and could play YM-music while displaying 128x94 graphics, but it was a bit hard to get synced correctly...<br>
I have also tried REALLY hard getting more data throughput by doing a FAST-mode player, but I could not get more data per frame. The bottleneck lies in the ZXpand classic.<br>

## Future

It would be possible to make an even faster player for the ZXpand plus, but I wanted to pay tribute to the ZXpand classic with this release. Of course, the ZXpand plus can use this player as well.

## License

The code I created for this project is licensed under Creative Commons CC0 1.0 Universal.<br>

## Contact

Mail: <adam.klotblixt@gmail.com>