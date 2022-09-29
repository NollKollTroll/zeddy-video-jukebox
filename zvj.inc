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

SCREEN_WIDTH_PIXELS	=	128
SCREEN_WIDTH_BYTES	=	SCREEN_WIDTH_PIXELS / 8
SCREEN_HEIGHT_RASTERS	=	96
VSYNC_RASTERS		=	3
WASTED_RASTERS		=	4
TOTAL_BLANK_RASTERS	=	TV_STD - 3 - WASTED_RASTERS - SCREEN_HEIGHT_RASTERS - VSYNC_RASTERS
TOP_BLANK_RASTERS	=	TOTAL_BLANK_RASTERS / 2
BOTTOM_BLANK_RASTERS	=	TOTAL_BLANK_RASTERS - TOP_BLANK_RASTERS

FrameCounter	db	0
BreakStatus	db	0
VideoFrames	dw	0

PRT0	EQU	$0007
PRT1	EQU	$2007
PRT2	EQU	$4007
PRT3	EQU	$6007
PRT4	EQU	$8007
PRT5	EQU	$a007
PRT6	EQU	$c007
PRT7	EQU	$e007

//de = file path, zero-terminated zx-string
//bc = nr of frames to play
StreamVideo:
	//save frame counter
	ld	(VideoFrames),bc
	//prepare buffer for write
	ld	bc,PRT0
	ld	a,$ff
	out	(c),a
	//send file-name characters to buffer, zero-terminated
	ld	bc,PRT2
	.NameOutLoop:
		ld	a,(de)
		out	(c),a
		inc	de
		and	a
	jp	nz,.NameOutLoop
	//open file for read
	ld	bc,PRT4
	xor	a
	out	(c),a
	call	GetResponse
BigLoop:
	//set red led
	//ld      bc,PRT7
	//ld      a,$b8
	//out     (c),a
	//read to frame buffer 0
	ld	hl,SCREEN_0 + 256 * 0
	call	RequestAndRead256BytesFromFileToHL
	ld	hl,SCREEN_0 + 256 * 1
	call	RequestAndRead256BytesFromFileToHL
	//clear red led
	//ld      bc,PRT7
	//ld      a,$b9
	//out     (c),a
	ld	hl,SCREEN_0 + 256 * 2
	call	RequestAndRead256BytesFromFileToHL
	ld	hl,SCREEN_0 + 256 * 3
	call	RequestAndRead256BytesFromFileToHL
	ld	hl,SCREEN_0 + 256 * 4
	call	RequestAndRead256BytesFromFileToHL
	ld	hl,SCREEN_0 + 256 * 5
	call	RequestAndRead256BytesFromFileToHL

	//change screen pointer
	ld	hl,SCREEN_0
	ld	(ScreenAddress),hl

	//test for break
	ld	a,(BreakStatus)
	and	1
	jp	z,CloseFile

	//check if end of file
	ld	bc,(VideoFrames)
	dec	bc
	ld	(VideoFrames),bc
	ld	a,b
	or	c
	jp	z,CloseFile

	//read to frame buffer 1
	ld	hl,SCREEN_1 + 256 * 0
	call	RequestAndRead256BytesFromFileToHL
	ld	hl,SCREEN_1 + 256 * 1
	call	RequestAndRead256BytesFromFileToHL
	ld	hl,SCREEN_1 + 256 * 2
	call	RequestAndRead256BytesFromFileToHL
	ld	hl,SCREEN_1 + 256 * 3
	call	RequestAndRead256BytesFromFileToHL
	ld	hl,SCREEN_1 + 256 * 4
	call	RequestAndRead256BytesFromFileToHL
	ld	hl,SCREEN_1 + 256 * 5
	call	RequestAndRead256BytesFromFileToHL

	//change screen pointer
	ld	hl,SCREEN_1
	ld	(ScreenAddress),hl

	//test for break
	ld	a,(BreakStatus)
	and	1
	jp	z,CloseFile

	//check if end of file
	ld	bc,(VideoFrames)
	dec	bc
	ld	(VideoFrames),bc
	ld	a,b
	or	c
	jp	nz,BigLoop

CloseFile:
	//loop until break is NOT pressed
	ld	a,(BreakStatus)
	and	1
	jp	z,CloseFile
	//close file
	ld	bc,PRT4
	ld	a,$80
	out	(c),a
	call	GetResponse
	ret

GetResponse:
	//get response code
	.DelayStart:
		repeat 32
			nop
		end repeat
		in	a,(c)
	jp	m,.DelayStart
	cp	$40
	//ret if ok
	ret	z
ErrorHandling:
	//hack to get back to main loop and try next file
	pop	hl
	ret

RequestAndRead256BytesFromFileToHL:
	//request to read 256 bytes from file
	ld	bc,PRT5
	ld	a,0
	out	(c),a
	//get response code
	.DelayStart:
		repeat 32
			nop
		end repeat
		in	a,(c)
	jp	m,.DelayStart
	//prepare for read using magic turbo-mode
	ld	bc,PRT0
	ld	a,42
	out	(c),a
	//waste cycles
	repeat 6
		nop
	end repeat
	//read video stream to screen memory
	repeat 256
		ini
	end repeat
	ret

EnableHires:
	LD	HL,FRAMES		;FRAMES counter
	LD	A,(HL)			;get old FRAMES
.Loop:
	CP	(HL)			;compare to new FRAMES
	JR	Z,.Loop 		;exit after a change is detected
	LD	IX,WRX16		;simple start of the hires mode
	RET

DisableHires:
	LD	A,$1E			;SINCLAIR ROM pattern table MSB base address (1E00)
	LD	I,A			;pointer to I register
	LD	IX,$0281		;simple end of the hires mode
	RET

WRX16:
GenerateVsync:
	//syncronize the VSYNC-start
	repeat 28
		nop
	end repeat
	//store break-key status, VSYNC on, NMI off, HSYNC off
	ld	a,$7f
	in	a,($fe)
	and	1
	ld	(BreakStatus),a
VsyncDelay:
	repeat 207 * VSYNC_RASTERS / 4 - 5
		nop
	end repeat
PrepareTopBorder:
	//VSYNC off, NMI on, HSYNC on
	out	($fe),a

	ld	a,256 - TOP_BLANK_RASTERS
	ex	af,af'
	ld	ix,GenerateHires
	pop	hl
	pop	de
	pop	bc
	pop	af
	//return to do some useful work, and generate bottom blank lines
	ret

	align	32 //needed
LineBuffer:
	//load hfile address lsb
	ld	r,a
	repeat SCREEN_WIDTH_BYTES
		nop
	end repeat
	ret	nz

GenerateHires:
	//delay to synchronize picture-start
	ld	b,7
	djnz	$
	nop
	nop
	//clear Z-flag
	dec	b
	//start of hires-screen
ScreenAddress = $ + 1
	ld	hl,SCREEN_0
	ld	de,SCREEN_WIDTH_BYTES
	ld	b,SCREEN_HEIGHT_RASTERS
HiresLines:
		ld	a,h
		ld	i,a
		ld	a,l
		repeat 8
			nop
		end repeat
		//start generating the hires screen
		call	LineBuffer+$8000
		repeat 8
			nop
		end repeat
		//point to next hires line
		add	hl,de
		dec	b
	//loop until last line is done
	jp	nz,HiresLines
PrepareBottomBorder:
	ld	ix,GenerateVsync
field_set0:
	ld	a,256 - BOTTOM_BLANK_RASTERS
	ex	af,af'
	ld	hl,FrameCounter
	inc	(hl)
	//NMI-generator on
	out	($fe),a
	pop	hl
	pop	de
	pop	bc
	pop	af
	//return to do some useful work, and generate bottom blank lines
	ret

//sync with screen
WaitVSync:
	ld	hl,FrameCounter
	ld	a,(hl)
.FrameLoop:
	cp	(hl)
	jp	z,.FrameLoop
	ret

	align	32
SCREEN_0:
	repeat SCREEN_WIDTH_BYTES * SCREEN_HEIGHT_RASTERS
		db	0
	end repeat

	align	32
SCREEN_1:
	repeat SCREEN_WIDTH_BYTES * SCREEN_HEIGHT_RASTERS
		db	0
	end repeat
