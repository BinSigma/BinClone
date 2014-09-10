;
; +-------------------------------------------------------------------------+
; |   This file	has been generated by The Interactive Disassembler (IDA)    |
; |	   Copyright (c) 2010 by Hex-Rays, <support@hex-rays.com>	    |
; |			 License info: 48-B231-7414-99			    |
; |		       NCFTA Canada/Concordia University		    |
; +-------------------------------------------------------------------------+
;
;
; +-------------------------------------------------------------------------+
; |   This file	has been generated by The Interactive Disassembler (IDA)    |
; |	   Copyright (c) 2010 by Hex-Rays, <support@hex-rays.com>	    |
; |			 License info: 48-B231-7414-99			    |
; |		       NCFTA Canada/Concordia University		    |
; +-------------------------------------------------------------------------+
;
; Input	MD5   :	A207F5BB4CAAFCAF1EBDA5E9CAD71D7E
; Input	CRC32 :	839FD6DD

; File Name   :	c:\Users\farhadi\Desktop\dll4\ArcCon.dll
; Format      :	Portable executable for	80386 (PE)
; Imagebase   :	10000000
; Section 1. (virtual address 00001000)
; Virtual size			: 00024EBE ( 151230.)
; Section size in file		: 00025000 ( 151552.)
; Offset to raw	data for section: 00001000
; Flags	60000020: Text Executable Readable
; Alignment	: default
; OS type	  :  MS	Windows
; Application type:  DLL 32bit

		include	uni.inc	; see unicode subdir of	ida for	info on	unicode

		.686p
		.mmx
		.model flat

; ===========================================================================

; Segment type:	Pure code
; Segment permissions: Read/Execute
_text		segment	para public 'CODE' use32
		assume cs:_text
		;org 10001000h
		assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

; =============== S U B	R O U T	I N E =======================================


; int __cdecl sub_10001000(wchar_t *Str2)
sub_10001000	proc near		; CODE XREF: ARCCON_InstallInit+84p

cbNeeded	= dword	ptr -180Ch
var_1808	= dword	ptr -1808h
hModule		= dword	ptr -1804h
Str1		= word ptr -1800h
idProcess	= dword	ptr -1000h
Str2		= dword	ptr  4

		mov	eax, 180Ch
		call	__alloca_probe
		lea	eax, [esp+180Ch+cbNeeded]
		lea	ecx, [esp+180Ch+idProcess]
		push	eax		; lpcbNeeded
		push	1000h		; cb
		push	ecx		; lpidProcess
		call	EnumProcesses
		test	eax, eax
		jnz	short loc_10001031
		mov	eax, 1
		add	esp, 180Ch
		retn
; ---------------------------------------------------------------------------

loc_10001031:				; CODE XREF: sub_10001000+23j
		push	ebx
		push	ebp
		mov	ebp, [esp+1814h+cbNeeded]
		xor	ebx, ebx
		push	esi
		push	edi
		shr	ebp, 2
		mov	[esp+181Ch+var_1808], ebx
		jz	loc_100010D3
		lea	edi, [esp+181Ch+idProcess]

loc_1000104F:				; CODE XREF: sub_10001000+BAj
		mov	edx, [edi]
		push	edx		; dwProcessId
		push	0		; bInheritHandle
		push	410h		; dwDesiredAccess
		call	ds:OpenProcess
		mov	esi, eax
		test	esi, esi
		jz	short loc_100010B4
		lea	eax, [esp+181Ch+cbNeeded]
		lea	ecx, [esp+181Ch+hModule]
		push	eax		; lpcbNeeded
		push	4		; cb
		push	ecx		; lphModule
		push	esi		; hProcess
		call	EnumProcessModules
		test	eax, eax
		jz	short loc_100010AD
		mov	eax, [esp+181Ch+hModule]
		lea	edx, [esp+181Ch+Str1]
		push	800h		; nSize
		push	edx		; lpBaseName
		push	eax		; hModule
		push	esi		; hProcess
		call	GetModuleBaseNameW
		test	eax, eax
		jz	short loc_100010AD
		mov	ecx, [esp+181Ch+Str2]
		lea	edx, [esp+181Ch+Str1]
		push	ecx		; Str2
		push	edx		; Str1
		call	__wcsicmp
		add	esp, 8
		test	eax, eax
		jz	short loc_100010CB

loc_100010AD:				; CODE XREF: sub_10001000+79j
					; sub_10001000+92j
		push	esi		; hObject
		call	ds:CloseHandle

loc_100010B4:				; CODE XREF: sub_10001000+63j
		inc	ebx
		add	edi, 4
		cmp	ebx, ebp
		jb	short loc_1000104F
		mov	eax, [esp+181Ch+var_1808]
		pop	edi
		pop	esi
		pop	ebp
		pop	ebx
		add	esp, 180Ch
		retn
; ---------------------------------------------------------------------------

loc_100010CB:				; CODE XREF: sub_10001000+ABj
		mov	[esp+181Ch+var_1808], 1

loc_100010D3:				; CODE XREF: sub_10001000+42j
		mov	eax, [esp+181Ch+var_1808]
		pop	edi
		pop	esi
		pop	ebp
		pop	ebx
		add	esp, 180Ch
		retn
sub_10001000	endp

; ---------------------------------------------------------------------------
		align 10h

; =============== S U B	R O U T	I N E =======================================




; ---------------------------------------------------------------------------
		align 10h

; =============== S U B	R O U T	I N E =======================================




; ---------------------------------------------------------------------------
		align 4
off_10007644	dd offset loc_10006CC1	; DATA XREF: sub_10006A70+24Ar
		dd offset loc_10006D28	; jump table for switch	statement
		dd offset loc_10006D92
		dd offset loc_10006DF9
		dd offset loc_10006E66
		dd offset loc_10006ED0
		dd offset loc_10006F3D
		dd offset loc_10006FA7
		dd offset loc_10007014
		dd offset loc_1000707E
		dd offset loc_100070E4
		dd offset loc_1000714D
byte_10007674	db	0,     1,     2,   0Bh ; DATA XREF: sub_10006A70+244r
		db    0Bh,   0Bh,   0Bh,     3 ; indirect table	for switch statement
		db	4,   0Bh,   0Bh,   0Bh
		db    0Bh,   0Bh,   0Bh,   0Bh
		db    0Bh,     5,     6,     7
		db	8,     9,   0Ah
		align 10h

; =============== S U B	R O U T	I N E =======================================


sub_10007690	proc near		; CODE XREF: sub_10008ED0+6Dp

var_14		= dword	ptr -14h
var_10		= dword	ptr -10h
Buffer		= dword	ptr -0Ch
var_8		= dword	ptr -8
var_4		= dword	ptr -4
arg_0		= dword	ptr  4

		sub	esp, 14h
		push	ebx
		mov	ebx, [esp+18h+arg_0]
		push	esi
		push	edi
		test	ebx, ebx
		mov	esi, ecx
		jz	loc_10007A7F
		mov	eax, [esi]
		mov	[esp+20h+Buffer], 51524752h
		mov	[esp+20h+var_8], eax
		mov	eax, [esi+8]
		test	eax, eax
		jnz	short loc_100076BF
		mov	[esp+20h+var_4], eax
		jmp	short loc_100076D9
; ---------------------------------------------------------------------------

loc_100076BF:				; CODE XREF: sub_10007690+27j
		mov	ecx, [esi+0Ch]
		sub	ecx, eax
		mov	eax, 2E8BA2E9h
		imul	ecx
		sar	edx, 5
		mov	ecx, edx
		shr	ecx, 1Fh
		add	edx, ecx
		mov	[esp+20h+var_4], edx

loc_100076D9:				; CODE XREF: sub_10007690+2Dj
		lea	edx, [esp+20h+Buffer]
		push	0Ch		; nNumberOfBytesToWrite
		push	edx		; lpBuffer
		mov	ecx, ebx
		call	sub_10006370
		cmp	eax, 0Ch
		jnz	loc_10007A7F
		mov	eax, [esi+8]
		mov	esi, [esi+0Ch]
		cmp	eax, esi
		mov	[esp+20h+var_14], eax
		mov	[esp+20h+var_10], esi
		jz	loc_10007A71
		lea	esi, [eax+24h]

loc_10007709:				; CODE XREF: sub_10007690+3DBj
		mov	eax, [esi-1Ch]
		test	eax, eax
		jbe	short loc_10007753
		lea	eax, [esp+20h+arg_0]
		push	2		; nNumberOfBytesToWrite
		push	eax		; lpBuffer
		mov	ecx, ebx
		mov	[esp+28h+arg_0], 101h
		call	sub_10006370
		cmp	eax, 2
		jnz	loc_10007A7F
		mov	ecx, [esi-1Ch]
		mov	eax, [esi-20h]
		test	eax, eax
		lea	edi, [ecx+ecx+2]
		jnz	short loc_10007742
		mov	eax, offset NewFileName

loc_10007742:				; CODE XREF: sub_10007690+ABj
		push	edi		; nNumberOfBytesToWrite
		push	eax		; lpBuffer
		mov	ecx, ebx
		call	sub_10006370
		cmp	eax, edi
		jnz	loc_10007A7F

loc_10007753:				; CODE XREF: sub_10007690+7Ej
		mov	eax, [esi-0Ch]
		test	eax, eax
		jbe	short loc_1000779D
		lea	edx, [esp+20h+arg_0]
		push	2		; nNumberOfBytesToWrite
		push	edx		; lpBuffer
		mov	ecx, ebx
		mov	[esp+28h+arg_0], 102h
		call	sub_10006370
		cmp	eax, 2
		jnz	loc_10007A7F
		mov	eax, [esi-0Ch]
		lea	edi, [eax+eax+2]
		mov	eax, [esi-10h]
		test	eax, eax
		jnz	short loc_1000778C
		mov	eax, offset NewFileName

loc_1000778C:				; CODE XREF: sub_10007690+F5j
		push	edi		; nNumberOfBytesToWrite
		push	eax		; lpBuffer
		mov	ecx, ebx
		call	sub_10006370
		cmp	eax, edi
		jnz	loc_10007A7F

loc_1000779D:				; CODE XREF: sub_10007690+C8j
		mov	eax, [esi+4]
		test	eax, eax
		jbe	short loc_100077E6
		lea	ecx, [esp+20h+arg_0]
		push	2		; nNumberOfBytesToWrite
		push	ecx		; lpBuffer
		mov	ecx, ebx
		mov	[esp+28h+arg_0], 103h
		call	sub_10006370
		cmp	eax, 2
		jnz	loc_10007A7F
		mov	edx, [esi+4]
		mov	eax, [esi]
		test	eax, eax
		lea	edi, [edx+edx+2]
		jnz	short loc_100077D5
		mov	eax, offset NewFileName

loc_100077D5:				; CODE XREF: sub_10007690+13Ej
		push	edi		; nNumberOfBytesToWrite
		push	eax		; lpBuffer
		mov	ecx, ebx
		call	sub_10006370
		cmp	eax, edi
		jnz	loc_10007A7F

loc_100077E6:				; CODE XREF: sub_10007690+112j
		mov	eax, [esi+14h]
		test	eax, eax
		jbe	short loc_10007830
		lea	eax, [esp+20h+arg_0]
		push	2		; nNumberOfBytesToWrite
		push	eax		; lpBuffer
		mov	ecx, ebx
		mov	[esp+28h+arg_0], 108h
		call	sub_10006370
		cmp	eax, 2
		jnz	loc_10007A7F
		mov	ecx, [esi+14h]
		mov	eax, [esi+10h]
		test	eax, eax
		lea	edi, [ecx+ecx+2]
		jnz	short loc_1000781F
		mov	eax, offset NewFileName

loc_1000781F:				; CODE XREF: sub_10007690+188j
		push	edi		; nNumberOfBytesToWrite
		push	eax		; lpBuffer
		mov	ecx, ebx
		call	sub_10006370
		cmp	eax, edi
		jnz	loc_10007A7F

loc_10007830:				; CODE XREF: sub_10007690+15Bj
		mov	eax, [esi+74h]
		test	eax, eax
		jz	short loc_1000787A
		lea	edx, [esp+20h+arg_0]
		push	2		; nNumberOfBytesToWrite
		push	edx		; lpBuffer
		mov	ecx, ebx
		mov	[esp+28h+arg_0], 116h
		call	sub_10006370
		cmp	eax, 2
		jnz	loc_10007A7F
		mov	eax, [esi+74h]
		lea	edi, [eax+eax+2]
		mov	eax, [esi+70h]
		test	eax, eax
		jnz	short loc_10007869
		mov	eax, offset NewFileName

loc_10007869:				; CODE XREF: sub_10007690+1D2j
		push	edi		; nNumberOfBytesToWrite
		push	eax		; lpBuffer
		mov	ecx, ebx
		call	sub_10006370
		cmp	eax, edi
		jnz	loc_10007A7F

loc_1000787A:				; CODE XREF: sub_10007690+1A5j
		mov	eax, [esi+84h]
		test	eax, eax
		jz	short loc_100078CD
		lea	ecx, [esp+20h+arg_0]
		push	2		; nNumberOfBytesToWrite
		push	ecx		; lpBuffer
		mov	ecx, ebx
		mov	[esp+28h+arg_0], 117h
		call	sub_10006370
		cmp	eax, 2
		jnz	loc_10007A7F
		mov	edx, [esi+84h]
		mov	eax, [esi+80h]
		test	eax, eax
		lea	edi, [edx+edx+2]
		jnz	short loc_100078BC
		mov	eax, offset NewFileName

loc_100078BC:				; CODE XREF: sub_10007690+225j
		push	edi		; nNumberOfBytesToWrite
		push	eax		; lpBuffer
		mov	ecx, ebx
		call	sub_10006370
		cmp	eax, edi
		jnz	loc_10007A7F

loc_100078CD:				; CODE XREF: sub_10007690+1F2j
		mov	eax, [esi+24h]
		test	eax, eax
		jbe	short loc_10007917
		lea	eax, [esp+20h+arg_0]
		push	2		; nNumberOfBytesToWrite
		push	eax		; lpBuffer
		mov	ecx, ebx
		mov	[esp+28h+arg_0], 109h
		call	sub_10006370
		cmp	eax, 2
		jnz	loc_10007A7F
		mov	ecx, [esi+24h]
		mov	eax, [esi+20h]
		test	eax, eax
		lea	edi, [ecx+ecx+2]
		jnz	short loc_10007906
		mov	eax, offset NewFileName

loc_10007906:				; CODE XREF: sub_10007690+26Fj
		push	edi		; nNumberOfBytesToWrite
		push	eax		; lpBuffer
		mov	ecx, ebx
		call	sub_10006370
		cmp	eax, edi
		jnz	loc_10007A7F

loc_10007917:				; CODE XREF: sub_10007690+242j
		mov	eax, [esi+34h]
		test	eax, eax
		jbe	short loc_10007961
		lea	edx, [esp+20h+arg_0]
		push	2		; nNumberOfBytesToWrite
		push	edx		; lpBuffer
		mov	ecx, ebx
		mov	[esp+28h+arg_0], 112h
		call	sub_10006370
		cmp	eax, 2
		jnz	loc_10007A7F
		mov	eax, [esi+34h]
		lea	edi, [eax+eax+2]
		mov	eax, [esi+30h]
		test	eax, eax
		jnz	short loc_10007950
		mov	eax, offset NewFileName

loc_10007950:				; CODE XREF: sub_10007690+2B9j
		push	edi		; nNumberOfBytesToWrite
		push	eax		; lpBuffer
		mov	ecx, ebx
		call	sub_10006370
		cmp	eax, edi
		jnz	loc_10007A7F

loc_10007961:				; CODE XREF: sub_10007690+28Cj
		mov	eax, [esi+44h]
		test	eax, eax
		jbe	short loc_100079AB
		lea	ecx, [esp+20h+arg_0]
		push	2		; nNumberOfBytesToWrite
		push	ecx		; lpBuffer
		mov	ecx, ebx
		mov	[esp+28h+arg_0], 113h
		call	sub_10006370
		cmp	eax, 2
		jnz	loc_10007A7F
		mov	edx, [esi+44h]
		mov	eax, [esi+40h]
		test	eax, eax
		lea	edi, [edx+edx+2]
		jnz	short loc_1000799A
		mov	eax, offset NewFileName

loc_1000799A:				; CODE XREF: sub_10007690+303j
		push	edi		; nNumberOfBytesToWrite
		push	eax		; lpBuffer
		mov	ecx, ebx
		call	sub_10006370
		cmp	eax, edi
		jnz	loc_10007A7F

loc_100079AB:				; CODE XREF: sub_10007690+2D6j
		mov	eax, [esi+54h]
		test	eax, eax
		jbe	short loc_100079F5
		lea	eax, [esp+20h+arg_0]
		push	2		; nNumberOfBytesToWrite
		push	eax		; lpBuffer
		mov	ecx, ebx
		mov	[esp+28h+arg_0], 114h
		call	sub_10006370
		cmp	eax, 2
		jnz	loc_10007A7F
		mov	ecx, [esi+54h]
		mov	eax, [esi+50h]
		test	eax, eax
		lea	edi, [ecx+ecx+2]
		jnz	short loc_100079E4
		mov	eax, offset NewFileName

loc_100079E4:				; CODE XREF: sub_10007690+34Dj
		push	edi		; nNumberOfBytesToWrite
		push	eax		; lpBuffer
		mov	ecx, ebx
		call	sub_10006370
		cmp	eax, edi
		jnz	loc_10007A7F

loc_100079F5:				; CODE XREF: sub_10007690+320j
		mov	eax, [esi+64h]
		test	eax, eax
		jbe	short loc_10007A37
		lea	edx, [esp+20h+arg_0]
		push	2		; nNumberOfBytesToWrite
		push	edx		; lpBuffer
		mov	ecx, ebx
		mov	[esp+28h+arg_0], 115h
		call	sub_10006370
		cmp	eax, 2
		jnz	short loc_10007A7F
		mov	eax, [esi+64h]
		lea	edi, [eax+eax+2]
		mov	eax, [esi+60h]
		test	eax, eax
		jnz	short loc_10007A2A
		mov	eax, offset NewFileName

loc_10007A2A:				; CODE XREF: sub_10007690+393j
		push	edi		; nNumberOfBytesToWrite
		push	eax		; lpBuffer
		mov	ecx, ebx
		call	sub_10006370
		cmp	eax, edi
		jnz	short loc_10007A7F

loc_10007A37:				; CODE XREF: sub_10007690+36Aj
		lea	ecx, [esp+20h+arg_0]
		push	2		; nNumberOfBytesToWrite
		push	ecx		; lpBuffer
		mov	ecx, ebx
		mov	[esp+28h+arg_0], 0
		call	sub_10006370
		cmp	eax, 2
		jnz	short loc_10007A7F
		mov	eax, [esp+20h+var_14]
		mov	ecx, [esp+20h+var_10]
		add	eax, 0B0h
		add	esi, 0B0h
		cmp	eax, ecx
		mov	[esp+20h+var_14], eax
		jnz	loc_10007709

loc_10007A71:				; CODE XREF: sub_10007690+70j
		pop	edi
		pop	esi
		mov	eax, 1
		pop	ebx
		add	esp, 14h
		retn	4
; ---------------------------------------------------------------------------

loc_10007A7F:				; CODE XREF: sub_10007690+Ej
					; sub_10007690+5Aj ...
		pop	edi
		pop	esi
		xor	eax, eax
		pop	ebx
		add	esp, 14h
		retn	4
sub_10007690	endp