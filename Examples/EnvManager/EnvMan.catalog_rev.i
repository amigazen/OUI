VERSION		EQU	38
REVISION	EQU	4
DATE	MACRO
		dc.b	'23.9.96'
	ENDM
VERS	MACRO
		dc.b	'EnvMan.catalog 38.4'
	ENDM
VSTRING	MACRO
		dc.b	'EnvMan.catalog 38.4 (23.9.96)',13,10,0
	ENDM
VERSTAG	MACRO
		dc.b	0,'$VER: EnvMan.catalog 38.4 (23.9.96)',0
	ENDM
