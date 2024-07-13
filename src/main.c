/*
 * sam4lc8c4 template.c
 *
 * Created: 7/12/2024 5:15:15 PM
 * Author : budali11
 */ 


#include "sam4lc8c.h"
#include "sysclk.h"
#include "usart.h"
#include "ioport.h"
#include "conf_clock.h"
#include "interrupt.h"

Usart *REB215_usart = USART0;
uint32_t REB215_io_rx = IOPORT_CREATE_PIN(IOPORT_GPIOB, 0);
uint32_t REB215_io_tx = IOPORT_CREATE_PIN(IOPORT_GPIOB, 1);

void delay(uint32_t n)
{
  for (uint32_t i = 0; i < n; i++) {
    for (uint32_t j = 0; j < 1000; j++) {
      nop();
    }
  }
}


int main(void)
{
  /* Initialize the SAM system */
  SystemInit();

  /* Initialize system */
  sysclk_init();
  NVIC_EnableIRQ(USART0_IRQn);

  /* led */
  sysclk_enable_peripheral_clock(GPIO);
  ioport_set_pin_dir(LED_0_PIN, IOPORT_DIR_OUTPUT);

  /* init io ports */
  /* usart0 */
  ioport_set_pin_mode(REB215_io_rx, MUX_PB00B_USART0_RXD);
  ioport_set_pin_mode(REB215_io_tx, MUX_PB01B_USART0_TXD);

  /* disable the GPIO to control the pin */
  ioport_disable_pin(REB215_io_rx);
  ioport_disable_pin(REB215_io_tx);

  /* enable usart0 clock */
  sysclk_enable_peripheral_clock(REB215_usart);

  /* init usart to normal mode */
  sam_usart_opt_t usart_init_opt = {
    .baudrate = 115200,
    .stop_bits = US_MR_NBSTOP_1_BIT,
    .char_length = US_MR_CHRL_8_BIT,
    .parity_type = US_MR_PAR_NO,
    .channel_mode = US_MR_CHMODE_NORMAL,
  };
  usart_init_rs232(REB215_usart, &usart_init_opt, sysclk_get_peripheral_bus_hz(REB215_usart));

  /* enable usart transmitter and receiver */
  usart_enable_tx(REB215_usart);
  usart_enable_rx(REB215_usart);

  /* config for usart receive interrupt */
  usart_enable_interrupt(REB215_usart, US_IER_RXRDY);

  /* Replace with your application code */
  char msg[] = "hello iot.";
  while (1) 
  {
    delay(100);
    ioport_set_pin_level(LED_0_PIN, IOPORT_PIN_LEVEL_HIGH);
    delay(100);
    ioport_set_pin_level(LED_0_PIN, IOPORT_PIN_LEVEL_LOW);
  }
}

void USART0_Handler(void)
{
  static char buffer[16] = {0};
  static char* pb = buffer;
  if ((usart_get_status(REB215_usart) & US_CSR_RXRDY) == 1){
    // receive interrupt
    usart_getchar(REB215_usart, pb);
    pb = (pb == buffer+15)? buffer: pb+1;
    if (pb == buffer) {
      for (int i = 0; i < 16; i++) 
        usart_putchar(REB215_usart, buffer[i]);
    }
  }
}

