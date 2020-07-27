//--------------------------------------------------ABC application-----------------------------------------
#ifndef _ABC_APP_C_
#define _ABC_APP_C_

#include <appconst.h>

//--------------------------------------------function prototypes---------------------------------------------------
static void button_LED_init(void);
void fram_address_display(void);
void spi_address_display(spi_data_t *spi_data) ;
void button_address_assign (led_button_t *led_button, spi_data_t *spi_data);
void led_quick_flash (led_button_t *led_button);
void button_address_activate (spi_data_t *spi_data);
//--------------------------------------------function prototypes---------------------------------------------------


static void button_LED_init(void) //initial button and LED io port
{
	ioport_set_pin_dir(FE_LED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(FE_LED1, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(FE_BUTTON, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(FE_BUTTON, IOPORT_MODE_PULLUP);
	ioport_set_pin_dir(FE_SWITCH1, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(FE_SWITCH1, IOPORT_MODE_PULLUP);
	ioport_set_pin_dir(FE_SWITCH2, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(FE_SWITCH2, IOPORT_MODE_PULLUP);
	
	#define FE_LED    IOPORT_CREATE_PIN(PORTE, 4) //LED0 port
	#define FE_LED1    IOPORT_CREATE_PIN(PORTE, 5) //LED1 port
	#define FE_BUTTON IOPORT_CREATE_PIN(PORTF, 4) //button(switch0) port
	#define FE_SWITCH1 IOPORT_CREATE_PIN(PORTF, 6) //switch1 port
	#define FE_SWITCH2 IOPORT_CREATE_PIN(PORTF, 7)
	
	
	PORTE.INT0MASK = 0x30;		//mask PortE pin (LEDs) to be written
	PORTF.INTCTRL = 0x02;		//(button port) disable INT1, enable INT0 and place it to medium priority
	PORTCFG.MPCMASK = 0xD0;		
	PORTF.PIN4CTRL = 0x01;		//config PortF to response on interrupt on rising edge
	PORTF.PIN6CTRL = 0x01;		
	PORTF.PIN7CTRL = 0x01;		
	PMIC.CTRL = 0x07;			//enable all level of interrupts

}

void fram_address_display(void) // display FE address of NVM on LED with ABC
{
	uint8_t read_address_page[EEPROM_PAGE_SIZE];
	
	nvm_eeprom_read_buffer(ADDRESS_PAGE_ADDR, read_address_page, EEPROM_PAGE_SIZE);
	
	if(read_address_page[ADDRESS_BYTE] <= MAX_ADDRESS){ //prevent default memory
		for(int i = 0; i < read_address_page[ADDRESS_BYTE]; i++) { // a flash LED Address-many time
			delay_ms(LED_BRINK_TIME_MS); //wait for brink time
			ioport_set_pin_level(FE_LED, !(LED_IDLE)); //set LED brink
			if (ioport_get_pin_level(FE_BUTTON)) break; // quit loop if button pressed
			delay_ms(LED_BRINK_TIME_MS); //wait for brink time
			ioport_set_pin_level(FE_LED, LED_IDLE); //set LED brink
			if (ioport_get_pin_level(FE_BUTTON)) break; // quit loop if button pressed
		}
	}
}

void spi_address_display(spi_data_t *spi_data) // display FE address of NVM on LED with ABC
{
	for(int i = 0; i < spi_data->fe_address; i++) { // a flash LED Address-many time
		delay_ms(LED_BRINK_TIME_MS); //wait for brink time
		ioport_set_pin_level(FE_LED, !(LED_IDLE)); //set LED brink
		if (ioport_get_pin_level(FE_BUTTON)) break; // quit loop if button pressed
		delay_ms(LED_BRINK_TIME_MS); //wait for brink time
		ioport_set_pin_level(FE_LED, LED_IDLE); //set LED brink
		if (ioport_get_pin_level(FE_BUTTON)) break; // quit loop if button pressed
	
	}
}

void button_address_assign (led_button_t *led_button, spi_data_t *spi_data) {
	
	int long_button_cnt = 0; //counting button press time to distinguish long click
	
	while (led_button->button_pressed) {
		// Detect long click by a loop count if the button is detected to be high for all MAX_LONG_BUTTON_COUNT number of loops, each loop separated by
		// MAX_LONG_LOOP_DELAY number of msec, the pressing action is registered as a "long press".
		led_button->button_pressed = ioport_get_pin_level(FE_BUTTON); // reload button status
		delay_ms(MAX_LONG_LOOP_DELAY);// delay between the loop long button timing count
		
		if (long_button_cnt < MAX_LONG_BUTTON_COUNT) long_button_cnt++; // long button timing no yet reach, count + 1
		else { // long button timing reached
			if (led_button->button_enable_address) {
				fram_write_address((led_button->i_address_cnt)-1, spi_data); //input address to flash ram if in address mode
				spi_data->fe_address = (led_button->i_address_cnt)-1;  //assign address to ram
			}
			if (!(led_button->button_enable_address)) led_button->i_address_cnt = 0; // reset button address counter if in idle mode

			led_button->changed_mode = true; // indicate a change in address programming mode

			led_button->button_enable_address = !(led_button->button_enable_address); // flip address programming mode
			led_button->led_state = !(led_button->led_state); // flip LED state
			ioport_set_pin_level(FE_LED, led_button->led_state); // output LED state
			
			while(led_button->button_pressed) led_button->button_pressed = ioport_get_pin_level(FE_BUTTON); //on-hold the program until button release
		}		
	}
}

void led_quick_flash (led_button_t *led_button){
	ioport_set_pin_level(FE_LED, !(led_button->led_state)); //make button flash for every click except long click
	delay_ms(LED_FLASH_TIME_MS);
	ioport_set_pin_level(FE_LED, (led_button->led_state));
}

void button_address_activate (spi_data_t *spi_data) //assign FE address to NVM with ABC
{
	led_button_t led_button; //define and initialize button and LED variables
	led_button.button_pressed = true; // button status one loop before
	led_button.button_previous_state_pressed = false; // button status one loop before
	led_button.button_enable_address = false; //address input mode enable
	led_button.led_state = LED_IDLE;//bool led_state = LED_IDLE; //current LED state
	led_button.i_address_cnt = 0; //storing button count before setting the address 


	while(led_button.button_pressed || led_button.button_enable_address){
		
		led_button.changed_mode = false; //indicate if a mode has change (optional)
		
		ioport_set_pin_level(FE_LED, led_button.led_state); //define LED initial state
		led_button.button_pressed = ioport_get_pin_level(FE_BUTTON); // get button status

		if (led_button.button_previous_state_pressed != led_button.button_pressed) { //start debouncing
			delay_ms(DEBOUNCE_DELAY_MS);
			led_button.button_previous_state_pressed = led_button.button_pressed; // check if button was pressed before delay
					
			led_button.button_pressed = ioport_get_pin_level(FE_BUTTON); // reload button status
			if (led_button.button_pressed) { // end debouncing
				if(led_button.i_address_cnt < MAX_ADDRESS) led_button.i_address_cnt++; // all clicks imply frontend address + 1
				button_address_assign(&led_button, spi_data); //determine whether to enter address mode
				if(!(led_button.changed_mode) && led_button.button_enable_address) led_quick_flash (&led_button);//to avoid the LED flashing once after long click		
				else if(!(led_button.changed_mode) && !(led_button.button_enable_address)) spi_address_display(spi_data); // flash the address using ABC
			}

		}
	}
}

#endif


//--------------------------------------------------ABC application-----------------------------------------