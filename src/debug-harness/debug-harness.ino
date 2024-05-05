#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

const int AA0 = 53;
const int AA1 = 52;
const int AA2 = 51;
const int AA3 = 50;
const int AA4 = 49;
const int AA5 = 48;
const int AA6 = 47;
const int AA7 = 46;
const int AA8 = 45;
const int AA9 = 44;
const int AA10 = 43;
const int AA11 = 42;
const int AA12 = 41;
const int AA13 = 40;
const int AA14 = 39;
const int AA15 = 38;
const int RW = 37;
const int PHI2 = 36;
const int DA0 = 35;
const int DA1 = 34;
const int DA2 = 33;
const int DA3 = 32;
const int DA4 = 31;
const int DA5 = 30;
const int DA6 = 29;
const int DA7 = 28;

const int RESET = A0;
const int PHI1 = A1;
const int IO3 = A2;
const int DEN = A3;
const int NMI = A4;
const int SYNC = A5;

const int IRQ = 7;
const int ZPB7 = 6;
const int SST = 5;
const int TAPE = 4;
const int RDY = 3;

const int address_lines[] = { AA0, AA1, AA2, AA3, AA4, AA5, AA6, AA7, AA8, AA9, AA10, AA11, AA12, AA13, AA14, AA15 };
const int data_lines[] = { DA0, DA1, DA2, DA4, DA5, DA6, DA6, DA7 };

void set_address_lines_direction(int mode) {
  size_t i;

  for (i = 0; i < ARRAY_SIZE(address_lines); i++) {
    pinMode(i, mode);
  }
}


void set_data_lines_direction(int mode) {
  size_t i;

  for (i = 0; i < ARRAY_SIZE(data_lines); i++) {
    pinMode(i, mode);
  }
}

void setup() {

  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  set_address_lines_direction(OUTPUT);
  set_data_lines_direction(OUTPUT);

  pinMode(DEN, INPUT);
  pinMode(RW, OUTPUT);
  pinMode(PHI2, OUTPUT);

  digitalWrite(RW, HIGH);
}


void loop() {
}
