#include <pololu/orangutan.h>

/* Giovanni Galasso
 * SENG5831 Spring 2015
 * 
 * This program became a little bigger than expected. I'm 
 * sure there is a better way to do this, but my lack of 
 * knowledge with C made me choose the simplest solution.
 * One thing I learned was that I needed to deactivate a light
 * after the button press, otherwise it would get caught in it's
 * current state, so in some cases would remain on solid. 
 * 
 * I couldn't get the printf statements to work as expected, as 
 * I wanted to know the value of a button at a given time, but %c 
 * yielded an empty result. What's up with that?
 * 
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

int main()
{
  //Initialize three ints to hold state of the buttons. 
  int top_button = 0, middle_button = 0,  bot_button = 0;
  clear();	
  lcd_init_printf();
  while(1) 
  {	
	//Check for any button press.
	unsigned char button = get_single_debounced_button_press(ANY_BUTTON);
	//If top button is pressed switch the state of it
	if(button & TOP_BUTTON) {
		if (top_button ==  1) {
			top_button = 0;
			clear();
			printf("Green0 = %d", button);
		} else if(top_button == 0) {
			top_button = 1;
			green_led(0);
			clear();
			printf("Green1 = %d", button);
		}
	//If bottom button is pressed switch it's state
	} else if(button & BOTTOM_BUTTON) {
		if (bot_button == 1) {
			bot_button = 0;
			clear();
			printf("Red0 = %d", button);
		} else if (bot_button == 0) {
			bot_button = 1;
			red_led(0);
			clear();
			printf("Red1 = %d", button);
		}
	//Middle button is pressed
	} else if(button & MIDDLE_BUTTON) {
		//If both buttons haven't been pressed deactivate both lights
		if (top_button == 0 && bot_button == 0) {
			top_button = 1;
			green_led(0);
			bot_button = 1;
			red_led(0);
		//If both buttons are deactivated than activate both lights
		} else if (top_button == 1 && bot_button == 1) {
			top_button = 0;
			bot_button = 0;
		//Top button has been pressed but bottom hasn't
		} else if (top_button == 1 && bot_button == 0) {
			top_button = 0;
			bot_button = 1;
			red_led(0);
		//Bottom button has been pressed but top hasn't
		} else if (top_button == 0 && bot_button == 1) {
			top_button = 1;
			green_led(0);
			bot_button = 0;
		}
	}
	//Toggle the LEDs of the green or red lights depending on
	//whether top or bottom buttons have been pressed.
	if(top_button == 0) {
		green_led(TOGGLE);
		delay_ms(100);
	}
	if(bot_button == 0) {
		red_led(TOGGLE);
		delay_ms(100);
	}
  }
}
 
