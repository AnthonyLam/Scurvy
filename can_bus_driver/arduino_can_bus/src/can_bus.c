#include "can_bus.h"

// Mailbox commands
#define RESET       0xFF
#define READ        0xFE
#define WRITE       0xFD
#define BIT_MODIFY  0xFC
#define READ_STATUS 0xFB
#define GET_MESSAGE 0xEF
#define SEND_MESSAGE 0xEE
#define CHECK_MESSAGE 0xED


void read_mailbox_message(tCAN *message)
{
    uint32_t word;
    word = MAILBOX_DATA(0);
    message->id = (word >> 24) & 0xFF;
    message->id |= (word >> 16) & 0xFF;
    message->header.rtr = (word >> 8) & 0xFF;
    message->header.length = word;

    for(int i=1; i>=0; --i) {
        word = MAILBOX_DATA(i+1);
        message->data[0+4*i] = (word >> 24) & 0xFF;
        message->data[1+4*i] = (word >> 16) & 0xFF;
        message->data[2+4*i] = (word >> 8) & 0xFF;
        message->data[3+4*i] = word;
    }
}

void write_mailbox_message(tCAN *message)
{
    uint8_t word[4];
    // Assume big-endian right hurr
    word[3] = (message->id>>8) & 0xFF; 
    word[2] = (message->id) & 0xFF;
    word[1] = message->header.rtr;
    word[0] = message->header.length; 

    MAILBOX_DATA(0) = *(uint32_t *)word; 

    for(int i=0; i<2; ++i) {
        word[3] = message->data[0+4*i];
        word[2] = message->data[1+4*i];
        word[1] = message->data[2+4*i];
        word[0] = message->data[3+4*i];
        MAILBOX_DATA(i+1) = *(uint32_t *)word;
    }
}

int main() {
    // Configure SPI to idle low, and sample on rising edges
    arduino_init(0,0,0,0);
    config_arduino_switch(A_GPIO,A_GPIO,A_GPIO,
                          A_GPIO,A_GPIO,A_GPIO,
                          D_UART,
                          D_GPIO,D_GPIO,D_GPIO,
                          D_GPIO,D_GPIO,D_GPIO,
                          D_GPIO,D_GPIO,D_SS,
                          D_MOSI,D_MISO,D_SPICLK);
    uint8_t command;
    tCAN *message;

    while(1) {
        // Command address is empty (ie no commands)
        // We might want to actually do some work while idle
        while(MAILBOX_CMD_ADDR == 0x00);

        command = MAILBOX_CMD_ADDR;
        switch(command) {
            case RESET:
                mcp2515_reset(MAILBOX_DATA(0));
                break;
            case READ:
                MAILBOX_DATA(0) = mcp2515_read_register(MAILBOX_DATA(0));
                break;
            case WRITE:
                mcp2515_write_register(MAILBOX_DATA(0),MAILBOX_DATA(1));
                break;
            case BIT_MODIFY:
                mcp2515_bit_modify(MAILBOX_DATA(0),MAILBOX_DATA(1),MAILBOX_DATA(2));
                break;
            case READ_STATUS:
                MAILBOX_DATA(0) = mcp2515_read_status(MAILBOX_DATA(0));
                break;
            case GET_MESSAGE:
                message = malloc(sizeof(tCAN));
                write_mailbox_message(message);
                mcp2515_get_message(message);
                free(message);
                break;
            case SEND_MESSAGE:
                message = malloc(sizeof(tCAN));
                read_mailbox_message(message);
                mcp2515_send_message(message);
                free(message);
                break;
            case CHECK_MESSAGE:
                // TODO Check message here
                // XGpio_WriteData();
            default:
                break;
        }
        // Reset mailbox command to a finished state
        MAILBOX_CMD_ADDR = 0x00;

    }
}