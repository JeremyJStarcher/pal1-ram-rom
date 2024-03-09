/*
* PicoROM
* Simulate a ROM chip (e.g. 28c256) with a Raspberry Pi Pico.
* Nick Bild (nick.bild@gmail.com)
* August 2021
*/

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "pin_definitions.h"
#include <stdlib.h>
#include <stdio.h>

#include <malloc.h>
#include "tusb.h"

// #include "data.c"

#include "rom_ext.c"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

#define LED_PIN 25

int ledValue = 0;

uint32_t addr_mask = 0; 
uint32_t data_mask = 0; 

uint32_t addr_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};
uint32_t data_pins[] = {D0, D1, D2, D3, D4, D5, D6, D7};
uint32_t ctrl_pins[] = {WE, CS};

int last_addr_pin = A15;


uint32_t getTotalHeap(void);
uint32_t getFreeHeap(void);

void setup_gpio();
uint16_t get_requested_address();
void put_data_on_bus(int);
void setup_rom_contents();
void setbyte(uint32_t addr, uint8_t value);

uint16_t ADDR_BOTTOM = (uint16_t) 0x2000;
uint16_t ADDR_TOP = ((uint16_t) 0xDFFF);

/*
 * Because this location of this RAM spans the bit 16 boundary
 * it is easiser to just allocate the full 64K.  Spares doing
 * math later on.
 *
 * Likewise, the data pins are NOT sequential due to gaps on the
 * pico.
 *
 * Could do bunches of bit shifting, or we could just treat them
 * all as 16-bit bytes.
 *
 * Go for the speedy option.
 *
 * Besides, what else would we use the memory for?
 */

volatile uint16_t rom_contents[32768*2] = {};

int main() {
    uint16_t addr;
    uint16_t data;
    uint32_t all;
    uint32_t we;
    uint32_t cs;

    volatile uint16_t save_addr;
    volatile uint16_t save_data;


    stdio_init_all();
    // wait for USB serial
    // while (!tud_cdc_connected()) { sleep_ms(100);  }

    printf("Welcome\n");
    printf("Free heap size: %ld\n", getFreeHeap());

    // Specify contents of emulated ROM.
    setup_rom_contents();
    printf("ROM SET\n");

    // Set system clock speed.
    // 400 MHz
    vreg_set_voltage(VREG_VOLTAGE_1_30);
    set_sys_clock_pll(1600000000, 4, 1);
    
    // GPIO setup.
    setup_gpio();


    printf("addr_mask: 0x%08x\n", addr_mask);
    printf("data_mask: 0x%08x\n", data_mask);



    // Tis bit needs flipped, but we can't flip it in hardware right at the moment
    uint16_t mask = 1 << 13;  // Create a mask with a 1 at bit position 13.


    printf("Starting main loop\n");
    while (true) {
        all = gpio_get_all();
        addr = all & (uint32_t) 0xFFFF;
        cs = (all & (uint32_t) (1 << CS)) == 0;


        if (addr >= ADDR_BOTTOM && addr <= ADDR_TOP) {
            we = (all & (uint32_t) (1 << WE)) == 0;


             // printf("WE %04x %04x\n", addr, data);

            // if (we) {
            //     data = (uint32_t) (all >> D0); // all >> D0;
            //     printf("WE %04x %04x\n", addr, data);
            //     rom_contents[addr] = data;
            // } else {
                data = rom_contents[addr];
                gpio_set_dir_masked(data_mask, data_mask);	
                gpio_put_masked(data_mask, data  << D0);
//            }
        } else {
            gpio_set_dir_masked(data_mask, 0);
        }
    }
}

void setup_gpio() {
    size_t i;
    int gpio;

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    for(i = 0; i < NELEMS(addr_pins); i++) {
        gpio = addr_pins[i];
        addr_mask |= (1 << gpio);
        gpio_init(gpio);
        gpio_set_function(gpio, GPIO_FUNC_SIO); 
        gpio_set_dir(gpio, GPIO_IN);
    }

    for(i = 0; i < NELEMS(data_pins); i++) {
        gpio = data_pins[i];
        data_mask |= (1 << gpio);
        gpio_init(gpio);
        gpio_set_function(gpio, GPIO_FUNC_SIO); 
        gpio_set_dir(gpio, GPIO_IN);
    }

    for(i = 0; i < NELEMS(ctrl_pins); i++) {
        gpio = ctrl_pins[i];
        gpio_init(gpio);
        gpio_set_function(gpio, GPIO_FUNC_SIO); 
        gpio_set_dir(gpio, GPIO_IN);
    }

    for(i = 0; i < 5; i++) {
        gpio_put(LED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_PIN, 0);
        sleep_ms(100);
    }
}

uint16_t get_requested_address() {
    return gpio_get_all() & (uint32_t) 0xFFFF;
}

void put_data_on_bus(int address) {
    gpio_put_masked(data_mask, rom_contents[address]);
}

#define SETBIT(T, B, V) (T = V ? T | (1<<B) : T & ~(1<<B))
#define CHECK_BIT(x, pos) (x & (1UL << pos) )

uint16_t get_mangled_addr(uint16_t addr) {
    return addr;

    size_t mangled_addr = addr;
    SETBIT(mangled_addr, A13, !CHECK_BIT(addr, A13));

    return mangled_addr;

    // size_t mangled_addr = 0;
    // SETBIT(mangled_addr, A0, CHECK_BIT(addr, A0));
    // SETBIT(mangled_addr, A1, CHECK_BIT(addr, A1));
    // SETBIT(mangled_addr, A12, CHECK_BIT(addr, A2));
    // SETBIT(mangled_addr, A2, CHECK_BIT(addr, A3));
    // SETBIT(mangled_addr, A3, CHECK_BIT(addr, A4));
    // SETBIT(mangled_addr, A4, CHECK_BIT(addr, A5));
    // SETBIT(mangled_addr, A5, CHECK_BIT(addr, A6));
    // SETBIT(mangled_addr, A6, CHECK_BIT(addr, A7));
    // SETBIT(mangled_addr, A7, CHECK_BIT(addr, A8));
    // SETBIT(mangled_addr, A8, CHECK_BIT(addr, A9));
    // SETBIT(mangled_addr, A9, CHECK_BIT(addr, A10));
    // SETBIT(mangled_addr, A10, CHECK_BIT(addr, A11));
    // SETBIT(mangled_addr, A11, CHECK_BIT(addr, A12));
    // SETBIT(mangled_addr, A13, CHECK_BIT(addr, A13));
    // SETBIT(mangled_addr, A14, CHECK_BIT(addr, A14));
    // SETBIT(mangled_addr, A15, CHECK_BIT(addr, A15));
    // return mangled_addr;
}

	
#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

void setup_rom_contents() {
    unsigned int idx = 0;
    
    for(idx = 0; idx <= rom_extSize ; idx++) {
            setbyte(0xA000+idx, rom_ext[idx]);
    }
    return;

//    set_values();
//    return;
    
    uint16_t data;
    size_t len = sizeof(rom_contents)/sizeof(rom_contents[0]);

    for(uint32_t addr = 0; addr < len; addr++) {
            rom_contents[addr] = 0; 
    }

    for(uint32_t addr = 0; addr < len; addr++) {
            uint32_t mangled_addr = get_mangled_addr(addr);
            if (rom_contents[mangled_addr] != 0) {
                printf("COLLISION: ");
                printf("addr %04x  m %04x  data %04x\n", addr, mangled_addr, data);
            }
    }

    for(size_t addr = 0; addr < len; addr++) {
        uint32_t mangled_addr = get_mangled_addr(addr);
        // uint16_t data = LO_UINT16(addr);
        uint16_t data = HI_UINT16(addr);
        setbyte(addr, data);
    }
}

void setbyte(uint32_t addr, uint8_t value) {
    size_t mangled_addr = get_mangled_addr(addr);
    uint16_t data = value;

        // There are gaps in the pins, so adjust the data.
        // Any bit position not part of the data bus is a 
        // "don't care".

        if (data & 1 << 7) {
            data += 1 << (D7 - D0);
        }

        rom_contents[mangled_addr] = data;
}


uint32_t getTotalHeap(void) {
   extern char __StackLimit, __bss_end__;
   
   return &__StackLimit  - &__bss_end__;
}

uint32_t getFreeHeap(void) {
   struct mallinfo m = mallinfo();

   return getTotalHeap() - m.uordblks;
}
