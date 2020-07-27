/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include <include.h> //header of customized function

//----------------------------------------------main_variables-----------------------------------------------------
//initialize variable struct (ram that store FE status/data)
spi_data_t spi_data;
pwm_config_list_t pwm_config_list;
uint8_t device_serial[SPI_SERIAL_FRAMESIZE];
adc_status_t adc_status;
dac_pulse_t dac_pulse;
//----------------------------------------------end_variables-----------------------------------------------------

int main (void){
//--------------------------------------------------initialization-------------------------------------------------------
	sysclk_init(); //initial the board central clock

	board_init(); //initialize the board
	delay_init(); // initialize the delay function
	pmic_init(); // initialize interrupt subroutine
	ioport_init(); //initialize some of the iports
	
	pmic_set_scheduling(PMIC_SCH_ROUND_ROBIN); //settings of pmic (copied from startup example)

	button_LED_init();  //initialize button and LED I/O for ABC address settings
	ioport_set_pin_level(FE_LED, LED_IDLE); // set LED to idle state
	
	adc_read_load_adccal_fues(&adc_status);// read and load ADC offset
	adca_init(); //enable ADCA for threshold reading
	adcb_init(); //enable ADCB for internal status reading
	
	dac_read_load_daccal_fues(&dac_pulse);
	dac_init(); //enable DAC for test pulse generation
	
	spi_init_pins(); // initialize SPI communication subroutine
	spi_load_address(&spi_data); // load the address of the front-end (fe_address)
	spi_address_display(&spi_data); // display the fe_address from ram to the LED
	
	read_serial(device_serial); //read device serial number to ram
	adc_read_temp_fues(&adc_status); // read temperature reference from fuse
		
	pwm_all_init(&pwm_config_list); //set initial pwm duty cycle for thresholds
		
	cpu_irq_enable(); // enable cpu interrupt
	sei(); // enable global interrupt
	
	adc_temp_init_read(); //read temp, vcc and bandgap with adc
	adc_pwm1_init_read(); //read threshold 1 and threshold 2
//-------------------------------------------initialization_ends---------------------------------------------------
	while (true); 
}


//-------------------------------------interrupt service routine (ISR)---------------------------------------------
ISR(INTERRUPT_ABC) { // interrupt of the action button, used to set fe address
	irqflags_t interrupt_flags; //interrupt flag class
	interrupt_flags = cpu_irq_save(); //save interrupt flag

	button_address_activate(&spi_data); //a_fe assignment with ABC function
		
	cpu_irq_restore(interrupt_flags); //restore interrupt flag
}

ISR(INTERRUPT_FE_A) { // interrupt when there is a change in the front end spi address line
	irqflags_t interrupt_flags; //interrupt flag class
	interrupt_flags = cpu_irq_save(); //save interrupt flag

	spi_get_be_address(&spi_data); //identify the a_fe from the be, if equal to a_fe in fe, enable SPI. Otherwise disable
	
	cpu_irq_restore(interrupt_flags); //restore interrupt flag
}

ISR(INTERRUPT_SPI) { //interrupt for spi communication
	irqflags_t interrupt_flags; //interrupt flag class
	interrupt_flags = cpu_irq_save(); //save interrupt flag
	
	spi_read(&spi_data, device_serial, &pwm_config_list, &adc_status, &dac_pulse); //spi communication with be
	
	cpu_irq_restore(interrupt_flags); //restore interrupt flag
}

ISR(INTERRUPT_ADC_TEMP) { // adcb temp result ready also initialze Vcc conversion
	irqflags_t interrupt_flags; //interrupt flag class
	interrupt_flags = cpu_irq_save(); //save interrupt flag

	adc_temp_read(&adc_status); // read temp result
	delay_us(ADC_READ_DELAY_US);// safety delay
	adc_vcc_init_read();// call for vcc conversion in the next channel
	
	cpu_irq_restore(interrupt_flags); //restore interrupt flag
}

ISR(INTERRUPT_ADC_VCC) { // adcb vcc result ready
	irqflags_t interrupt_flags; //interrupt flag class
	interrupt_flags = cpu_irq_save(); //save interrupt flag

	adc_vcc_read(&adc_status); // read vcc result
	
	adca_change_ref_intvcc ();//set reference to Vcc/1.6
	delay_ms(ADC_STARTUP_DELAY_MS);// safety startup delay
	adc_bandgap_init_read();// call for bandgap conversion in the next channel
	
	cpu_irq_restore(interrupt_flags); //restore interrupt flag
}

ISR(INTERRUPT_ADC_BANDGAP) { // adcb bandgap result ready
	irqflags_t interrupt_flags; //interrupt flag class
	interrupt_flags = cpu_irq_save(); //save interrupt flag

	adc_bandgap_read(&adc_status);// read bandgap result
	adca_change_ref_int1v (); //set reference to internal 1V
	delay_ms(ADC_STARTUP_DELAY_MS);// safety startup delay
	
		
	cpu_irq_restore(interrupt_flags); //restore interrupt flag
}

ISR(INTERRUPT_ADC_PWM1) {// adcb threshold 1 result ready
	irqflags_t interrupt_flags; //interrupt flag class
	interrupt_flags = cpu_irq_save(); //save interrupt flag

	adc_pwm1_read(&adc_status); //read threshold 1 result
	delay_us(ADC_READ_DELAY_US);// safety delay
		
	cpu_irq_restore(interrupt_flags); //restore interrupt flag
}

ISR(INTERRUPT_ADC_PWM2) {// adcb threshold 2 result ready
	irqflags_t interrupt_flags; //interrupt flag class
	interrupt_flags = cpu_irq_save(); //save interrupt flag

	adc_pwm2_read(&adc_status); //read threshold 2 result
	delay_us(ADC_READ_DELAY_US); // safety delay
	adc_pwm1_init_read();// call for threshold 1 conversion
	
	cpu_irq_restore(interrupt_flags); //restore interrupt flag
}
//-------------------------------------interrupt service routine (ISR)---------------------------------------------