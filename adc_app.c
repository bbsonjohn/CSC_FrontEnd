//---------------------------------------------------ADC Application----------------------------------------------
#ifndef _ADC_APP_C_
#define _ADC_APP_C_

#include <appconst.h>

//--------------------------------------------function prototypes---------------------------------------------------
void adc_read_load_adccal_fues(adc_status_t *adc_status);
void adc_read_temp_fues(adc_status_t *adc_status);
static void adcb_init(void);
static void adca_init(void);
void adca_change_ref_int1v ( void ); 
void adca_change_ref_intvcc ( void );
void adc_temp_init_read( void ) ;
void adc_bandgap_init_read( void );
void adc_vcc_init_read( void );
void adc_pwm1_init_read( void );
void adc_pwm2_init_read( void );
void adc_temp_read(adc_status_t *adc_status);
void adc_bandgap_read(adc_status_t *adc_status);
void adc_vcc_read(adc_status_t *adc_status);
void adc_pwm1_read(adc_status_t *adc_status);
void adc_pwm2_read(adc_status_t *adc_status);
//--------------------------------------------function prototypes---------------------------------------------------

void adc_read_load_adccal_fues(adc_status_t *adc_status){ // read and load adc calibration
	//read calibration from adc
	adc_status->ref_acall = nvm_read_production_signature_row(ADCACAL0);
	adc_status->ref_acalh = nvm_read_production_signature_row(ADCACAL1);
	adc_status->ref_bcall = nvm_read_production_signature_row(ADCBCAL0);
	adc_status->ref_bcalh = nvm_read_production_signature_row(ADCBCAL1);
	
	//load calibration into ADC.CAL register
	ADCA.CALL = adc_status->ref_acall;
	ADCA.CALH = adc_status->ref_acalh;
	ADCB.CALL = adc_status->ref_bcall;
	ADCB.CALH = adc_status->ref_bcalh;
}

void adc_read_temp_fues(adc_status_t *adc_status){ // read and store temperature reference calibrated at 85C
	adc_status->ref_t0 = nvm_read_production_signature_row(TEMPSENSE0);
	adc_status->ref_t1 = nvm_read_production_signature_row(TEMPSENSE1);
}

static void adcb_init(void) // initialize ADC
{	
	sysclk_enable_module(SYSCLK_PORT_B, SYSCLK_ADC);  //Enable peripherial clock (portA for adcA)
	ADCB.CTRLA = 0x01;  //disable DMA, start "channel 0" with out flushing pipeline, enable ADC
	ADCB.CTRLB = 0x00;//Set conversion mode to 0 (unsigned), single conversion, 12-bit right justified
	ADCB.REFCTRL = 0x03; //Set ADC reference to internal 1V, enable bandgap and temp ref
	ADCB.EVCTRL = 0x00; //disable event input
	ADCB.PRESCALER = 0x04;  //Set minimum clock prescalar = 1/64
	ADCB.INTFLAGS = 0x0F;  //Clear all interrupt flags
	
	ADCB.CH0.CTRL = 0x00; // Set internal input source
	ADCB.CH0.MUXCTRL = 0x00; //Set pins connection to temperature
	ADCB.CH0.INTCTRL = 0x01;  //Set interrupt flag to conversion complete, low level interrupt
	
	ADCB.CH1.CTRL = 0x00; // Set internal input source
	ADCB.CH1.MUXCTRL = 0x01 << 3; //Set pins connection to bandgap
	ADCB.CH1.INTCTRL = 0x01;  //Set interrupt flag to conversion complete, low level interrupt
	
	ADCB.CH2.CTRL = 0x00; // Set internal input source
	ADCB.CH2.MUXCTRL = 0x02 << 3; //Set pins connection to Vcc/10
	ADCB.CH2.INTCTRL = 0x01;  //Set interrupt flag to conversion complete, low level interrupt	
	delay_ms(ADC_STARTUP_DELAY_MS); //startup delay

}

static void adca_init(void) // initialize ADC
{
	//initialize port for ADC
	ioport_configure_port_pin(&PORTA, PIN0_bm, IOPORT_DIR_INPUT); 
	ioport_configure_port_pin(&PORTA, PIN1_bm, IOPORT_DIR_INPUT); 
	ioport_configure_port_pin(&PORTA, PIN2_bm, IOPORT_DIR_INPUT); 
	ioport_configure_port_pin(&PORTA, PIN3_bm, IOPORT_DIR_INPUT); 
	ioport_configure_port_pin(&PORTA, PIN4_bm, IOPORT_DIR_INPUT); 
	ioport_configure_port_pin(&PORTA, PIN5_bm, IOPORT_DIR_INPUT); 
	ioport_configure_port_pin(&PORTA, PIN6_bm, IOPORT_DIR_INPUT); 
	ioport_configure_port_pin(&PORTA, PIN7_bm, IOPORT_DIR_INPUT); 
	
	sysclk_enable_module(SYSCLK_PORT_A, SYSCLK_ADC);  //Enable peripherial clock (portB for adcB)
	ADCA.CTRLA = 0x01;  //disable DMA, start "channel 0" without flushing pipeline, enable ADC
	ADCA.CTRLB = 0x00; //Set conversion mode to 0 (unsigned), single conversion, 12-bit right justified*
	ADCA.REFCTRL = 0x03; //Set ADC reference to internal 1V, enable bandgap and temp ref
	ADCA.EVCTRL = 0x00; //disable event input
	ADCA.PRESCALER = 0x04;  //Set minimum clock prescalar = 1/64
	ADCA.INTFLAGS = 0x0F;  //Clear all interrupt flags
	
	
	ADCA.CH1.CTRL = 0x01; // Set single-ended input source
	ADCA.CH1.MUXCTRL = 0x01 << 3; //Set pins connection to ADC1
	ADCA.CH1.INTCTRL = 0x01;  //Set interrupt flag to conversion complete, low level interrupt
	
	ADCA.CH2.CTRL = 0x01; // Set single-ended input source
	ADCA.CH2.MUXCTRL = 0x02 << 3; //Set pins connection to ADC2
	ADCA.CH2.INTCTRL = 0x01;  //Set interrupt flag to conversion complete, low level interrupt
	
	delay_ms(ADC_STARTUP_DELAY_MS); //startup delay
}

void adca_change_ref_int1v ( void ) {
	//change adc ref pin to internal 1V, enable bandgap and temp ref
	ADCB.REFCTRL = 0x03;
} 

void adca_change_ref_intvcc ( void ) {
	//change adc ref pin to Vcc/1.6, enable bandgap and temp ref
	ADCB.REFCTRL = 0x13;
}

void adc_temp_init_read( void ) { 
	//start ADC conversion
	ADCB.CH0.CTRL |= 0x80;
}

void adc_bandgap_init_read( void ) { 
	//start ADC conversion
	ADCB.CH1.CTRL |= 0x80;
}

void adc_vcc_init_read( void ) { 
	//start ADC conversion
	ADCB.CH2.CTRL |= 0x80;
}

void adc_pwm1_init_read( void ) { 
	//start ADC conversion
	ADCA.CH1.CTRL |= 0x80;
}

void adc_pwm2_init_read( void ) {
	//start ADC conversion
	ADCA.CH2.CTRL |= 0x80;
}

void adc_temp_read(adc_status_t *adc_status) { //read temperature and Vcc from adc
	uint16_t ref; //reference temperature
	uint16_t degrees; //actual degree
	
	//reading reference temperature
	ref = (adc_status->ref_t0); 
	ref += (adc_status->ref_t1) << 8;
	
	//get and store adc result
	degrees = ADCB.CH0.RESL & adc_Lbyte_bm; 
	degrees += (ADCB.CH0.RESH & adc_Hbyte_bm) << 8;
	
	//numerical conversion of actual temperature
	degrees *= (((273 + 85)*10) / (float)ref);
	adc_status->temp_b0 = degrees%256;
	adc_status->temp_b1 = degrees/256;

}

void adc_bandgap_read(adc_status_t *adc_status) { //read temperature and Vcc from adc
	uint16_t bandgap;
	
	//get and store adc result
	bandgap = ADCB.CH1.RESL & adc_Lbyte_bm;
	bandgap += (ADCB.CH1.RESH & adc_Hbyte_bm) << 8;
	
	//break the 12bit bandgap for readout
	adc_status->bandgap_b0 = bandgap%256;
	adc_status->bandgap_b1 = bandgap/256;
	
}

void adc_vcc_read(adc_status_t *adc_status) { //read temperature and Vcc from adc
	uint16_t vcc;
	
	//get and store adc result
	vcc = ADCB.CH2.RESL & adc_Lbyte_bm;
	vcc += (ADCB.CH2.RESH & adc_Hbyte_bm) << 8;
	
	//break the 12bit vcc for readout
	adc_status->vcc_b0 = vcc%256;
	adc_status->vcc_b1 = vcc/256;
	
}

void adc_pwm1_read(adc_status_t *adc_status) { //read temperature and Vcc from adc
	uint16_t threshold;
	
	//get and store adc result
	threshold = ADCA.CH1.RESL & adc_Lbyte_bm;
	threshold += (ADCA.CH1.RESH & adc_Hbyte_bm) << 8;
	
	//break the 12bit threshold for readout
	adc_status->pwm_1_b0 = threshold%256;
	adc_status->pwm_1_b1 = threshold/256;
}

void adc_pwm2_read(adc_status_t *adc_status) { //read temperature and Vcc from adc
	uint16_t threshold;
	
	//get and store adc result
	threshold = ADCA.CH2.RESL & adc_Lbyte_bm;
	threshold += (ADCA.CH2.RESH & adc_Hbyte_bm) << 8;
	
	//break the 12bit threshold for readout
	adc_status->pwm_2_b0 = threshold%256;
	adc_status->pwm_2_b1 = threshold/256;
}

#endif



//---------------------------------------------------ADC Application----------------------------------------------
