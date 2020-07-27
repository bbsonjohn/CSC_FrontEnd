//----------------------------------------------SPI application----------------------------------------------------
#ifndef _SPI_APP_C_
#define _SPI_APP_C_

#include <appconst.h>
#include <fram_app.c>
#include <adc_app.c>
#include <dac_app.c>

//--------------------------------------------function prototypes---------------------------------------------------
void spi_init_pins(void);
static inline void spi_byte_wait(void);
void spi_get_be_address(spi_data_t *spi_data);
void read_serial(uint8_t *device_serial);
void spi_serial_out(uint8_t *device_serial);
void spi_write_pwm (pwm_config_list_t *pwm_config_list);
void spi_read_pwm (pwm_config_list_t *pwm_config_list);
void spi_read_adc_state (adc_status_t *adc_status);
void spi_read_adc_threshold (adc_status_t *adc_status);
void spi_write_dac_pulse(int dac_channel, dac_pulse_t *dac_pulse);
void spi_echo (void);
void spi_echo_2 (void); 
void spi_read(spi_data_t *spi_data, uint8_t *device_serial, pwm_config_list_t *pwm_config_list, adc_status_t *adc_status, dac_pulse_t *dac_pulse);
//--------------------------------------------function prototypes---------------------------------------------------


void spi_init_pins(void) // initialize SPI io pin
{
	ioport_configure_port_pin(&PORTD, PIN4_bm, IOPORT_PULL_DOWN | IOPORT_DIR_INPUT); // SPI SSbar
	ioport_configure_port_pin(&PORTD, PIN5_bm, IOPORT_DIR_INPUT); // SPI MOSI
	ioport_configure_port_pin(&PORTD, PIN6_bm, IOPORT_INIT_LOW | IOPORT_DIR_OUTPUT); // SPI MISO
	ioport_configure_port_pin(&PORTD, PIN7_bm, IOPORT_DIR_INPUT); // SPI SCK
	ioport_configure_port_pin(&PORTC, PIN6_bm, IOPORT_DIR_INPUT); // FE Address PE4
	ioport_configure_port_pin(&PORTC, PIN7_bm, IOPORT_DIR_INPUT); // FE Address PE5
	
	sysclk_enable_module(SYSCLK_PORT_D, PR_SPI_bm);// enable peripheral clock for SPI and Port D

	SPID.CTRL = 0x80;	// enable SPI, set most significant bit first, set clock transfer rate
	SPID.CTRL += SPI_MODE << 2; // set SPI mode;
	SPID.INTCTRL = 0x02; // enable interrupt for SPI

	spi_disable(&SPID); //disable until further A_FE notification from BE
	
	PORTE.INT0MASK = 0x30;		//mask PortE pin to be written
	PORTE.INTCTRL = 0x02;		//disable INT1, enable INT0 and place it to medium priority
	PORTCFG.MPCMASK = 0x30;		//configure PortE pin
	PORTE.PIN4CTRL = 0x00;		//config PortE pin4 to response on interrupt on both edges
	PORTE.PIN5CTRL = 0x00;		//config PortE pin4 to response on interrupt on both edges
	PMIC.CTRL = 0x07;			//enable all level of interrupts
}

static inline void spi_byte_wait(void){ // wait until master write spi
	for(int j = 0; j < SPI_BYTE_WAIT_US; j++){
		if(!(SPID.STATUS & SPI_IF_bm)) delay_us(SPI_DELAY_US);// wait unit an interrupt flag indicating a message has arrived from BE
		else break;
	}
}


void spi_get_be_address(spi_data_t *spi_data){ //get and store address from address line and enable/disable SPI

	spi_data->be_address = ioport_get_pin_level(SPI_A_FE1)*2 + ioport_get_pin_level(SPI_A_FE0); //read address and convert to 2-bit register

	if(spi_data->fe_address == spi_data->be_address) {
		spi_enable(&SPID);  // enable spi if address line match FE NVM address
	}
	else spi_disable(&SPID); // disable otherwise
}

void read_serial(uint8_t *device_serial){ // read 11 byte microcontroller serial fuse and store it in vector device_serial
	
	device_serial[0] = nvm_read_production_signature_row(
	nvm_get_production_signature_row_offset(LOTNUM0));
	device_serial[1] = nvm_read_production_signature_row(
	nvm_get_production_signature_row_offset(LOTNUM1));
	device_serial[2] = nvm_read_production_signature_row(
	nvm_get_production_signature_row_offset(LOTNUM2));
	device_serial[3] = nvm_read_production_signature_row(
	nvm_get_production_signature_row_offset(LOTNUM3));
	device_serial[4] = nvm_read_production_signature_row(
	nvm_get_production_signature_row_offset(LOTNUM4));
	device_serial[5] = nvm_read_production_signature_row(
	nvm_get_production_signature_row_offset(LOTNUM5));
	device_serial[6]  = nvm_read_production_signature_row(
	nvm_get_production_signature_row_offset(WAFNUM));
	device_serial[7] = nvm_read_production_signature_row(
	nvm_get_production_signature_row_offset(COORDX0));
	device_serial[8] = nvm_read_production_signature_row(
	nvm_get_production_signature_row_offset(COORDX1));
	device_serial[9] = nvm_read_production_signature_row(
	nvm_get_production_signature_row_offset(COORDY0));
	device_serial[10] = nvm_read_production_signature_row(
	nvm_get_production_signature_row_offset(COORDY1));
}

void spi_serial_out(uint8_t *device_serial) { // spi sed out serial number of microcontroller
	uint8_t data_buffer = 0;
	for (int i = 0; i < SPI_SERIAL_FRAMESIZE; i++) {
		SPID.DATA = device_serial[i]; //read 1 byte serial
		spi_byte_wait();  // wait for master's write spi
		data_buffer = SPID.DATA; // reset spi IF flag
		delay_us(SPI_DELAY_US); // safety delay
	}
}

void spi_write_pwm (pwm_config_list_t *pwm_config_list) { //read from spi the pwm threshold and write it to ram
	for (int i = 0; i < SPI_READ_PWM_FRAMESIZE; i++) { 
		spi_byte_wait(); // wait for master's write spi
		pwm_config_list->pwm_duty_cycle[i] = SPID.DATA; //set pwm duty cycle in ram
		SPID.DATA = 0; // output 0 to master as a acknowledgment of data received
	}
}

void spi_read_pwm (pwm_config_list_t *pwm_config_list) { //output pwm threshold with spi
	uint8_t data_buffer = 0;
	for (int i = 0; i < SPI_WRITE_PWM_FRAMESIZE; i++) {
		SPID.DATA = pwm_config_list->pwm_duty_cycle[i]; //output pwm duty cycle from ram to spi
		spi_byte_wait(); // wait for master's write spi
		data_buffer = SPID.DATA; // reset spi IF flag
		delay_us(SPI_DELAY_US); // safety delay
	}
}

void spi_read_adc_state (adc_status_t *adc_status) {
	uint8_t data_buffer = 0;
	
	SPID.DATA = adc_status->temp_b1;//(adc_status->temp)/256 ; //output upper byte temperature
	spi_byte_wait(); // wait for master's write spi
	data_buffer = SPID.DATA; // reset spi IF flag
	delay_us(SPI_DELAY_US); // safety delay
	
	SPID.DATA = adc_status->temp_b0 ; //output lower byte temperature
	spi_byte_wait(); // wait for master's write spi
	data_buffer = SPID.DATA; // reset spi IF flag
	delay_us(SPI_DELAY_US); // safety delay
	
		
	SPID.DATA = adc_status->vcc_b1;//(adc_status->temp)/256 ; //output upper byte temperature
	spi_byte_wait(); // wait for master's write spi
	data_buffer = SPID.DATA; // reset spi IF flag
	delay_us(SPI_DELAY_US); // safety delay
		
	SPID.DATA = adc_status->vcc_b0 ; //output lower byte temperature
	spi_byte_wait(); // wait for master's write spi
	data_buffer = SPID.DATA; // reset spi IF flag
	delay_us(SPI_DELAY_US); // safety delay
	
	
	SPID.DATA = adc_status->bandgap_b1;//(adc_status->temp)/256 ; //output upper byte temperature
	spi_byte_wait(); // wait for master's write spi
	data_buffer = SPID.DATA; // reset spi IF flag
	delay_us(SPI_DELAY_US); // safety delay
	
	SPID.DATA = adc_status->bandgap_b0 ; //output lower byte temperature
	spi_byte_wait(); // wait for master's write spi
	data_buffer = SPID.DATA; // reset spi IF flag
	delay_us(SPI_DELAY_US); // safety delay
	
	adc_temp_init_read(); // initial ADCB internal status read chain (temp > Vcc > bandgap)
	
}

void spi_read_adc_threshold (adc_status_t *adc_status) {
	uint8_t data_buffer = 0;
	
	SPID.DATA = adc_status->pwm_1_b1;//(adc_status->temp)/256 ; //output upper byte of adc ch1
	spi_byte_wait(); // wait for master's write spi
	data_buffer = SPID.DATA; // reset spi IF flag
	delay_us(SPI_DELAY_US); // safety delay
	SPID.DATA = adc_status->pwm_1_b0;//(adc_status->temp)/256 ; //output lower byte adc ch1
	spi_byte_wait(); // wait for master's write spi
	data_buffer = SPID.DATA; // reset spi IF flag
	delay_us(SPI_DELAY_US); // safety delay
	
	SPID.DATA = adc_status->pwm_2_b1 ; //output upper byte of adc ch1
	spi_byte_wait(); // wait for master's write spi
	data_buffer = SPID.DATA; // reset spi IF flag
	delay_us(SPI_DELAY_US); // safety delay
	SPID.DATA = adc_status->pwm_2_b0 ; //output lower byte of adc ch2
	spi_byte_wait(); // wait for master's write spi
	data_buffer = SPID.DATA; // reset spi IF flag
	
	adc_pwm2_init_read(); // initial ADCA threshold read chain (pwm 2 > pwm 1)
	
}
void spi_write_dac_pulse(int dac_channel, dac_pulse_t *dac_pulse){
		
		for (int i = 0; i < DAC_SAMPLE_POINT; i++) {
			spi_byte_wait(); // wait for master's write spi
			dac_pulse->ch_Lbyte[dac_channel][i] = SPID.DATA & dac_Lbyte_bm; //write low byte to dac_channel
			SPID.DATA = 0; // output 0 to master as a acknowledgment of data receive
			delay_us(SPI_DELAY_US);
			
			spi_byte_wait(); // wait for master's write spi
			dac_pulse->ch_Hbyte[dac_channel][i] = SPID.DATA & dac_Hbyte_bm; //write high byte to dac_channel
			SPID.DATA = 0; // output 0 to master as a acknowledgment of data receive
			delay_us(SPI_DELAY_US);
		}
		dac_output(dac_channel, dac_pulse); // output test pulse by DAC
		
	}

void spi_echo (void) { //output pwm threshold with spi
	uint8_t data_buffer = 0;
	for (int i = 0; i < SPI_ECHO_FRAMESIZE; i++) {
		spi_byte_wait(); // wait for master's write spi
		data_buffer = SPID.DATA; //set pwm duty cycle in ram
		SPID.DATA = data_buffer; // echo master spi message
	}
}

void spi_echo_2 (void) { //output pwm threshold with spi
	uint8_t data_buffer = 0;
	for (int i = 0; i < SPI_ECHO_2_FRAMESIZE; i++) {
		spi_byte_wait(); // wait for master's write spi
		data_buffer = SPID.DATA; //set pwm duty cycle in ram
		SPID.DATA = data_buffer + 1; // return master message + 1
	}
}

void spi_read(spi_data_t *spi_data, uint8_t *device_serial, pwm_config_list_t *pwm_config_list, adc_status_t *adc_status, dac_pulse_t *dac_pulse) { //routine reading/writing SPI

	uint8_t header_buffer = 0; // store header
		
	header_buffer = SPID.DATA; // retrieve  spi header
	SPID.DATA = 0;	// output 0 for acknowledgment
	switch (header_buffer){ // check which scenarios the header of the whole record is, then react accordingly
		case SPI_WRITE_PWM_LABEL: { //master want to change threshold by writing pwm duty cycle value
			spi_write_pwm(pwm_config_list); //rewrite pwm duty cycle config ram
			pwm_set_threshold(pwm_config_list); //write duty cycle to all pwm
		}
		break;		
		case SPI_READ_PWM_LABEL: spi_read_pwm(pwm_config_list);// read the pwm duty cycle value
		break;
		case SPI_SERIAL_LABEL : spi_serial_out(device_serial);// read the device serial num
		break;
		case SPI_STATUS_ADC_LABEL : spi_read_adc_state(adc_status);// read adc internal status (temp, vcc, bandgap)
		break;
		case SPI_THRESHOLD_ADC_LABEL : spi_read_adc_threshold(adc_status);// read adc internal status (temp, vcc, bandgap)
		break;
		case SPI_WRITE_DAC0_PULSE : spi_write_dac_pulse(0, dac_pulse);// output dac test pulse for ch0
		break;
		case SPI_WRITE_DAC1_PULSE : spi_write_dac_pulse(1, dac_pulse);// output dac test pulse for ch1
		break;
		case SPI_ECHO_LABEL: spi_echo(); //echo test
		break;
		case SPI_ECHO_2_LABEL: spi_echo_2(); //echo message + 1 test
		break;
		default: 
		break;
	}

}




#endif




//----------------------------------------------SPI application----------------------------------------------------