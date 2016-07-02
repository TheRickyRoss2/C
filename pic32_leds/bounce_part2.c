/**
 * File: bounce_part2.c
 * @brief Displays the value of the potentiometer, both raw and as a percent.
 * @author Ric Rodriguez, 2016.05.11  */

// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries


// **** Set macros and preprocessor directives ****

// **** Declare any datatypes here ****

struct AdcResult {
    uint8_t event;
    uint16_t value;
};
// **** Define global, module-level, or external variables here ****
static struct AdcResult Adc;
// **** Declare function prototypes ****

int main(void)
{
    BOARD_Init();

    // Enable interrupts for the ADC
    ConfigIntADC10(ADC_INT_PRI_2 | ADC_INT_SUB_PRI_0 | ADC_INT_ON);

    // Set B2 to an input so AN0 can be used by the ADC.
    TRISBCLR = 1 << 2;

    // Configure and start the ADC
    // Read AN0 as sample a. We don't use alternate sampling, so setting sampleb is pointless.
    SetChanADC10(ADC_CH0_NEG_SAMPLEA_NVREF | ADC_CH0_POS_SAMPLEA_AN2);
    OpenADC10(
            ADC_MODULE_ON | ADC_IDLE_CONTINUE | ADC_FORMAT_INTG16 | ADC_CLK_AUTO | ADC_AUTO_SAMPLING_ON,
            ADC_VREF_AVDD_AVSS | ADC_OFFSET_CAL_DISABLE | ADC_SCAN_OFF | ADC_SAMPLES_PER_INT_8 |
            ADC_BUF_16 | ADC_ALT_INPUT_OFF,
            ADC_SAMPLE_TIME_29 | ADC_CONV_CLK_PB | ADC_CONV_CLK_51Tcy2,
            ENABLE_AN2_ANA,
            SKIP_SCAN_ALL
            );
    EnableADC10();

    /***************************************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     **************************************************************************************************/
    OledInit();
    while (1) {
        if (Adc.event) {
            char str[32];
            sprintf(str, "Potentiometer value:\n%d\n%d%%\n",
             Adc.value, (int) ((Adc.value / 1023.0)*100));
            OledClear();
            OledDrawString(str);
            OledUpdate();
            Adc.event=0;
        }        
    }
    /***************************************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     **************************************************************************************************/

    while (1);
}

/**
 * This is the ISR for the ADC1 peripheral. It runs continuously triggered by the hardware directly
 * and not called by any code, as all interrupts are. During each run it reads all 8 samples from
 * the ADC (registers ADC1BUF0 through ADC1BUF7), averages them, and stores them in a module-level
 * AdcResult struct. If this averaged ADC value has changed between this run and the last one, the
 * event flag in the AdcResult struct is set to true.
 */
void __ISR(_ADC_VECTOR, IPL2AUTO) AdcHandler(void)
{
    // Clear the interrupt flag.
    INTClearFlag(INT_AD1);
    int array[8];
    array[0] = ADC1BUF0;
    array[1] = ADC1BUF1;
    array[2] = ADC1BUF2;
    array[3] = ADC1BUF3;
    array[4] = ADC1BUF4;
    array[5] = ADC1BUF5;
    array[6] = ADC1BUF6;
    array[7] = ADC1BUF7;
    int i, avg, tmp = 0;
    for (i = 0; i < 8; i++) {
        tmp += array[i];
    }
    avg = (int) (tmp / 8.0);
    if (Adc.value ^ avg) {
        Adc.value = avg;
        Adc.event = 1;
    }

}