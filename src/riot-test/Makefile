RIOTTEST = riottest
ILINK = ilink

all: $(RIOTTEST).ptp $(ILINK).ptp Makefile
	./convert.py
	cp $(RIOTTEST).c ../pal-expansion

$(RIOTTEST).ptp: $(RIOTTEST).bin Makefile
	srec_cat $(RIOTTEST).bin -binary -offset 0x0200 -o $(RIOTTEST).ptp -MOS_Technologies

$(RIOTTEST).bin: $(RIOTTEST).o
	ld65 -t none -vm -m $(RIOTTEST).map -o $(RIOTTEST).bin $(RIOTTEST).o

$(RIOTTEST).o:	$(RIOTTEST).s
	ca65 --verbose -g -l $(RIOTTEST).lst $(RIOTTEST).s



$(ILINK).ptp: $(ILINK).bin Makefile
	srec_cat $(ILINK).bin -binary -offset 0x0200 -o $(ILINK).ptp -MOS_Technologies

$(ILINK).bin: $(ILINK).o
	ld65 -t none -vm -m $(ILINK).map -o $(ILINK).bin $(ILINK).o

$(ILINK).o:	$(ILINK).s
	ca65 --verbose -g -l $(ILINK).lst $(ILINK).s



clean:
	$(RM) *.o *.lst *.map *.bin $(RIOTTEST).c

distclean: clean
