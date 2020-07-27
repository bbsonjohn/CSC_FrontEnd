//-----------------------------------------------Flash RAM application----------------------------------------------
#ifndef _FRAM_APP_C_
#define _FRAM_APP_C_

#include <appconst.h>

#define BYTE_TO_BITS 256 // conversion 2^8 = 256

//--------------------------------------------function prototypes---------------------------------------------------
void spi_load_address(spi_data_t *spi_data);
void fram_write_address (int i_fe_address, spi_data_t *spi_data);
void fram_write_pwm (pwm_config_list_t *pwm_config_list);
void fram_load_pwm (pwm_config_list_t *pwm_config_list);
//--------------------------------------------function prototypes---------------------------------------------------

void spi_load_address(spi_data_t *spi_data) //load address from NVM to program RAM
{
	uint8_t read_address_page[EEPROM_PAGE_SIZE]; //define buffer size

	read_address_page[ADDRESS_BYTE] = 0; //set default buffer to avoid error
	nvm_eeprom_read_buffer(ADDRESS_PAGE_ADDR, read_address_page, EEPROM_PAGE_SIZE); // read from address to address page buffer
	if(read_address_page[ADDRESS_BYTE] < MAX_ADDRESS) spi_data->fe_address = read_address_page[ADDRESS_BYTE]; // write buffer page to spi_data object
	else spi_data->fe_address = 0; //prevent default FF address at start-up
	
}


void fram_write_address (int i_fe_address, spi_data_t *spi_data){ //write address to NVM
	
	uint8_t write_address_page[EEPROM_PAGE_SIZE];

	write_address_page[ADDRESS_BYTE] = i_fe_address; // write the whole address page with 1 byte of address

	nvm_eeprom_load_page_to_buffer(write_address_page); // load address byte to buffer
	nvm_eeprom_atomic_write_page(ADDRESS_PAGE); // write address from buffer to NVM
	
	delay_ms(FRAM_WRITE_DELAY_MS); // wait until write finish
	
	spi_load_address(spi_data);  //load address from nvm to program
	
}


void fram_write_pwm (pwm_config_list_t *pwm_config_list){ // write PWM to NVM
	
	uint8_t write_address_page[EEPROM_PAGE_SIZE];
	
	for(int i = 0; i < PWM_NUMBER_MAX; i++) {
	write_address_page[i*PWM_BYTE] = pwm_config_list->pwm_duty_cycle[i]; // iteratively write all pwm duty cycle to buffer page
	}

	nvm_eeprom_load_page_to_buffer(write_address_page); // load pwm buffer oage byte to buffer
	nvm_eeprom_atomic_write_page(PWM_PAGE); // write buffer page from buffer to NVM
	
	delay_ms(FRAM_WRITE_DELAY_MS); // wait until write finish
	
}

void fram_load_pwm (pwm_config_list_t *pwm_config_list){ // load pwm from NVM
	uint8_t read_pwm_page[EEPROM_PAGE_SIZE]; //define buffer for pwm read from NVM

	for(int i=1; i<=PWM_NUMBER_MAX; i++) read_pwm_page[i*PWM_BYTE] = 0; //write the buffer page to all zeros
	nvm_eeprom_read_buffer(PWM_PAGE_ADDR, read_pwm_page, EEPROM_PAGE_SIZE); //read from NVM and store to buffer
	for(int i=1; i<=PWM_NUMBER_MAX; i++) pwm_config_list->pwm_duty_cycle[i] = read_pwm_page[PWM_BYTE*i]; //load buffer to pwm ram
}

#endif
//-----------------------------------------------Flash RAM application----------------------------------------------