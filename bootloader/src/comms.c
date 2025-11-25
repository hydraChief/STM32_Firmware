#include "comms.h"
#include "common-defines.h"
#include "core/uart.h"
#include "core/crc8.h"
#include "core/ring_buffer.h"

#define PACKET_BUFFER_LENGTH  (16)

// ring buffer setup for buffering packets
static ring_buffer_t packet_buffer;
static comms_packet_t* buffer;
// Comms state machine states
typedef  enum comms_state_t{
    CommsState_Length,
    CommsState_Data,
    CommsState_CRC,
} comms_state_t;

static comms_state_t state= CommsState_Length;
static uint8_t data_byte_count=0;

static comms_packet_t temporary_packet= { .length=0, .data={0}, .crc=0};
static comms_packet_t retx_packet = { .length=0, .data={0}, .crc=0};
static comms_packet_t ack_packet = { .length=0, .data={0}, .crc=0};
static comms_packet_t last_transmitted_packet= { .length=0, .data={0}, .crc=0};

static bool comms_is_single_byte(const comms_packet_t* packet, uint8_t byte){
    if(packet->length!=1 || packet->data[0]!=byte)return false;
    for(int i=1;i<PACKET_DATA_LENGTH;i++){
        if(packet->data[i]!=0xff)return false;
    }
    return true;
}

void comms_setup(void){
    // setup ring buffer for packets
    ring_buffer_setup(&packet_buffer, PACKET_BUFFER_LENGTH, sizeof(comms_packet_t), buffer);
    //re-transfer packet setup
    retx_packet.length=1;
    retx_packet.data[0]=PACKET_RETX_DATA0;
    for(int i=1;i<PACKET_DATA_LENGTH;i++){
        retx_packet.data[i]=0xff;
    }
    retx_packet.crc=comms_compute_crc(&retx_packet);
    // ack packet setup
    ack_packet.length=1;
    ack_packet.data[0]=PACKET_ACK_DATA0;
    for(int i=1;i<PACKET_DATA_LENGTH;i++){
        ack_packet.data[i]=0xff;
    }
    ack_packet.crc=comms_compute_crc(&ack_packet);
}

void comms_update(void){
    while(uart_data_available()){
        switch(state){
            case CommsState_Length: {
                temporary_packet.length=uart_read_byte();
                state=CommsState_Data;
            } break;

            case CommsState_Data: {
                if(data_byte_count< temporary_packet.length){
                    temporary_packet.data[data_byte_count]=uart_read_byte();
                    data_byte_count++;
                }else {
                    // write remaining bytes as 0xff in packet data
                    for(int i=data_byte_count;i<PACKET_DATA_LENGTH;i++){
                        temporary_packet.data[i]=0xff;
                    }
                    state= CommsState_CRC;
                    data_byte_count=0;
                }
            } break;

            case CommsState_CRC: {
                // check crc if not correct send retx packet
                temporary_packet.crc=uart_read_byte();
                if(comms_compute_crc(&temporary_packet)!=temporary_packet.crc){
                    comms_write(&retx_packet);
                    state=CommsState_Length; 
                    break;
                }
                //check if packet is retx packet
                if(comms_is_single_byte(&temporary_packet, PACKET_RETX_DATA0)){
                    comms_write(&last_transmitted_packet);
                    state= CommsState_Length;
                    break;
                }

                //check if ack packet
                if(comms_is_single_byte(&temporary_packet, PACKET_ACK_DATA0)){
                    state= CommsState_Length;
                    break;
                }

                last_transmitted_packet=temporary_packet;// store last transmitted packet for retx
                // push packet to buffer
                ring_buffer_write(&packet_buffer, &temporary_packet);
                comms_write(&ack_packet);
                state=CommsState_Length;
            } break;

            default: {
                state= CommsState_Length;
            }
        }
    }
}


bool comms_packet_available(void){
    return !ring_buffer_empty(&packet_buffer);
}

void comms_read(comms_packet_t* packet){
    ring_buffer_read(&packet_buffer, packet);
}

void comms_write(comms_packet_t* packet){
    uart_write((uint8_t*)(packet), PACKET_LENGHT);// here the cast is legal and safe as the packet is contiguous in memory, and by casting we have address of first byte of the struct
}

uint8_t comms_compute_crc(comms_packet_t* packet){
    return crc8((uint8_t*)&packet, PACKET_LENGHT-PACKET_CRC_BYTES);
}
