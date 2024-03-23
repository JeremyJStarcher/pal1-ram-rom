                                                                        
| ADDRESS |      AREA      | LABEL |              FUNCTION               |
|         |                |       |                                     |
|  00EF   |                | PCL   | Program Counter - Low Order Byte    |
|  00F0   |                | PGH   | Program Counter - High Order Byte   |
|  00F1   |     Machine    | P     | Status Register                     |
|  00F2   |     Register   | SF    | Stack Pointer                       |
|         |     Storage    |       |                                     |
|  00F3   |     Buffer     | A     | Accumulator                         |
|  00F4   |                | Y     | Y-Index Register                    |
|  00F5   |                | X     | X-Index Register                    |
|  1700   |                | PAD   | 6530-003 A Data Register            |
|  1701   |   Application  | PADD  | 6530-003 A Data Direction Register  |
|  1702   |        I/O     | PBD   | 6530-003 B Data Register            |
|  1703   |                | PBDD  | 6530-003 B Data Direction Register  |
|  1704   |                |       | 6530-003 Interval Timer             |
|         | Interval Timer |       |   (See Section 1.6 of               |
|         |                |       |    Hardware Manual)                 |
|  170F   |                |       |                                     |
|  17F5   |                | SAL   | Starting Address - Low Order Byte   |
|  17F6   |   Audio Tape   | SAH   | Starting Address - High Order Byte  |
|  17F7   |   Load & Dump  | EAL   | Ending Address - Low Order Byte     |
|  17F8   |                | EAH   | Ending Address - High Order Byte    |
|  17F9   |                | ID    | File Identification Number          |
|  l7FA   |                | NMIL  | NMI Vector - Low Order Byte         |
|  l7FB   |                | NMIH  | NMI Vector - High Order Byte        |
|  l7FC   |   Interrupt    | RSTL  | RST Vector - Low Order Byte         |
|         |    Vectors     |       |                                     |
|  17FD   |                | RSTH  | RST Vector - High Order Byte        |
|  l7FE   |                | IRQL  | IRQ Vector - Low Order Byte         |
|  17FF   |                | IRQH  | IRQ Vector - High Order Byte        |
|  1800   |                | DUMPT | Start Address - Audio Tape Dump     |
|         |  Audio Tape    |       |                                     |
|  1873   |                | LOADT | Start Address - Audio Tape Load     |
|  1C00   | STOP Key + SST |       | Start Address for NMI using KIM     |
|         |                |       | "Save Nachine" Routine (Load in     |
|         |                |       | 17FA & 17FB)                        |
|  17F7   |   Paper Tape   | EAL   | Ending Address - Low Order Byte     |
|  17F8   |    Dump (Q)    | EAH   | Ending Address - High Order Byte    |
