#ifndef _APPCONST_H_
#define _APPCONST_H_

//-------------------------------------------------DAC application--------------------------------------------------
#define DAC_SAMPLE_POINT 3 //number of points to sample in a DAC pulse
#define DAC_CHANNEL 2 // number of DAC channel
#define DAC_LEVEL_HOLD_US 3 //smallest possible level hold time
#define DAC_STARTUP_DELAY_US 20 //DAC safety start up delay in us

enum dac_data_bm { //bitsmask for writing 12 bit DAC data
	dac_Lbyte_bm = 0xFF,
	dac_Hbyte_bm = 0x0F
	};

typedef struct dac_pulse{ //DAC pulse shape
	uint8_t ch_Lbyte[DAC_CHANNEL][DAC_SAMPLE_POINT];
	uint8_t ch_Hbyte[DAC_CHANNEL][DAC_SAMPLE_POINT];
	uint8_t dac0_cal_gain;
	uint8_t dac0_cal_offset;
	uint8_t dac1_cal_gain;
	uint8_t dac1_cal_offset;
} dac_pulse_t;
//-------------------------------------------------DAC application--------------------------------------------------
//---------------------------------------------SPI initialization--------------------------------------------------

enum SPI_SETTING{
	 SPI_MODE = 0x00 //spi operation mode 0
};

#define SPI_A_FE0    IOPORT_CREATE_PIN(PORTC, 6) // pin for least significant bit of spi FE address
#define SPI_A_FE1    IOPORT_CREATE_PIN(PORTC, 7) // pin for most significant bit of spi FE address
#define SPI_nSS		 IOPORT_CREATE_PIN(PORTD, 4) // pin for chip select (bar) of spi
#define SPI_MOSI	 IOPORT_CREATE_PIN(PORTD, 5) // pin for MOSI of spi
#define SPI_MISO	 IOPORT_CREATE_PIN(PORTD, 6) // pin for MISO of spi
#define SPI_SCLK	 IOPORT_CREATE_PIN(PORTD, 7) // pin for serial clock of spi

#define SPI_DELAY_US 1 // safety delay for spi
#define SPI_IDLE_OUT 0 // idle frame of spi
#define SPI_BYTE_WAIT_US 4000 //maximum byte-byte separation in us


enum SPI_FRAMESIZE{ // size for different spi record without header
	SPI_WRITE_PWM_FRAMESIZE = 16, //write 16 PWM duty cycles of microcontroller
	SPI_READ_PWM_FRAMESIZE = 16, //read 16 PWM duty cycles of microcontroller
	SPI_SERIAL_FRAMESIZE = 11, //read serial number of microcontroller
	SPI_STATUS_ADC_FRAMESIZE = 2, //read ADC of microcontroller and store to ram
	SPI_THRESHOLD_ADC_FRAMESIZE = 4, //read threshold with ADC
	SPI_WRITE_DAC_PULSE_FRAMESIZE = DAC_SAMPLE_POINT*2,	
	SPI_ECHO_FRAMESIZE = 1, //echo header
	SPI_ECHO_2_FRAMESIZE = 8 //echo 2 header
};

enum SPI_LABEL { // header 1-byte information correspondent to record content
	SPI_WRITE_PWM_LABEL = 0x01, //write 16 PWM duty cycles from spi to microcontroller
	SPI_READ_PWM_LABEL = 0x02, //read 16 PWM duty cycles from microcontroller to spi
	SPI_SERIAL_LABEL = 0x03, //read serial number of microcontroller
	SPI_STATUS_ADC_LABEL = 0x04, //output ADC from ram to spi
	SPI_THRESHOLD_ADC_LABEL = 0x07, //read threshold with ADC
	SPI_WRITE_DAC0_PULSE = 0x05, //write dac channel 0
	SPI_WRITE_DAC1_PULSE = 0x06, //write dac channel 1
	SPI_ECHO_LABEL = 0xDD, //echo master message
	SPI_ECHO_2_LABEL = 0xDE  //echo master message with + 1 value
};



typedef struct SPI_DATA{ // structure containing spi io data/address
	uint8_t fe_address; // spi fe address store in nvm of spi
	uint8_t be_address; // spi fe address arrived at fe through 2-bit fe_a line
} spi_data_t;
//---------------------------------------------SPI initialization--------------------------------------------------
//----------------------------------------------PWM initialization--------------------------------------------------
#define PWM_NUMBER_MAX 16 //total number of pwms (channels) on fe
#define PWM_DUTY_DEFAULT 50 //startup default pwm duty cycles setting
#define PWM_FREQUENCY 10000000 // default pwm tc frequency

typedef struct pwm_config_list{ //struct containing pwm configuration info
	
	struct pwm_config pwm_cfg[PWM_NUMBER_MAX]; // configuration struct for pwm setting
	long int pwm_duty_cycle[PWM_NUMBER_MAX]; // pwm duty cycle 
	long int pwm_frequency_list[PWM_NUMBER_MAX]; // pwm tc frequency
	
} pwm_config_list_t;

//----------------------------------------------PWM initialization--------------------------------------------------
//---------------------------------------------Flash RAM initialization---------------------------------------------
#define FRAM_WRITE_DELAY_MS 10 //safety delay for flash ram writing time

#define ADDRESS_PAGE 1 // page where address located
#define ADDRESS_PAGE_ADDR ADDRESS_PAGE * EEPROM_PAGE_SIZE // location of FE address memory
#define ADDRESS_BYTE 1 // byte in page where address located
#define MAX_ADDRESS 254 //maximum allowed Front End number of addresses

#define PWM_PAGE 2 // page where PWM Info located
#define PWM_PAGE_ADDR PWM_PAGE * EEPROM_PAGE_SIZE // location of PWM memory
#define PWM_BYTE 1 // byte of a single PWM (1 Byte duty cycle)

typedef struct nvm_page{
	int read_address_page; //address page being read
} led_nvm_page_t;

//---------------------------------------------Flash RAM initialization---------------------------------------------
//----------------------------------------------Button/LED applications---------------------------------------------
#define FE_LED    IOPORT_CREATE_PIN(PORTE, 4) //LED0 port
#define FE_LED1    IOPORT_CREATE_PIN(PORTE, 5) //LED1 port
#define FE_BUTTON IOPORT_CREATE_PIN(PORTF, 4) //button(switch0) port
#define FE_SWITCH1 IOPORT_CREATE_PIN(PORTF, 6) //switch1 port
#define FE_SWITCH2 IOPORT_CREATE_PIN(PORTF, 7) //switch2 port
#define FE_BE0 IOPORT_CREATE_PIN(PORTC, 4) //BE0 port
#define FE_BE1 IOPORT_CREATE_PIN(PORTC, 5) //BE1 port

#define DEBOUNCE_DELAY_MS 10 //debounce delay for button in ms

//long button click detection
#define MAX_LONG_LOOP_DELAY 10 // time delay for each detection loop in ms 
#define MAX_LONG_BUTTON_COUNT 200 //number of detection loop

#define LED_FLASH_TIME_MS 100 //Time of flash for very short LED flashes (e.g. when short press button in address setting mode)
#define LED_BRINK_TIME_MS 250 //Time of flash for long LED flashes (e.g. when displaying FE_A with ABC)
#define LED_SETUP_TIME_S 2 // LED froze time when the system start up (not used) 

#define LED_IDLE true //idle state of LED

typedef struct LED_BUTTON{
	bool button_pressed; // variable for storing button status
	bool button_previous_state_pressed; // button status one loop before
	bool button_enable_address; //address input mode enable
	bool led_state;//bool led_state = LED_IDLE; //current LED state
	int i_address_cnt; //counting address from button input
	bool changed_mode;
} led_button_t;
//----------------------------------------------Button/LED applications---------------------------------------------
//------------------------------------------------ADC application---------------------------------------------------
#define ADC_TEMP ADC_INT_TEMPSENSE //ADC temperature setting
#define ADC_BANDGAP ADC_INT_BANDGAP //ADC bandgap setting
#define ADC_VCC ADC_REF_VCC //ADC Vcc setting
#define ADC_PWM_CH1 ADC_CH1 //ADC read back PWM 1
#define ADC_PWM_CH2 ADC_CH2 //ADC read back PWM 2

#define ADC_READ_DELAY_US 10// minimum reading delay of ADC in US

enum adc_data_bm { //bitsmask for reading 12 bit ADC data
	adc_Lbyte_bm = 0xFF,
	adc_Hbyte_bm = 0x0F
};

#define CONFIG_ADC_VERSION   1 //configuration for ADC version

#define ADC_STARTUP_DELAY_MS 3 // minimum startup stabilization time of ADC in MS

typedef struct ADC_STATUS{
	uint8_t temp_b0; //temperature 12 bits
	uint8_t temp_b1;
	uint8_t bandgap_b0; //bandgap 12 bits
	uint8_t bandgap_b1;
	uint8_t vcc_b0; //Vcc 12 bits
	uint8_t vcc_b1;
	uint8_t ref_t0; //reference temperature bits
	uint8_t ref_t1;
	uint8_t ref_acall; //acda calibration bits
	uint8_t ref_acalh;
	uint8_t ref_bcall; //acdb calibration bits
	uint8_t ref_bcalh;
	uint8_t pwm_1_b0; //threshold 1 reading 12 bits
	uint8_t pwm_1_b1;
	uint8_t pwm_2_b0; //threshold 2 reading 12 bits
	uint8_t pwm_2_b1;
} adc_status_t;
//-------------------------------------------------ADC application--------------------------------------------------

//----------------------------------------------Interupt application------------------------------------------------
// interrupt vector for ISR (interrupt service routine)
#define INTERRUPT_ABC PORTB_INT0_vect
#define INTERRUPT_FE_A PORTE_INT0_vect
#define INTERRUPT_SPI SPID_INT_vect
#define INTERRUPT_ADC_TEMP ADCB_CH0_vect
#define INTERRUPT_ADC_BANDGAP ADCB_CH1_vect
#define INTERRUPT_ADC_VCC ADCB_CH2_vect
#define INTERRUPT_ADC_PWM1 ADCA_CH1_vect
#define INTERRUPT_ADC_PWM2 ADCA_CH2_vect
//-----------------------------------------------Interupt application-----------------------------------------------
#endif