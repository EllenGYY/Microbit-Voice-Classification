/* Lab 6 - the ADC.
 * Sept. 2021, Philipp H. Kindt <philipp.kindt@informatik.tu-chemnitz.de>
 */
// From C standard library
#include <limits.h>
#include <float.h>
#include <inttypes.h>

// From Nordic SDK
#include "nrf_gpio.h"
#include "nrf_saadc.h"
#include "nrf52_bitfields.h"

// From blessed (microbit-lab)
#include "nrf52_irq.h"

//number of samples the ADC needs to write before creating an interrupt request
#define ADC_NSAMPLES 1000

//Two buffers are needed. The one buffer will be filled actively, while the other one is evaluated.
//In every ISR, both buffers need to be swapped
int16_t adc_buffer1[1000];
int16_t adc_buffer2[1000];
int16_t* buffer_rd = adc_buffer2;



#define IOT_MICROBIT_LED_COL1_GPIO   0
#define IOT_MICROBIT_LED_COL2_GPIO   0
#define IOT_MICROBIT_LED_COL3_GPIO   0
#define IOT_MICROBIT_LED_COL4_GPIO   1
#define IOT_MICROBIT_LED_COL5_GPIO   0


#define IOT_MICROBIT_LED_COL1_PIN   28
#define IOT_MICROBIT_LED_COL2_PIN   11
#define IOT_MICROBIT_LED_COL3_PIN   31
#define IOT_MICROBIT_LED_COL4_PIN   5
#define IOT_MICROBIT_LED_COL5_PIN   30

/*GPIO assigment of columns*/
#define IOT_MICROBIT_LED_ROW1_PIN   21
#define IOT_MICROBIT_LED_ROW2_PIN   22
#define IOT_MICROBIT_LED_ROW3_PIN   15
#define IOT_MICROBIT_LED_ROW4_PIN   24
#define IOT_MICROBIT_LED_ROW5_PIN   19

#define IOT_MICROBIT_LED_ROW1_GPIO   0
#define IOT_MICROBIT_LED_ROW2_GPIO   0
#define IOT_MICROBIT_LED_ROW3_GPIO   0
#define IOT_MICROBIT_LED_ROW4_GPIO   0
#define IOT_MICROBIT_LED_ROW5_GPIO   0

/* join column and row in one data structure */
#define IOT_MICROBIT_LED_COL1 NRF_GPIO_PIN_MAP(IOT_MICROBIT_LED_COL1_GPIO,IOT_MICROBIT_LED_COL1_PIN)
#define IOT_MICROBIT_LED_COL2 NRF_GPIO_PIN_MAP(IOT_MICROBIT_LED_COL2_GPIO,IOT_MICROBIT_LED_COL2_PIN)
#define IOT_MICROBIT_LED_COL3 NRF_GPIO_PIN_MAP(IOT_MICROBIT_LED_COL3_GPIO,IOT_MICROBIT_LED_COL3_PIN)
#define IOT_MICROBIT_LED_COL4 NRF_GPIO_PIN_MAP(IOT_MICROBIT_LED_COL4_GPIO,IOT_MICROBIT_LED_COL4_PIN)
#define IOT_MICROBIT_LED_COL5 NRF_GPIO_PIN_MAP(IOT_MICROBIT_LED_COL5_GPIO,IOT_MICROBIT_LED_COL5_PIN)

#define IOT_MICROBIT_LED_ROW1 NRF_GPIO_PIN_MAP(IOT_MICROBIT_LED_ROW1_GPIO,IOT_MICROBIT_LED_ROW1_PIN)
#define IOT_MICROBIT_LED_ROW2 NRF_GPIO_PIN_MAP(IOT_MICROBIT_LED_ROW2_GPIO,IOT_MICROBIT_LED_ROW2_PIN)
#define IOT_MICROBIT_LED_ROW3 NRF_GPIO_PIN_MAP(IOT_MICROBIT_LED_ROW3_GPIO,IOT_MICROBIT_LED_ROW3_PIN)
#define IOT_MICROBIT_LED_ROW4 NRF_GPIO_PIN_MAP(IOT_MICROBIT_LED_ROW4_GPIO,IOT_MICROBIT_LED_ROW4_PIN)
#define IOT_MICROBIT_LED_ROW5 NRF_GPIO_PIN_MAP(IOT_MICROBIT_LED_ROW5_GPIO,IOT_MICROBIT_LED_ROW5_PIN)

/*create an array of cols and an array of rows*/
uint32_t cols[] = {IOT_MICROBIT_LED_COL1, IOT_MICROBIT_LED_COL2, IOT_MICROBIT_LED_COL3, IOT_MICROBIT_LED_COL4, IOT_MICROBIT_LED_COL5};
uint32_t rows[] = {IOT_MICROBIT_LED_ROW1, IOT_MICROBIT_LED_ROW2, IOT_MICROBIT_LED_ROW3, IOT_MICROBIT_LED_ROW4, IOT_MICROBIT_LED_ROW5};





void configure_sadc(){

  //Activate the ADC
  NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos;

  //Optional: Calibrate the ADC
  NRF_SAADC->TASKS_CALIBRATEOFFSET = 1;
  while(NRF_SAADC->EVENTS_CALIBRATEDONE == 0){
    asm("nop");
  }
  NRF_SAADC->EVENTS_CALIBRATEDONE = 0;

  //Single-Ended mode
  NRF_SAADC->CH[0].CONFIG |= SAADC_CH_CONFIG_MODE_SE << SAADC_CH_CONFIG_MODE_Pos;

  //Use analog input number 2. It's internally connected to the microbit
  NRF_SAADC->CH[0].PSELP = SAADC_CH_PSELP_PSELP_AnalogInput3;

  //single-ended mode => set to PSELN to "not connected"
  NRF_SAADC->CH[0].PSELN = SAADC_CH_PSELP_PSELP_NC;

  //Gain 1/2
  NRF_SAADC->CH[0].CONFIG |= SAADC_CH_CONFIG_GAIN_Gain1_4 << SAADC_CH_CONFIG_GAIN_Pos;

  //Internal reference voltage
  NRF_SAADC->CH[0].CONFIG |= SAADC_CH_CONFIG_REFSEL_VDD1_4 << SAADC_CH_CONFIG_REFSEL_Pos;

  //3 microseconds aquisition time
  NRF_SAADC->CH[0].CONFIG = (NRF_SAADC->CH[0].CONFIG & ~(SAADC_CH_CONFIG_TACQ_Msk)) | SAADC_CH_CONFIG_TACQ_3us << SAADC_CH_CONFIG_TACQ_Pos;

  //12 bit reolution
  NRF_SAADC->RESOLUTION = NRF_SAADC_RESOLUTION_12BIT;


  //set pointer to adc_buffer1
  NRF_SAADC->RESULT.PTR = adc_buffer1;

  //set the number of samples to write into the memory before generating an interrupt
  NRF_SAADC->RESULT.MAXCNT = ADC_NSAMPLES;

  //Activate end interupt
  NRF_SAADC->INTENSET = SAADC_INTENSET_END_Enabled<<SAADC_INTENSET_END_Pos;



  /***** Place your code for configuring the ADC here ***/

  //32x oversampling
    NRF_SAADC->OVERSAMPLE = SAADC_OVERSAMPLE_OVERSAMPLE_Over32x<<SAADC_OVERSAMPLE_OVERSAMPLE_Pos;

    //Continuous sampling// timer mode.
    NRF_SAADC->SAMPLERATE = (SAADC_SAMPLERATE_MODE_Timers << SAADC_SAMPLERATE_MODE_Pos);

    //Prescaleer to 80.
    NRF_SAADC->SAMPLERATE |= (80<<SAADC_SAMPLERATE_CC_Pos);              //200 kHz

    //resulting sampling rate: 16 MHz / (80*32) =  6.250 kHz



  //start the ADC. Both tasks are needed
  NRF_SAADC->TASKS_START = 1;
  NRF_SAADC->TASKS_SAMPLE = 1;

  //enable the interrupt
  NVIC_EnableIRQ(SAADC_IRQn);
  NVIC_SetPriority(SAADC_IRQn,IRQ_PRIORITY_MEDIUM);
  NVIC_ClearPendingIRQ(SAADC_IRQn);







}

int main(void)
{

  //Init the log function to allow us writing to the console
  log_init();
  log_printf("Configuring SAAD\n");

  //Set the Micro:bit led display pins to output mode
  nrf_gpio_cfg_output(IOT_MICROBIT_LED_COL1);
  nrf_gpio_cfg_output(IOT_MICROBIT_LED_COL2);
  nrf_gpio_cfg_output(IOT_MICROBIT_LED_COL3);
  nrf_gpio_cfg_output(IOT_MICROBIT_LED_COL4);
  nrf_gpio_cfg_output(IOT_MICROBIT_LED_COL5);
  nrf_gpio_cfg_output(IOT_MICROBIT_LED_ROW1);
  nrf_gpio_cfg_output(IOT_MICROBIT_LED_ROW2);
  nrf_gpio_cfg_output(IOT_MICROBIT_LED_ROW3);
  nrf_gpio_cfg_output(IOT_MICROBIT_LED_ROW4);
  nrf_gpio_cfg_output(IOT_MICROBIT_LED_ROW5);

  //GIPO pins can be driven with different power levels. We here chose the highest drive power for brighter LEDs
  NRF_P0->PIN_CNF[IOT_MICROBIT_LED_ROW1_PIN] |= NRF_GPIO_PIN_H0H1<<GPIO_PIN_CNF_DRIVE_Pos;
  NRF_P0->PIN_CNF[IOT_MICROBIT_LED_ROW2_PIN] |= NRF_GPIO_PIN_H0H1<<GPIO_PIN_CNF_DRIVE_Pos;
  NRF_P0->PIN_CNF[IOT_MICROBIT_LED_ROW3_PIN] |= NRF_GPIO_PIN_H0H1<<GPIO_PIN_CNF_DRIVE_Pos;
  NRF_P0->PIN_CNF[IOT_MICROBIT_LED_ROW4_PIN] |= NRF_GPIO_PIN_H0H1<<GPIO_PIN_CNF_DRIVE_Pos;
  NRF_P0->PIN_CNF[IOT_MICROBIT_LED_ROW5_PIN] |= NRF_GPIO_PIN_H0H1<<GPIO_PIN_CNF_DRIVE_Pos;
  NRF_P0->PIN_CNF[IOT_MICROBIT_LED_COL1_PIN] |= NRF_GPIO_PIN_H0H1<<GPIO_PIN_CNF_DRIVE_Pos;
  NRF_P0->PIN_CNF[IOT_MICROBIT_LED_COL2_PIN] |= NRF_GPIO_PIN_H0H1<<GPIO_PIN_CNF_DRIVE_Pos;
  NRF_P0->PIN_CNF[IOT_MICROBIT_LED_COL3_PIN] |= NRF_GPIO_PIN_H0H1<<GPIO_PIN_CNF_DRIVE_Pos;
  NRF_P1->PIN_CNF[IOT_MICROBIT_LED_COL4_PIN] |= NRF_GPIO_PIN_H0H1<<GPIO_PIN_CNF_DRIVE_Pos;
  NRF_P0->PIN_CNF[IOT_MICROBIT_LED_COL5_PIN] |= NRF_GPIO_PIN_H0H1<<GPIO_PIN_CNF_DRIVE_Pos;


  //configure the ADC
  configure_sadc();

  //The microphone is connected trough a switch, which needs to be closed using a GPIO. It
  //is connected to GPIO 20. Let's switch it on!
  nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(0,20));
  nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(0,20),1);

  //High drive power is required for this GPIO pin.
  NRF_P0->PIN_CNF[20] |= NRF_GPIO_PIN_H0H1<<GPIO_PIN_CNF_DRIVE_Pos;

  //wait forever
  while(1){
    ;
  }
  return 0;
}


void SAADC_IRQHandler(void){

  //The maximum magnitude that occured so far. We will scale the volume meter to this
  static int32_t magnitude_max = -1;

  //check if the END-event occured.
  if(NRF_SAADC->EVENTS_END == 1){
    NRF_SAADC->EVENTS_END = 0;          //reset the end event

    //swat between both buffers. After this, buffer_rd points to the one that was filled previously, whereas
    //the adc writes its new values to the other buffer.
    if( NRF_SAADC->RESULT.PTR == adc_buffer1){
      NRF_SAADC->RESULT.PTR = adc_buffer2;
      buffer_rd = adc_buffer1;
    }else{
      NRF_SAADC->RESULT.PTR = adc_buffer1;
      buffer_rd = adc_buffer2;
    }
    //restart the ADC for filling the next buffer. Here, only the START-task is needed, not the TASKS_SAMPLE
    NRF_SAADC->TASKS_START = 1;


    //If the magnitude_max has a negative value, this is the first call of the IRQHandler.
    //We then exit this ISR, because we do not yet have a valid magnitude_max
    if(magnitude_max < 0){
      magnitude_max = 0;                        //set
      return;           //first  interrupt => discard
    }

    //number of samples in the buffer
    uint32_t nSamples = NRF_SAADC->RESULT.AMOUNT;

    //compute the min and the maximum in the buffer of ADC values.
    int32_t max = INT_MIN;
    int32_t min = INT_MAX;
    for(uint32_t i = 0; i < nSamples; i++){
      if(buffer_rd[i] > max){
        max = buffer_rd[i];
      }
      if(buffer_rd[i] < min){
        min = buffer_rd[i];
      }
    }

    //compute the magnitude - here the largest difference between maximum value and minimum  value
    int32_t magnitude = max - min;

    //track the global maximum magnitude to scale the volume meter
    if(magnitude > magnitude_max){
      magnitude_max = magnitude;
    }

    //compute the percentage of the magnitude occurred in this buffer with the maximum that occured in all earlier buffers
    double percentage = ((double)(magnitude))/((double) magnitude_max);
    log_printf("%d - %d -%d -%d => %d\n",min, max, magnitude,magnitude_max,(int32_t) (percentage * 1000.0));

    //write according to classes
    if(percentage > 0.2){
      nrf_gpio_pin_write(IOT_MICROBIT_LED_ROW1,1);
    }else{
      nrf_gpio_pin_write(IOT_MICROBIT_LED_ROW1,0);
    }
    if(percentage > 0.4){
      nrf_gpio_pin_write(IOT_MICROBIT_LED_ROW2,1);
    }else{
      nrf_gpio_pin_write(IOT_MICROBIT_LED_ROW2,0);
    }
    if(percentage > 0.6){
      nrf_gpio_pin_write(IOT_MICROBIT_LED_ROW3,1);
    }else{
      nrf_gpio_pin_write(IOT_MICROBIT_LED_ROW3,0);
    }
    if(percentage > 0.8){
      nrf_gpio_pin_write(IOT_MICROBIT_LED_ROW4,1);
    }else{
      nrf_gpio_pin_write(IOT_MICROBIT_LED_ROW4,0);
    }

    if(percentage > 0.99){
      nrf_gpio_pin_write(IOT_MICROBIT_LED_ROW5,1);
    }else{
      nrf_gpio_pin_write(IOT_MICROBIT_LED_ROW5,0);

    }

  }
}
