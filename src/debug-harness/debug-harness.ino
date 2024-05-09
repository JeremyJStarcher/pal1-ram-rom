#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

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

uint8_t slowRead(int pin) {
  uint8_t v;
  //delay(1);
  v = digitalRead(pin);
  //delay(1);
  return v;
}

uint8_t readDataPins() {
  uint8_t value = 0;
  uint8_t v;
  //Serial.print("Data: ");
  for (int i = 0; i < ARRAY_SIZE(data_lines); i++) {
    int pin = data_lines[i];
    //pinMode(pin, INPUT_PULLUP);
    v = slowRead(pin);

    //    Serial.print(pin);
    //    Serial.print(":");
    //    Serial.print(v);
    //    Serial.print(" ");

    value |= v << i;
  }
  //  Serial.print(value);
  //  Serial.print(" 0x");
  //  Serial.print(value, HEX);
  //  Serial.println();
  return value;
}


uint16_t readAddressPins() {
  uint16_t value = 0;
  for (int i = 0; i < ARRAY_SIZE(address_lines); i++) {
    value |= (digitalRead(address_lines[i]) << i);
  }
  return value;
}

void writeAddressToPins(uint16_t value) {
  //Serial.print(value, HEX);
  //Serial.print("  ");
  for (int i = 0; i < ARRAY_SIZE(address_lines); i++) {
    //Serial.print(value & (1 << i) ? "1" : "0");
    digitalWrite(address_lines[i], value & (1 << i) ? HIGH : LOW);
  }
  //Serial.println();
}

void set_address_lines_direction(int mode) {
  for (size_t i = 0; i < ARRAY_SIZE(address_lines); i++) {
    pinMode(address_lines[i], mode);
  }
}

void set_data_lines_direction(int mode) {
  for (size_t i = 0; i < ARRAY_SIZE(data_lines); i++) {
    int pin = data_lines[i];
    // Serial.print("Setting pin ");
    // Serial.print(pin);
    // Serial.print(" to mode ");
    // Serial.println(mode);
    pinMode(pin, mode);
  }
}

void setup() {

  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  delay(1 * 1000);

  Serial.println("====RESET===");

  set_address_lines_direction(OUTPUT);
  set_data_lines_direction(INPUT);

  pinMode(DEN, INPUT);
  pinMode(RW, OUTPUT);
  pinMode(PHI2, OUTPUT);

  digitalWrite(RW, HIGH);

  for (int addr = 0x2000; addr < 0xFFF0; addr += 1 ) {

    digitalWrite(PHI2, LOW);
    writeAddressToPins(addr);

    digitalWrite(PHI2, HIGH);
    uint8_t data = readDataPins();
    
    uint8_t is_match = (addr & 0x00FF) == data;
    //uint8_t is_match = (addr & 0xFF00) == data << 8;

    if (!is_match) {
      Serial.print("ADDR = ");
      Serial.print(addr, HEX);
      Serial.print(" Data = ");
      Serial.print(data, HEX);
      Serial.print(" ");
      Serial.print(data, BIN);

      Serial.print(" MATCH? ");
      Serial.print(addr & 0x00FF, HEX);
      Serial.print(" MATCH2? ");
      Serial.print(is_match, HEX);

      Serial.println();
    }
  }
  Serial.println("<-- DONE -->");

}


void loop() {
}
