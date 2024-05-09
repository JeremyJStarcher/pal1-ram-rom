String a;

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

const int ADDR_LOW = 0x2000;
const int ADDR_HIGH = 0xFFF0;

#define TEST_LSB
#define TEST_MSB
#define TEST_WRITE

const int AA0 = 52;
const int AA1 = 53;

const int AA2 = 50;
const int AA3 = 51;

const int AA4 = 48;
const int AA5 = 49;

const int AA6 = 46;
const int AA7 = 47;

const int AA8 = 44;
const int AA9 = 45;

const int AA10 = 42;
const int AA11 = 43;

const int AA12 = 40;
const int AA13 = 41;

const int AA14 = 38;
const int AA15 = 39;

// R/W which the CPU sets high to read and low to write.
const int RW = 36;
const int PHI2 = 37;

const int DATA0 = 34;
const int DATA2 = 32;
const int DATA4 = A8;
const int DATA6 = 28;

const int DATA1 = 35;
const int DATA3 = 33;
const int DATA5 = 31;
const int DATA7 = 29;

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
const int data_lines[] = { DATA0, DATA1, DATA2, DATA3, DATA4, DATA5, DATA6, DATA7 };

void readString() {
  while (!Serial.available()) {
  }
  while (Serial.available()) {
    a = Serial.readString();  // read the incoming data as string
    //Serial.println(a);
  }
}

uint8_t slowRead(int pin) {
  uint8_t v;
  //delay(100);
  v = digitalRead(pin);
  //delay(100);
  return v;
}

uint8_t readDataPins() {
  uint8_t value = 0;
  uint8_t v;
  for (int i = 0; i < ARRAY_SIZE(data_lines); i++) {
    int pin = data_lines[i];
    pinMode(pin, INPUT);
    v = slowRead(pin);
    value |= v << i;
  }
  return value;
}


uint16_t readAddressPins() {
  set_data_lines_direction(INPUT);

  uint16_t value = 0;
  for (int i = 0; i < ARRAY_SIZE(address_lines); i++) {
    value |= (digitalRead(address_lines[i]) << i);
  }
  return value;
}

void writeDataToPins(uint8_t value) {
  for (int i = 0; i < ARRAY_SIZE(data_lines); i++) {
    int pin = data_lines[i];
    pinMode(pin, OUTPUT);
    digitalWrite(pin, value & (1 << i) ? HIGH : LOW);
    //delay(1 * 1000);
  }
}

void writeAddressToPins(uint16_t value) {
  for (int i = 0; i < ARRAY_SIZE(address_lines); i++) {
    digitalWrite(address_lines[i], value & (1 << i) ? HIGH : LOW);
  }
}

void set_address_lines_direction(int mode) {
  for (size_t i = 0; i < ARRAY_SIZE(address_lines); i++) {
    pinMode(address_lines[i], mode);
  }
}

void set_data_lines_direction(int mode) {
  for (size_t i = 0; i < ARRAY_SIZE(data_lines); i++) {
    int pin = data_lines[i];
    pinMode(pin, mode);
  }
}

void dump_addr_data(int addr, int data) {
  Serial.print("ADDR = ");
  Serial.print(addr, HEX);
  Serial.print(" Data = ");
  Serial.print(data, HEX);
  Serial.print(" ");
  Serial.print(data, BIN);

  Serial.print(" MATCH? ");
  Serial.print(addr & 0x00FF, HEX);

  Serial.println();
}

void reset_pins() {
  set_address_lines_direction(OUTPUT);
  set_data_lines_direction(INPUT);

  pinMode(DEN, INPUT);
  pinMode(RW, OUTPUT);
  pinMode(PHI2, OUTPUT);
  digitalWrite(RW, HIGH);
}

uint8_t read_data(uint16_t addr) {
  digitalWrite(PHI2, LOW);
  writeAddressToPins(addr);
  digitalWrite(PHI2, HIGH);
  uint8_t data = readDataPins();
  return data;
}

void setup() {
  uint8_t data;
  uint8_t is_match;

  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  delay(1 * 1000);

  Serial.println();
  Serial.println("====RESET===");
  reset_pins();

#ifdef TEST_LSB
  reset_pins();
  Serial.println("LOAD LSB INTO RAM AND PRESS ENTER");
  readString();
  Serial.println("RUNNING LSB TEST");
  for (int addr = ADDR_LOW; addr < ADDR_HIGH; addr += 1) {
    data = read_data(addr);
    is_match = (addr & 0x00FF) == data;
    if (!is_match) {
      Serial.print("LSB ");
      dump_addr_data(addr, data);
    }
  }
#endif


#ifdef TEST_MSB
  reset_pins();
  Serial.println("LOAD MSB INTO RAM AND PRESS ENTER");
  readString();
  Serial.println("RUNNING MSB TEST");
  for (int addr = ADDR_LOW; addr < ADDR_HIGH; addr += 1) {
    data = read_data(addr);
    is_match = (addr & 0xFF00) == data << 8;
    if (!is_match) {
      Serial.print("MSB ");
      dump_addr_data(addr, data);
    }
  }
#endif

#ifdef TEST_WRITE
  reset_pins();
  Serial.println("RUNNING WRITE TEST");
  for (int addr = ADDR_LOW; addr < ADDR_HIGH; addr += 1) {
    int flipped = ~(addr)&0x00FF;
    digitalWrite(PHI2, LOW);
    digitalWrite(RW, LOW);
    writeAddressToPins(addr);
    writeDataToPins(flipped);

    digitalWrite(PHI2, HIGH);
    digitalWrite(RW, HIGH);

    data = readDataPins();

    if (data != flipped) {
      Serial.print("FLIP TEST ");
      dump_addr_data(addr, data);
    }
  }
#endif

  Serial.println("<-- DONE -->");
}


void loop() {
}
