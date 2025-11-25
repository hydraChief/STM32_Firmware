#include<libopencm3/stm32/rcc.h>
#include<libopencm3/cm3/nvic.h>
#include<libopencm3/stm32/usart.h>

#include "core/uart.h"
#include "core/ring_buffer.h"

#define UART_BAUDRATE (115200)
#define RING_BUFFER_SIZE (64)

static uint8_t buffer;
static ring_buffer_t rb = {0U};

void usart2_isr(void){
    const bool overrun_occured=usart_get_flag(USART2, USART_SR_ORE)==1;
    const bool received_data=usart_get_flag(USART2, USART_FLAG_RXNE)==1;
    if(overrun_occured || received_data){
        if(ring_buffer_write(&rb, &(uint8_t){usart_recv(USART2)})){
            //can't return the error or anything even if success!!
        }
    }
}

void uart_setup(void){
    ring_buffer_setup(&rb, RING_BUFFER_SIZE, sizeof(buffer), &buffer);

    rcc_periph_clock_enable(RCC_USART2);

    usart_set_mode(USART2, USART_MODE_TX_RX);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
    usart_set_databits(USART2, 8);
    usart_set_baudrate(USART2, UART_BAUDRATE);
    usart_set_parity(USART2, 0);
    usart_set_stopbits(USART2, 1);

    usart_enable_rx_interrupt(USART2);
    nvic_enable_irq(NVIC_USART2_IRQ);

    usart_enable(USART2);
}

void uart_write(uint8_t* data, const uint32_t length){
    for (uint32_t i=0; i<length; i++){
        uart_write_byte((uint16_t)data[i]);
    }
}

void uart_write_byte(uint8_t data){
    usart_send_blocking(USART2, (uint16_t)data);
}

uint8_t uart_read(uint8_t *data, const uint32_t length){
    if (length>0 ){
        for(uint32_t i=0; i<length; i++){
            if(!ring_buffer_read(&rb, (uint32_t*)&data[i])){
                return i;
            }
        }
        return length;
    }
    return 0;
}

uint8_t uart_read_byte(void){
    uint8_t data;
    (void)uart_read(&data, 1);
    return data;
}

bool uart_data_available(void){
    return !ring_buffer_empty(&rb);
}