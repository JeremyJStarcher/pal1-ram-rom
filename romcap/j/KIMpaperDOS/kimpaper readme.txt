    KIM-1 Mos Technology BIN - papertape format conversion utility

    Hans Otten, June 8 2007, November 2019 March 2020

    Version 1.1 (MOS Technology object format papertape, TASM compatible)

    Applicable license:

    Creative Commons Naamsvermelding-NietCommercieel 2.5 Nederland

    See http://creativecommons.org/licenses/by-nc/2.5/nl/

    Remake of PDP-11 RSX-11M KIM utilities
    Rewrite in Turbo Pascal
    Rewrite in Feepascal compilable on Windows and Linux

    PAPER TAPE FORMAT
    The paper tape is
    a specific format designed to insure error free recovery.  Each byte
    of data to be stored is converted to two half bytes.  The half bytes
    (whose possible values are 0 to F HEX) are translated into their ASCII
    equivalents and written out onto paper tape in this form.

    Each record outputted
    - begins with a ";" character (ASCII 3B) to mark the start of a valid record.
    - The next byte transmitted (max 18 HEX) or (24 10) is the number of data bytes
      contained in the record.
    - The record's starting address High (1 byte, 2 characters),
    - starting address Lo (1 byte, 2 characters),
    - and data (max 24 bytes, 48 characters) follow.

    Each record is terminated by
    - the record's check-sum (2 bytes, 4 characters),
    - a carriage return (ASCII OD),
    - line feed (ASCII 0A),
    - and six "NULL" characters (ASCII 00).

    The last record transmitted has
    - zero data bytes (indicated by 00).
    - The starting address field is replaced by a four digit Hex number
      representing the total number of data records contained in the transmission,
    - followed by the records usual check-sum digits.

    - A "XOFF" character ends the transmission.

    Example

    ;180000FFEEDDCCBBAA0099887766554433221122334455667788990AFC
     --    --  --  --  --  --  --  --  --  --  --  --  --  ----
    ;0000010001
     ----    ----

    See the mpascalint.obj file for a more complicated papertape, created with the assember TASM Commandline
    
    > tasm -65 -g1 mpascalint.asm

    deliverS mpascalint.obj with many different load addresses

    (TASM Assembler Version 3.2 2001 Squak Valley Software)

    In this implementation output of XOFF and NULL characters is suppressed
    Papertape file output is pure ASCII
    While reading a papertapefile all controll chars except LFCR are ignored
    including UTF-8 overhead. UTF-8 is therefore acceptable input.

    Also the last record is ignored after the 00 bytes (for compatibility with TASM320)

    Borland Pascal 7 syntax

    Compiled with Freepascal:
    > fpc -Mtp kimpaper.pas
     
    Long file name support, 32 bit code due to FreePascal! 


E:\BP>kimpaper
KIM-1 Mos Technology BIN - papertape format conversion utiliy, Hans Otten, 2007

Syntax is: KIMPAPER [-[b|p] filename [startaddress]

E:\BP>kimpaper -h
KIM-1 Mos Technology BIN - papertape format conversion utiliy, Hans Otten, 2007
Syntax is: KIMPAPER [-[b|p|h] filename [startaddress]
 first parameter switches
        -h help
        -p convert to papertape
        -b convert to binary

second parameter (first if no parameters, assumed binary to papertape)
               name of file to convert
               .BIN for binary, forces conversion to PAPertape
               .PAP for papertape, forces conversion to BINary

third parameter (assumed 0000 if not present)
    startaddress for BIN to papertape conversion

Files of type .BIN wil force conversion to papertape.PAP
Files of type .PAP wil force conversion to binary .BIN

If the startaddress changes in the record to a different load address nn-contigious witht he previous the current BIN file is closed and a new one,
with a sequence number is created.

For every BIN file the load address is displayed in the screen. 
See the mpascalint.obj and the many BIN files results, each displayed with a corresponding load address.