//Written in 2022 by Adam Klotblixt (adam.klotblixt@gmail.com)
//
//To the extent possible under law, the author have dedicated all
//copyright and related and neighboring rights to this software to the
//public domain worldwide.
//This software is distributed without any warranty.
//
//You should have received a copy of the CC0 Public Domain Dedication
//along with this software. If not, see
//<http://creativecommons.org/publicdomain/zero/1.0/>.

format zx81
;labelusenumeric
;LISTOFF
	// hardware options to be set and change defaults in ZX81DEF.INC
	MEMAVL	   EQU	   MEM_16K	   // can be MEM_1K, MEM_2K, MEM_4K, MEM_8K, MEM_16K, MEM_32K, MEM_48K
	STARTMODE  EQU	   SLOW_MODE	   // SLOW or FAST
	DFILETYPE  EQU	   EXPANDED	   // COLLAPSED or EXPANDED or AUTO
	include '..\..\SINCL-ZX\ZX81.INC'  // definitions of constants
;LISTON
	AUTOLINE 10
	REM _hide _asm

;---------------------------------------------------------------

//Use 262 to get 60Hz, 312 to get 50Hz
TV_STD	EQU	262

//The filename is a zero-terminated zx-string
Video1Name:	dbzx	'cam01.zvj', 0
//Insert the filesize (bytes), which will be calculated to number of frames
Video1Frames	EQU	2767872 / SCREEN_WIDTH_BYTES / SCREEN_HEIGHT_RASTERS

Video2Name:	dbzx	'cam02.zvj', 0
Video2Frames	EQU	4488192 / SCREEN_WIDTH_BYTES / SCREEN_HEIGHT_RASTERS

Video3Name:	dbzx	'cam03.zvj', 0
Video3Frames	EQU	4611072 / SCREEN_WIDTH_BYTES / SCREEN_HEIGHT_RASTERS

Video4Name:	dbzx	'bbb.zvj', 0
Video4Frames	EQU	19496448 / SCREEN_WIDTH_BYTES / SCREEN_HEIGHT_RASTERS

Video5Name:	dbzx	'tron.zvj', 0
Video5Frames	EQU	4116480 / SCREEN_WIDTH_BYTES / SCREEN_HEIGHT_RASTERS

Video6Name:	dbzx	'bad.zvj', 0
Video6Frames	EQU	6732288 / SCREEN_WIDTH_BYTES / SCREEN_HEIGHT_RASTERS

Main:
	call	EnableHires
MainLoop:
	//It is also necessary to change the followin list
	ld	de,Video1Name
	ld	bc,Video1Frames
	call	StreamVideo

	ld	de,Video2Name
	ld	bc,Video2Frames
	call	StreamVideo

	ld	de,Video3Name
	ld	bc,Video3Frames
	call	StreamVideo

	ld	de,Video4Name
	ld	bc,Video4Frames
	call	StreamVideo

	ld	de,Video5Name
	ld	bc,Video5Frames
	call	StreamVideo

	ld	de,Video6Name
	ld	bc,Video6Frames
	call	StreamVideo
	jp	MainLoop

;---------------------------------------------------------------

include 'zvj.inc'
	END _asm

AUTORUN:
	RAND USR #Main

//include D_FILE and needed memory areas
include '..\..\SINCL-ZX\ZX81DISP.INC'

VARS_ADDR:
	db 80h //DO NOT REMOVE!!!

WORKSPACE:

assert ($-MEMST)<MEMAVL
// end of program
