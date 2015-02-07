#include <pololu/orangutan.h>
/* Giovanni Galasso
 * SENG5831 Spring 2015
 * HW 2
 * 
 * Spent alot of time trying to figure out how to manipulate the bits. I need to figure
 * out some things in that regard still.
 *
 * The timer functionality is kind of a work in progress. I'm sure there is a better way 
 * to do it, especially when you start adjusting it with input.
 *
 *
 */

//Taken from serial 2 program.
char receive_buffer[32];
unsigned char receive_buffer_position = 0;
char send_buffer[32];

unsigned long interval = 1000; //Initial blinking interval
unsigned long timer = 1000; //Timer value to keep track of next blink
static long const interval_min = 100; //Min blinking interval
static long const interval_max = 3000; //Max blinking interval
static long const interval_step = 100; //+ or - adjust interval by this amount

//Check if the yellow LED is on (128) or off (0)
int get_yellow() {
	int yellow_value;
	yellow_value = PORTA & (1 << 7);
	return yellow_value;
} 
//Check if the green LED is one (16) or off (0)
int get_green() {
	int green_value;
	green_value = PORTA & (1 << 4);
	return green_value;
}

void process_received_byte(char byte)
{
    clear();		// clear LCD
    print("RX: ");
    print_character(byte);
    lcd_goto_xy(0, 1);	// go to start of second LCD row
    
    switch(byte)
    {
    //Decrease the blink interval
	case '-':
            decrease_interval();
	    	break;
    //Increase the blink interval
	case '+':
            increase_interval();
           	 break;

    default:
            wait_for_sending_to_finish();
            send_buffer[0] = byte ^ 0x20;
            serial_send(USB_COMM, send_buffer, 1);
            print("TX: ");
            print_character(send_buffer[0]);
            break;
    }
}

void check_for_new_bytes_received() {
    while(serial_get_received_bytes(USB_COMM) != receive_buffer_position) {
        // Process the new byte that has just been received.
        process_received_byte(receive_buffer[receive_buffer_position]);
        
        // Increment receive_buffer_position, but wrap around when it gets to
        // the end of the buffer.
        if (receive_buffer_position == sizeof(receive_buffer)-1) {
            receive_buffer_position = 0;
        }
        else {
            receive_buffer_position++;
        }
    }
}

void wait_for_sending_to_finish() {
    while(!serial_send_buffer_empty(USB_COMM)) {
        serial_check();		// USB_COMM port is always in SERIAL_CHECK mode
    }
}

void increase_interval() {
    if (interval < interval_max) {
        //timer += interval_step;
        interval += interval_step;
        if (interval > interval_max) {
            interval = interval_max;
        }
        clear();
        print("Slowing blink speed");
    }

}

void decrease_interval() {
    if (interval > interval_min) {
        //timer = timer - interval_step;
        interval = interval - interval_step;
        if (interval < interval_min) {
            interval = interval_min;
        }
        clear();
        print("Increasing blink");
    }
}


int main()
{
    
    lcd_init_printf();
    //Ended up not using the defines
    #define BOTH_ON 0x90
    #define GREEN_ON_YELLOW_OFF 0x10
    #define YELLOW_ON_GREEN_OFF 0x80
    #define BOTH_OFF 0x00
    //Initialize three ints to hold state of the buttons.
    int top_but = 0, mid_but = 0, bot_but = 0;
    
    DDRA |= 1 << 7 | 1 << 4;
    //PORTA |= 1 << 4; //Flip 4 on
    //PORTA |= 1 << 7; //Flip 7 on
    serial_set_baud_rate(USB_COMM, 9600);
    serial_receive_ring(USB_COMM, receive_buffer, sizeof(receive_buffer));
    while(1)
    {
        unsigned char button = get_single_debounced_button_press(ANY_BUTTON);
        unsigned long curTime = get_ms();
        
        // USB_COMM is always in SERIAL_CHECK mode, so we need to call this
        // function often to make sure serial receptions and transmissions
        // occur.
        serial_check();
        
        // Deal with any new bytes received.
        check_for_new_bytes_received();
        
        //Toggle the top button.
        if (button & TOP_BUTTON) {
            if (top_but == 0) {
                lcd_goto_xy(0,0);
                clear();
                print("Top Light Off");
                top_but = 1;
                PORTA &= ~(1 << 7); //Turn bit 7 off
            } else if (top_but == 1) {
                lcd_goto_xy(0,0);
                clear();
                print("Top Light On");
                top_but = 0;
            }
        
        //Toggle the bottom button.
        } else if (button & BOTTOM_BUTTON) {
            if (bot_but == 0) {
                lcd_goto_xy(0,0);
                clear();
                print("Bot Light Off");
                bot_but = 1;
                PORTA &= ~(1 << 4); //Turn bit 4 off
            } else if (bot_but == 1) {
                lcd_goto_xy(0,0);
                clear();
                print("Bot Light On");
                bot_but = 0;
            }
        }
        //Print to screen status of the timer and the current time.
        lcd_goto_xy(0,0);
        printf("Current %lu", curTime);
        lcd_goto_xy(0,1);
        printf("Timer %lu", timer);

        if (timer < curTime) {
            clear();
            timer += interval;

            if (top_but == 0 && (get_yellow() == 128)) {
                PORTA &= ~(1 << 7); //7 OFF
            } else if (top_but == 0 && (get_yellow() == 0)) {
                PORTA |= 1 << 7; //7 ON
            }
            
            if (bot_but == 0 && (get_green() == 16)) {
                PORTA &= ~(1 << 4); //4 OFF
            } else if (bot_but == 0 && (get_green() == 0)) {
                PORTA |= 1 << 4; //4 ON
            }
            
        }
        //PINA 
        //printf("%d", PORTD1);
        //PORTD1 = 1;
        //#define LED_MASK = (1 << PORTD);	
        //LED_MASK = 0;
    }
}
