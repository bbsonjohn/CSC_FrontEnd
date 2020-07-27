#ifndef _DAC_APP_C_
#define _DAC_APP_C_

#include <appconst.h>

//--------------------------------------------function prototypes---------------------------------------------------
void dac_read_load_daccal_fues(dac_pulse_t *dac_pulse);
void dac_init(void);
void dac_output(int dac_channel, dac_pulse_t *dac_pulse);
//--------------------------------------------function prototypes---------------------------------------------------

void dac_read_load_daccal_fues(dac_pulse_t *dac_pulse){ // read and load dac calibration
	//read calibration from dac
	dac_pulse->dac0_cal_gain = nvm_read_production_signature_row(DAC_CAL_DACB0_GAIN);
	dac_pulse->dac0_cal_offset = nvm_read_production_signature_row(DAC_CAL_DACB0_OFFSET);
	dac_pulse->dac1_cal_gain = nvm_read_production_signature_row(DAC_CAL_DACB1_GAIN);
	dac_pulse->dac1_cal_offset = nvm_read_production_signature_row(DAC_CAL_DACB1_OFFSET);

	//load calibration into DACB register
	DACB.CH0GAINCAL = dac_pulse->dac0_cal_gain;
	DACB.CH0OFFSETCAL = dac_pulse->dac0_cal_offset;
	DACB.CH1GAINCAL = dac_pulse->dac1_cal_gain;
	DACB.CH1OFFSETCAL = dac_pulse->dac1_cal_offset;
}

void dac_init(void){
	sysclk_enable_module(SYSCLK_PORT_B, SYSCLK_DAC); // enable peripheral clock for DAC at port B
	ioport_configure_port_pin(&PORTB, PIN2_bm, IOPORT_INIT_LOW | IOPORT_DIR_OUTPUT); // initialize DAC output pins
	ioport_configure_port_pin(&PORTB, PIN3_bm, IOPORT_INIT_LOW | IOPORT_DIR_OUTPUT);
	
	DACB_CTRLB = 0x40; //Set (CHSEL) to select dual channels operation; Clear CH0TRIG and CH1TRIG
	DACB_CTRLC = 0x00; //Set (REFSEL) to use bandgap as conversion reference.
	DACB_CTRLA = 0x0C; //Enable CH0EN and CH1EN in Control Register A (CTRLA) to enable both DAC outputs.
	
	DACB_CTRLA |= DAC_ENABLE_bm; //enable DAC
	
	delay_us(DAC_STARTUP_DELAY_US); //DAC startup delay
}


void dac_output(int dac_channel, dac_pulse_t *dac_pulse){

	switch (dac_channel){ // check which scenarios the header of the whole record is, then react accordingly
		case 0: { 			
			for (int i = 0; i < DAC_SAMPLE_POINT; i++) {
				DACB.CH0DATAL = dac_pulse->ch_Lbyte[dac_channel][i]; //write low byte level to channel 0
				DACB.CH0DATAH = dac_pulse->ch_Hbyte[dac_channel][i]; //write level to channel 0
				delay_us(DAC_LEVEL_HOLD_US);
			}
		}
		break;
		case 1: {			
			for (int i = 0; i < DAC_SAMPLE_POINT; i++) {
				DACB.CH1DATAL = dac_pulse->ch_Lbyte[dac_channel][i]; //write level to channel 1
				DACB.CH1DATAH = dac_pulse->ch_Hbyte[dac_channel][i]; //write level to channel 1
				delay_us(DAC_LEVEL_HOLD_US);
				}
		}
		break;
		default: break;
	}
}





#endif