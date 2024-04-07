
romdest = "../../src/kim-1-due/src/roms"
#all:	kim.ptp
all:	kim.c

kim.c: kim.bin Makefile
	../tools/rom2c kim.bin ${romdest}/cassette.h cassette 0000 03FF
	../tools/rom2c kim.bin ${romdest}/monitor.h monitor 0400 0800

kim.ptp: kim.bin Makefile
	srec_cat kim.bin -binary -offset 0x1800 -o kim.ptp -MOS_Technologies

kim.bin: kim.o
	ld65 -t none -vm -m kim.map -o kim.bin kim.o

kim.o:	kim.s
	ca65 -g -l kim.lst --feature labels_without_colons kim.s

clean:
	$(RM) *.o *.lst *.map *.bin *.ptp *.c

distclean: clean
