/*
 * KIM-1 ROM/RAM
 * Based off the PICORom project by Nick Bild (nick.bild@gmail.com)
 *
 * Heavily modified for RAM/ROM and sideloading data.
 *
 * March 2024
 */

#define EMULATE_RIOT
#define EMULATE_MEMORY

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

// #include "6532_timer.h"
#include "6532_timer.c"
#include "clockspeed.h"
#include "commands.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pin_definitions.h"
#include "rambuf.h"
#include "tusb.h"

#define FLASH_SIZE (2 * 1024 * 1024)
#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))
#define LED_PIN 25

int ledValue = 0;

uint32_t addr_mask = 0;
uint32_t data_mask = 0;

uint32_t addr_pins[] = {A0, A1, A2,  A3,  A4,  A5,  A6,  A7,
                        A8, A9, A10, A11, A12, A13, A14, A15};
uint32_t data_pins[] = {D0, D1, D2, D3, D4, D5, D6, D7};
uint32_t ctrl_pins[] = {WE, PHI2};

uint32_t getTotalHeap(void);
uint32_t getFreeHeap(void);

void setup_gpio(void);
void main_memory_loop(void);

uint16_t ADDR_BOTTOM = (uint16_t)0x2000;
uint16_t ADDR_TOP = ((uint16_t)0xFFFF);

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a)&0xFF)

// this address should always never change value
// to stop things that probe RAM
#define IO_ADDRESS 0xFFF7

extern char __flash_binary_end;

void init_ux() {
  // wait for USB serial
  while (!tud_cdc_connected()) {
    sleep_ms(100);
  }

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

  uintptr_t end = (uintptr_t)&__flash_binary_end;
  printf("Binary ends at     : %08x\n", end);

  uintptr_t flashleft = (XIP_BASE + FLASH_SIZE) - end;

  printf("Bottom of flash    : 0x%08x\n", XIP_BASE);
  printf("Top of flash       : 0x%08x\n", XIP_BASE + FLASH_SIZE);
  printf("Flash Left         : 0x%08x\n", flashleft);
  printf("Flash Left         : %11db %.2fk %.2fm\n", flashleft,
         flashleft / 1024.0, flashleft / 1024.0 / 1024.0);
  printf("clk_sys            : %ukHz %.2fmHz\n",
         frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS),
         frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS) / 1000.0);

  printf("Size of sys_state   : %u\n", sizeof sys_state);
  printf("                    : %u\n", sizeof sys_state / 1024);
  printf(" (Flash pages)      : %u R %u\n", sizeof sys_state / (4 * 1024),
         sizeof sys_state % (4 * 1024));

  command_loop(XIP_BASE, FLASH_SIZE);
}

int main() {
  // Set system clock speed.
  // 400 MHz
  //  vreg_set_voltage(VREG_VOLTAGE_1_30);
  //  set_sys_clock_pll(1600000000, 4, 1);

  set_sys_clock_khz(CLOCK_SPEED_HIGH, false);
  vreg_set_voltage(VREG_VOLTAGE_1_30);

  stdio_init_all();

  // Specify contents of emulated ROM.
  setup_memory_contents();

  // GPIO setup.
  setup_gpio();
  gpio_put(DEN, 0);

  setup_riot_lookup_tables();

  multicore_launch_core1(main_memory_loop);

  init_ux();
}

void main_memory_loop() {
  uint_fast16_t addr;
  uint_fast16_t data;
  uint32_t all;
  uint32_t we_n;
  uint32_t cs;
  uint32_t phi2;

#ifdef EMULATE_RIOT
  uint_fast8_t riot_phi2_state = false;
  uint_fast8_t riot_range;
  uint_fast8_t riot_underflow;
  uint_fast16_t riot_data;
  uint_fast16_t riot_addr;

#endif

  save_and_disable_interrupts();

  while (true) {
    all = gpio_get_all();

    addr = all & (uint32_t)0xFFFF;
    phi2 = (all & (uint32_t)(1 << PHI2));

    we_n = (all & (uint32_t)(1 << WE));

    if (phi2) {
#ifdef EMULATE_RIOT
      riot_phi2_state = true;
#endif
    } else {
      gpio_set_dir_masked(data_mask, 0);

#ifdef EMULATE_RIOT
      if (riot_phi2_state) {
        riot_counter--;
        riot_phi2_state = false;
        // Do this calculation when we are not as busy
        riot_underflow = riot_counter & (1 << 18);
      }
#endif
    }

#ifdef EMULATE_RIOT
    riot_range = addr >= 0x1700 && addr <= 0x170F;
    if (riot_range) {
      if (phi2) {
        if (we_n == 0) {
          data = (uint32_t)((all & data_mask) >> D0);
          // inline 16_to_8
          data |= ((data >> (D7 - D0)) & 1) << 7;

          // Inline this looking for speed
          // Comes close but is still a bit slow
          addr = addr & 0x000F;
          period_type = periodtypemap[addr];
          riot_counter = data << shiftmap[addr];
        } else {
          // data = riot_read_timer(addr);

          if (riot_underflow) {
            // Let the casting get rid of the rest
            data = riot_counter;
          } else {
            data = riot_counter >> shiftmap2[period_type];
          }

          // inline 8 to 16
          data |= ((data & (1 << 7)) ? 1 : 0) << D7 - D0;
          // gpio_put(DEN, 0);
          gpio_set_dir_masked(data_mask, data_mask);
          gpio_put_masked(data_mask, data << D0);
        }
      }
      continue;
    }
#endif

#ifdef EMULATE_MEMORY
    data = sys_state.memory[addr];
    int decen = (data & 1 << IN_USE_BIT) ? 1 : 0;

    gpio_put(DEN, decen);

    if (decen && phi2) {
      if (we_n == 0 && (data & (1 << RO_MEMORY_BIT)) == 0) {
        data = (uint32_t)((all & data_mask) >> D0);
        sys_state.memory[addr] = data | (1 << IN_USE_BIT);
      } else {
        gpio_set_dir_masked(data_mask, data_mask);
        gpio_put_masked(data_mask, data << D0);
      }
    } else {
      //      gpio_set_dir_masked(data_mask, 0);
    }
#endif
  }
}

void setup_gpio() {
  size_t i;
  int gpio;

#if 0
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  for (i = 0; i < 5; i++) {
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

  for (i = 0; i < NELEMS(addr_pins); i++) {
    gpio = addr_pins[i];
    addr_mask |= (1 << gpio);
    gpio_init(gpio);
    gpio_set_function(gpio, GPIO_FUNC_SIO);
    gpio_set_dir(gpio, GPIO_IN);
  }

  for (i = 0; i < NELEMS(data_pins); i++) {
    gpio = data_pins[i];
    data_mask |= (1 << gpio);
    gpio_init(gpio);
    gpio_set_function(gpio, GPIO_FUNC_SIO);
    gpio_set_dir(gpio, GPIO_IN);
  }

  for (i = 0; i < NELEMS(ctrl_pins); i++) {
    gpio = ctrl_pins[i];
    gpio_init(gpio);
    gpio_set_function(gpio, GPIO_FUNC_SIO);
    gpio_set_dir(gpio, GPIO_IN);
  }
}

uint32_t getTotalHeap(void) {
  extern char __StackLimit, __bss_end__;
  return &__StackLimit - &__bss_end__;
}

uint32_t getFreeHeap(void) {
  struct mallinfo m = mallinfo();
  return getTotalHeap() - m.uordblks;
}
