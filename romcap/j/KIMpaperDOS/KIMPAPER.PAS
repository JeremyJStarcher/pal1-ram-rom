program kimbintopaper (input, output) ;

uses DOS ;

{

    KIM-1 Mos Technology BIN - papertape format conversion utility

    Hans Otten, june 8 2007

    Version 1.0 (convert to papertape works, needs testing)

    Applicable license:

    Creative Commons Naamsvermelding-NietCommercieel 2.5 Nederland

    See http://creativecommons.org/licenses/by-nc/2.5/nl/

    Remake of PDP-11 RSX-11M KIM utilities
    Rewrite in Turbo Pascal


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

    In this implementation output of XOFF and NULL characters is suppressed
    Papertape file output is pure ASCII
    While reading a papertapefile all controll chars except LFCR are ignored
    including UTF-8 overhead. UTF-8 is therefore acceptable input.


    Borland Pascal 7 syntax

    Compiled with Freepascal:
    > fpc -Mtp kimpaper.pas

}

type

  hex4t = array[1..4] of byte  ;


var

  startaddr      : longint ;
  converttopaper : boolean ;

  lowhex,
  highhex        : char ;

  papertapefile,
  Binfile        : file of byte ;
  papertapename,
  binname        : string ;



function checkhex (hexchar : byte) : boolean ;

 begin

   checkhex :=  (chr(hexchar) in ['A'..'F']) or (chr(hexchar) in ['0' .. '9']) ;


 end ; { checkhex }


function hextochar (data: byte) : char ;

  begin

    data := data mod 16 ;

    if data < 10
      then
        hextochar := chr(ord('0') + data)
      else
        hextochar := chr(ord('A') + data - 10) ;

  end ; { hextochar }



procedure converttohex(var lowhex, highhex : char ; data : byte) ;

  begin

   data := data mod 256 ;
   lowhex := hextochar (data mod 16) ;
   highhex := hextochar (data div 16) ;

  end ; { converttohex }


function convertfromhex (hexchar : byte) : byte ;


  begin

    if chr(hexchar) in ['A'..'Z']
      then
        convertfromhex := hexchar - ord('A') + 10
      else
        convertfromhex := hexchar - ord('0') ;


  end ; { converthex }


function packhex (lowhex, highhex: byte) : byte ;

  { convert 2 hex chars to binary byte }




  begin

    if checkhex(lowhex) and checkhex(highhex)
      then
        begin
          packhex  := convertfromhex(lowhex) + 16 * convertfromhex(highhex)
        end
      else
        begin
          writeln('Invalid non-hex character in file ',papertapename) ;
          halt(1) ;
        end ;

   end ; { packhex }


procedure convertlongtohex (number: longint ; var hex4: hex4t) ;


  var

    lowhex, highhex : char ;


  begin

    converttohex(lowhex, highhex, number div 256) ;
    hex4[1] := ord(highhex) ;
    hex4[2] := ord(lowhex) ;
    converttohex(lowhex, highhex, number mod 256) ;
    hex4[3] := ord(highhex) ;
    hex4[4] := ord(lowhex) ;


  end ;


procedure ShowHelp ;

  begin


     writeln('Syntax is: KIMPAPER [-[b|p|h] filename [startaddress]') ;
     writeln(' first parameter switches') ;
     writeln('        -h help') ;
     writeln('        -p convert to papertape') ;
     writeln('        -b convert to binary') ;
     writeln ;
     writeln('second parameter (first if no parameters, assumed binary to papertape)') ;
     writeln('               name of file to convert') ;
     writeln('               .BIN for binary, forces conversion to PAPertape') ;
     writeln('               .PAP for papertape, forces conversion to BINary') ;
     writeln ;
     writeln('third parameter (assumed 0000 if not present)') ;
     writeln('    startaddress for BIN to papertape conversion') ;
     writeln ;
     writeln('Files of type .BIN wil force conversion to papertape.PAP') ;
     writeln('Files of type .PAP wil force conversion to binary .BIN') ;
     writeln ;

  end ; { ShowHelp }


procedure processcommandline ;

  {
  first parameter switches
                   -h help
                   -p convert to papertape
                   -b convert to binary

   second parameter (first if no parameters, assumed binary to papertape)
                   name of file to convert

   third parameter (assumed 0000 if not present)
                    startaddress for BIN to papertape conversion


  }

  var

    p1, p2, p3 : string ;
    P : PathStr ;
    D : DirStr ;
    N : NameStr ;
    E : ExtStr ;
    i : integer ;
    stat : boolean ;

  begin


    p1 := '' ;
    p2 := '' ;
    p3 := '' ;

    if (paramcount = 0 ) or (paramcount > 3)
      then
        begin
          writeln('Syntax is: KIMPAPER [-[b|p] filename [startaddress]') ;
          halt(0) ;
        end  ;


    case  paramcount of
     1 : { must be filename }
         begin
           p2 := ParamStr(1)
         end ;
     2 : { options and filename or filename and startaddress }
         begin
           p1 := ParamStr(1) ;
           if p1[1] = '-'
             then
               p2 := Paramstr(2)
             else
               begin
                 p1 := '' ;
                 p2 := Paramstr(1) ;
                 p3 := Paramstr(2) ;
               end ;
         end ;
     3 : { option and filename and startaddress }
       begin
            p1 := Paramstr(1) ;
            p2 := Paramstr(2) ;
            p3 := Paramstr(3) ;
       end ;
     end ;

    { set defaults }

    converttopaper := true ;
    if p2 = '-h'
      then
        begin
          ShowHelp ;
          halt(0) ;
        end ;
    if length(p1) > 1
      then
        begin
          if p1[2] = 'b'
            then
              converttopaper := false
          else if p1[2] = 'p'
            then
            converttopaper := true
          else
            begin
              writeln('Invalid option') ;
              halt(1) ;
            end ;
         end ;


     { filename processing }
     P := p2 ;
     FSplit(P,D,N,E) ;
     if length(E) > 0
       then
         for i := 1 to length(E) do
           E[i] := Upcase(E[i]) ;
     if E = '.BIN'
       then
         begin
           Converttopaper := true ;
           BINname := D + N + E ;
           papertapename := D + N + '.PAP' ;
         end
     else if E = '.PAP'
       then
         begin
           Converttopaper := false ;
           Papertapename := D + N + E ;
           BINname := D + N + '.BIN' ;
         end
     else   { missing or other extension }

         begin
           if Converttopaper
             then
               begin
                 Papertapename := D + N + '.PAP' ;
                 BINname := D + N + E ;
               end
             else
               begin
                 papertapename := D + N + E ;
                 BINname := D + N + '.BIN' ;
               end
         end ;

    { startaddress processing, has to four hex chars }
    startaddr := 0 ;
    if length(p3) > 0
      then
        begin
          stat := (length(p3) = 4) ;
          if stat
            then
              startaddr := Packhex(ord(p3[2]), ord(p3[1])) ;
          if stat
	    then
	      startaddr := (256 * startaddr) + Packhex(ord(p3[4]), ord(p3[3])) ;
          if not stat
            then
              begin
                writeln('Invalid start address ', p3) ;
                halt(1) ;
              end ;
        end ;



  end ; { ProcessCommandline }


procedure OpenPapertapeRead ;

  { Papertape is sort of textfile, opened as binary }


  begin

    {$I-}
    assign(papertapefile, papertapename) ;
    reset(papertapefile) ;
    {$I+}
    if IOResult <> 0
      then
        begin
          writeln('Error opening file ', papertapename) ;
          halt(1) ;
        end ;

  end ; { OpenPapertapeRead }

Procedure OpenPapertapeWrite ;

  { Papertape is textfile. opened as binary }



  begin


    assign(papertapefile,papertapename) ;
    {$I-}
    rewrite(papertapefile) ;
    {$I+}
    if IOResult <> 0
      then
        begin
          writeln('Error opening file ', papertapename) ;
          halt(1) ;
        end ;


  end ; { OpenPaperTapeWrite }


procedure OpenBinRead ;

  begin

    {$I-}
    assign(binfile, binname) ;
    reset(binfile) ;
    {$I+}
    if IOResult <> 0
      then
        begin
          writeln('Error opening file ', binname) ;
          halt(1) ;
        end ;


  end ; {OpenBinRead }


procedure OpenBinWrite ;

  begin

    {$I-}
    assign(binfile, binname) ;
    rewrite(binfile) ;
    {$I+}
    if IOResult <> 0
      then
        begin
          writeln('Error ', IOresult:2, ' opening file ', binname) ;
          halt(1) ;
        end ;


  end ; { OpenBinWrite }


procedure writePapertapebyte(b: byte) ;

  begin

    {$I-}
    write(papertapefile,b) ;
    {$I+}
    if IOResult <> 0
      then
        begin
          writeln('Error writing to file ', papertapename);
          halt(1) ;
        end ;


  end ; { writePapertapebyte }


procedure getpaperbyte (var b : byte) ;

 { reads one byte from papertape
   skips all chars except ;, 0-9 and A-F
   if f attempt to read past eof: fatal, program halt
   expects papertape file to be opened
 }


  begin

    repeat
      {$I-1}
      read(papertapefile, b) ;
      {$I+}
      if IOResult <> 0
        then
          begin
            writeln('Error reading or invalid character in file ', papertapename) ;
            halt(1) ;
          end ;
    until (chr(b) = ';') or
    (chr(b) in ['0'..'9']) or
    (chr(b) in ['A'..'Z']) ;

  end ; { getpaperbyte }


procedure ConvertPapertapeToBIN ;

var
  b            : byte ;
  morerecords  : boolean ;
  readlines,
  totallines,
  startaddr,
  cstartaddr,
  lstartaddr,
  checksum,
  readchecksum : longint ;
  i,
  inlinecnt,
  totalinline  : integer ;

  hex4         : hex4t ;


  function Gethexnumber (check: boolean) : integer ;

    { read two hex bytes and convert to number, add checksum }
    var

      lowhex,
      highhex      : byte ;
      number : integer ;

    begin

      GetPaperByte(highhex) ;
      GetPaperByte(lowhex) ;
      number := Packhex(lowhex, highhex) ;
      if check
        then
          checksum := checksum + number ;
      GetHexnumber := number ;

    end ; { GetHexNumber }

  function GetLongHexnumber(check: boolean) : longint ;

    { read 4 hex chars into number }

    var

      number : longint ;


    begin

      number := GetHexnumber(check) ;
      GetLongHexnumber := 256 * number + GetHexNumber(check) ;


    end ; { Get LongHexnumber }


begin

  OpenPapertapeRead ;

  OpenBinWrite ;

  morerecords := true ;
  totallines := 0 ;
  while morerecords do { read all records until closing record }
    begin
      checksum := 0 ;
      { read record header ; }
      GetPaperbyte(b) ;
      if b <> ord(';')
        then
          begin
            writeln('Record start expected in papertape ',papertapename) ;
            halt(1)
          end ;
      { read totalinline count and add to checksum }

      totalinline := GetHexNumber(true) ;

      if totalinline = 0 { last record }
        then
          begin
            { read total lines and add to checksum}
            readlines := GetLongHexnumber(true) ;

            { if read total lines <> total lines then fatal error }
            if totallines <> readlines
              then
                begin
                  writeln('Error in number of records in ', papertapename) ;
                  halt(1)
                end ;


            { read checksum }
            readchecksum := GetlongHexNumber(false) ;
            { if read checksum <> calculated checksum fatal error }
            if checksum <> readchecksum
              then
                begin
                  writeln('Checksum error in ', papertapename) ;
                  halt(1)
                end ;


            { if read checksum <> calculated checksum fatal error }
            morerecords := false
          end
        else
          begin
            { read startaddress }
            lstartaddr := GetLongHexnumber(true) ;
            if totallines = 0
              then
                begin
                   startaddr := lstartaddr ;
                   cstartaddr := lstartaddr ;
                end
              else
                begin
                  if cstartaddr <> lstartaddr
                    then
                      writeln('No continous start address in line ', totallines) ;
                end ;
            { read data bytes from line }
            for inlinecnt := 1 to totalinline do
              begin
                { read two hex data bytes and add to checksum }
                b := GetHexnumber(true) ;
                { write to BIN file }
                {$I+}
                write(binfile,b) ;
                if IOresult <> 0
                  then
                    begin
                      writeln('Error writing to file ', binname) ;
                    end ;

              end ;
            { read checksum }
            readchecksum := GetlongHexNumber(false) ;

            { if read checksum <> calculated checksum fatal error }
            if checksum <> readchecksum
              then
                begin
                  writeln('Checksum error in ', papertapename) ;
                  halt(1)
                end ;
            totallines := totallines + 1 ;
            cstartaddr := cstartaddr + totalinline ;
          end

    end ;
  Close(Papertapefile) ;
  Close(BINfile) ;
  { convert start address from first record to hex }
  convertlongtohex(startaddr, hex4) ;
  write('Start address in ', papertapename, ' is ') ;
  for i := 1 to 4 do
    write(chr(hex4[i])) ;
  writeln ;


end ; { ConvertPapertapeToBin }

procedure ConvertBinToPapertape ;

  var

    binsize,
    totallinecount,
    checksum     : longint ;
    totalinline,
    inlinecnt,
    i            : integer ;
    hex4         : hex4t ;
    b,h          : byte ;


  begin

    OpenBinRead ;
    OpenPapertapeWrite ;

    binsize := filesize(binfile) ;
    if binsize = 0
     then
       begin
         writeln('Empty  BIN file, nothing to convert') ;
         halt(1) ;
       end ;
    totallinecount := 0 ;

    { dump BIN line by line, 24 max per line }
    while binsize > 0 do
      begin
        totallinecount := totallinecount + 1;
        { determine nr of bytes in this line to convert }
        inlinecnt := 0 ;
        checksum := 0 ;
        if binsize > 23
          then
            begin
              binsize := binsize - 24 ;
              totalinline := 24 ;
            end
          else  { last line }
            begin
              totalinline := binsize ;
              binsize := 0 ;
            end ;

        b := ord(';') ;
	writepapertapebyte(b) ;

        converttohex(lowhex,highhex,totalinline) ;
        h := ord(highhex) ;
        writepapertapebyte(h) ;
        h := ord(lowhex) ;
        writepapertapebyte(h) ;
        checksum := checksum + totalinline ;


        { convert current start address to hex }
        convertlongtohex(startaddr, hex4) ;
        for i := 1 to 4 do
          writepapertapebyte(hex4[i]) ;
        checksum := checksum + (startaddr mod 256) + (startaddr div 256) ;


        startaddr := startaddr + totalinline ;
        if startaddr > 65535
          then
            startaddr := startaddr - 65535 ;


        { the bytes in this line }
        for inlinecnt := 1 to totalinline do
          begin
            {$I-}
            read(binfile,b) ;
            {$I+}
            if IOResult <> 0
              then
                begin
                  writeln('Error reading file ', binname);
                  halt(1) ;
                end ;
            checksum := checksum + b ;
            converttohex(lowhex,highhex,b) ;
            h := ord(highhex) ;
            writepapertapebyte(h) ;
            h := ord(lowhex) ;
            writepapertapebyte(h) ;
          end ;

        { dump checksum }
        convertlongtohex(checksum, hex4) ;
        for i := 1 to 4 do
          writepapertapebyte(hex4[i]) ;


        { end of line }
        b := 13 ; {CR }
        writepapertapebyte(b) ;
        b := 10 ; { LF }
        writepapertapebyte(b) ;

      end ;

    { now finish with total line record }
    b := ord(';') ;
    writepapertapebyte(b) ;
    b := ord('0') ;
    writepapertapebyte(b) ;
    writepapertapebyte(b) ;


    checksum := 0 ;
    { convert totallinecount to hex }
    convertlongtohex(totallinecount, hex4) ;
    for i := 1 to 4 do
      writepapertapebyte(hex4[i]) ;
    checksum := checksum + (totallinecount mod 256) + (totallinecount div 256) ;

    { dump checksum }
    convertlongtohex(checksum, hex4) ;
    for i := 1 to 4 do
      writepapertapebyte(hex4[i]) ;

    { close files }

    Close(Papertapefile) ;
    Close(BINfile) ;


  end ; { ConvertBintoPapertape }


begin

  writeln('KIM-1 Mos Technology BIN - papertape format conversion utiliy, Hans Otten, 2007');

  processcommandline ;

  if converttopaper
    then
      convertbintopapertape
    else
      convertpapertapetobin ;


end.
