/*spi.obj
-stack   0x2000
-sysstack 0x1000
-heap   0x2000
-c
-o spi.out
-m spi.map
*/
MEMORY
{
 PAGE 0:
        MMR:    origin=000000h,length=0000c0h 
        SPRAM:  origin=0000c0h,length=000040h
 		VECS:   origin=000100h,length=000100h
 		DARAM:  origin=000200h,length=00fe00h
 		SARAM0: origin=010000h,length=010000h
 		SARAM1: origin=020000h,length=010000h
 		SARAM2: origin=030000h,length=010000h
 		CE0:    origin=040000h,length=3c0000h
 		CE1:    origin=400000h,length=400000h
 		CE2:    origin=800000h,length=400000h
 		CE3:    origin=0c00000h,length=3f8000h
 		PROM:   origin=0ff8000h,length=007f00h

PAGE 2:
		IOPORT: origin=000000h,length=020000h
}

SECTIONS
{
 .vectors:{} >VECS PAGE 0 
 .text:>SARAM0 PAGE 0   

 .data:>DARAM PAGE 0
 .bss:>DARAM PAGE 0
 .const:>DARAM PAGE 0
 .sysmem:>DARAM PAGE 0
 .stack:>DARAM PAGE 0
 .sysstack:>DARAM PAGE 0
 .cio:>DARAM PAGE 0

 .switch:>DARAM PAGE 0
 .cinit:>DARAM PAGE 0
 .pinit:>DARAM PAGE 0
 .csldata:>DARAM PAGE 0
 
 section_xx:>SARAM1 PAGE 0
 section_yy:>SARAM2 PAGE 0

 .ioport:>IOPORT PAGE 2 
}
