/*
* KIM-1 ROM/RAM
* Based off the PICORom project by Nick Bild (nick.bild@gmail.com)
*
* Heavily modified for RAM/ROM and sideloading data.
* 
* March 2024
*/

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "pico/multicore.h"
#include "hardware/vreg.h"
#include "tusb.h"

#include "pin_definitions.h"
#include "commands.h"
#include "rambuf.h"

#define FLASH_SIZE (2 * 1024 * 1024)
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define LED_PIN 25

int ledValue = 0;

uint32_t addr_mask = 0;
uint32_t data_mask = 0;

uint32_t addr_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};
uint32_t data_pins[] = {D0, D1, D2, D3, D4, D5, D6, D7};
uint32_t ctrl_pins[] = {WE, PHI2};

uint32_t getTotalHeap(void);
uint32_t getFreeHeap(void);

void setup_gpio(void);
void main_memory_loop(void);

uint16_t ADDR_BOTTOM = (uint16_t) 0x2000;
uint16_t ADDR_TOP = ((uint16_t) 0xFFFF);

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

// this address should always never change value
// to stop things that probe RAM
#define IO_ADDRESS 0xFFF7

extern char __flash_binary_end;

void init_ux()
{
    // wait for USB serial
    while (!tud_cdc_connected()) { sleep_ms(100);  }

    printf("\n\n\n");

    printf("addr_mask          : 0x%08x\n", addr_mask);
    printf("addr_mask          : ");
    print_binary(addr_mask, 32);
    printf("\n");

    printf("data_mask          : 0x%08x\n", data_mask);
    printf("data_mask          : ");
    print_binary(data_mask, 32);
    printf("\n");
 
    printf("Free heap size     : %ld\n", getFreeHeap());

    uintptr_t end = (uintptr_t) &__flash_binary_end;
    printf("Binary ends at     : %08x\n", end);

    uintptr_t flashleft = (XIP_BASE + FLASH_SIZE) - end;

    printf("Bottom of flash    : 0x%08x\n", XIP_BASE);
    printf("Top of flash       : 0x%08x\n", XIP_BASE + FLASH_SIZE);
    printf("Flash Left         : 0x%08x\n", flashleft);
    printf("Flash Left         : %11db %.2fk %.2fm\n", flashleft, flashleft/1024.0, flashleft/1024.0/1024.0);
    printf("clk_sys            : %ukHz %.2fmHz\n",
        frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS),
        frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS) / 1000.0
    );


   printf("Size of sys_state   : %u\n", sizeof sys_state);
   printf("                    : %u\n", sizeof sys_state / 1024);
   printf(" (Flash pages)      : %u R %u\n", sizeof sys_state / (4 * 1024), sizeof sys_state % (4 * 1024));
 
   command_loop(XIP_BASE, FLASH_SIZE);
}


int main() {

    // Set system clock speed.
    // 400 MHz
    //  vreg_set_voltage(VREG_VOLTAGE_1_30);
    //  set_sys_clock_pll(1600000000, 4, 1);


    set_sys_clock_khz(250000, false);

    stdio_init_all();

    // Specify contents of emulated ROM.
    setup_memory_contents();


    //multicore_launch_core1(init_ux);

    multicore_launch_core1(main_memory_loop);


    init_ux();

}


void main_memory_loop() {
    uint16_t addr;
    uint16_t data;
    uint32_t all;
    uint32_t we;
    uint32_t cs;
    uint32_t phi2;

    // GPIO setup.
    setup_gpio();

    gpio_put(DEN, 0);

    while (true) {
        all = gpio_get_all();
        addr = all & (uint32_t) 0xFFFF;
        phi2 = (all & (uint32_t) (1 << PHI2));

        // Set the DECEN line based on the current addressing
        // If it is outside the range, then send DEN low
        // letting the KIM-1 decode that address internally. 
        gpio_put(DEN, addr >= ADDR_BOTTOM && addr <= ADDR_TOP);

        if (addr >= ADDR_BOTTOM && addr <= ADDR_TOP && phi2 != 0) {
            we = (all & (uint32_t) (1 << WE) );

            if (we == 0) {
                if ((sys_state.memory[addr] & (1 << RO_MEMORY_BIT)) == 0) {
                    data = (uint32_t) ((all & data_mask) >> D0);
                    sys_state.memory[addr] =  data;
                }
            } else {
                data = sys_state.memory[addr];
                gpio_set_dir_masked(data_mask, data_mask);	
                gpio_put_masked(data_mask, data  << D0);
            }
        } else {
            gpio_set_dir_masked(data_mask, 0);
        }
    }

}



void setup_gpio() {
    size_t i;
    int gpio;

#if 1
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    for(i = 0; i < 5; i++) {
        gpio_put(LED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_PIN, 0);
        sleep_ms(100);
    }
    gpio_put(LED_PIN, 0);
    gpio_set_dir(LED_PIN, GPIO_IN);
#endif

    gpio_init(DEN);
    gpio_set_function(DEN, GPIO_FUNC_SIO);
    gpio_set_dir(DEN, GPIO_OUT);

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
}


uint32_t getTotalHeap(void) {
   extern char __StackLimit, __bss_end__;
   return &__StackLimit  - &__bss_end__;
}

uint32_t getFreeHeap(void) {
   struct mallinfo m = mallinfo();
   return getTotalHeap() - m.uordblks;
}
