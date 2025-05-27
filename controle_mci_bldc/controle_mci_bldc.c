
#include "driverlib.h"
#include "device.h"
#include "board.h"

#define EPWM1_TIMER_TBPRD   62500U
#define EPWM1_MAX_CMPA      57300U //57525 = 18,41ms 92,04%
#define EPWM1_MIN_CMPA      56250U //56025 = 17,92ms 89,64%

//#define EPWM_CMP_UP          62075U
//#define EPWM_CMP_DOWN        52650U
#define EPWM_CMP_UP            57300U //min 57525U
#define EPWM_CMP_DOWN          56250U //max 56025U
#define STEP                   1050 //1500 de 0 a 1000

uint16_t compAValue;

uint16_t Vbus; //tensão no barramento do sistema
//
// Globals
//
typedef struct
{
    uint32_t epwmModule;
    uint16_t epwmCompADirection;
    uint16_t epwmCompBDirection;
    uint16_t epwmTimerIntCount;
    uint16_t epwmMaxCompA;
    uint16_t epwmMinCompA;
}epwmInformation;

//
// Globals to hold the ePWM information used in this example
//
epwmInformation epwm1Info;

//
// Function Prototypes
//
void initEPWM1(void);
__interrupt void epwm1ISR(void);

void updateCompare(epwmInformation *epwmInfo);

//
// Main
//
void main(void)
{
    //
    // Initialize device clock and peripherals
    //
    Device_init();

    //
    // Disable pin locks and enable internal pull ups.
    //
    Device_initGPIO();

    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // Assign the interrupt service routines to ePWM interrupts
    //
    Interrupt_register(INT_EPWM1, &epwm1ISR);


    //
    // Disable sync(Freeze clock to PWM as well)
    //
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    //
    // Configure GPIO0/1 , GPIO2/3 and GPIO4/5 as ePWM1A/1B, ePWM2A/2B and
    // ePWM3A/3B pins respectively
    // Configure EPWM Modules
    //
    Board_init();

    initEPWM1();

    //
    // Enable sync and clock to PWM
    //
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    //
    // Enable ePWM interrupts
    //
    Interrupt_enable(INT_EPWM1);

    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;

    //
    // IDLE loop. Just sit and loop forever (optional):
    //
    //
    while(1)
       {
           //
           // Convert, wait for completion, and store results
           //
           ADC_forceMultipleSOC(Vbus_BASE, (ADC_FORCE_SOC0 | ADC_FORCE_SOC1));

           //
           // Wait for ADCA to complete, then acknowledge flag
           //
           while(ADC_getInterruptStatus(Vbus_BASE, ADC_INT_NUMBER1) == false)
           {
           }
           ADC_clearInterruptStatus(Vbus_BASE, ADC_INT_NUMBER1);

           //
           // Store results
           //
           Vbus = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);

           //
           // Software breakpoint. At this point, conversion results are stored in
           // myADC0Result0, myADC0Result1, myADC1Result0, and myADC1Result1.
           //
           // Hit run again to get updated conversions.
           //
           //ESTOP0;
       }
}

//
// epwm1ISR - ePWM 1 ISR
//
__interrupt void epwm1ISR(void)
{
    //
    // Update the CMPA and CMPB values
    //
    updateCompare(&epwm1Info);

    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(myEPWM1_BASE);

    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

//
// initEPWM1 - Configure ePWM1
//
void initEPWM1()
{
    //
    // Information this example uses to keep track of the direction the
    // CMPA/CMPB values are moving, the min and max allowed values and
    // a pointer to the correct ePWM registers
    //
    epwm1Info.epwmCompADirection = EPWM_CMP_UP;
    epwm1Info.epwmCompBDirection = EPWM_CMP_DOWN;
    epwm1Info.epwmTimerIntCount = 0U;
    epwm1Info.epwmModule = myEPWM1_BASE;
    epwm1Info.epwmMaxCompA = EPWM1_MAX_CMPA;
    epwm1Info.epwmMinCompA = EPWM1_MIN_CMPA;
}

//
// updateCompare - Function to update the frequency
//
void updateCompare(epwmInformation *epwmInfo)
{


    compAValue = EPWM_getCounterCompareValue(epwmInfo->epwmModule,
                                             EPWM_COUNTER_COMPARE_A);


    //
    //  Change the CMPA/CMPB values every 10th interrupt.
    //


    if(epwmInfo->epwmTimerIntCount == 500U)
    {

        GPIO_togglePin(GPIO0);

        epwmInfo->epwmTimerIntCount = 0U;

        //
        // If we were increasing CMPA, check to see if we reached the max
        // value. If not, increase CMPA else, change directions and decrease
        // CMPA
        //
        if(epwmInfo->epwmCompADirection == EPWM_CMP_UP)
        {
            if(compAValue < (epwmInfo->epwmMaxCompA))
            {
                compAValue = STEP + compAValue;
                EPWM_setCounterCompareValue(epwmInfo->epwmModule,
                                            EPWM_COUNTER_COMPARE_A,
                                            compAValue);
            }
            else
            {
                compAValue = compAValue - STEP;
                epwmInfo->epwmCompADirection = EPWM_CMP_DOWN;
                EPWM_setCounterCompareValue(epwmInfo->epwmModule,
                                            EPWM_COUNTER_COMPARE_A,
                                            compAValue);
            }
        }
        //
        // If we were decreasing CMPA, check to see if we reached the min
        // value. If not, decrease CMPA else, change directions and increase
        // CMPA
        //
        else
        {
            if( compAValue == (epwmInfo->epwmMinCompA))
            {
                compAValue = STEP + compAValue;
                epwmInfo->epwmCompADirection = EPWM_CMP_UP;
                EPWM_setCounterCompareValue(epwmInfo->epwmModule,
                                            EPWM_COUNTER_COMPARE_A,
                                            compAValue);
            }
            else
            {
                compAValue = compAValue - STEP;
                EPWM_setCounterCompareValue(epwmInfo->epwmModule,
                                            EPWM_COUNTER_COMPARE_A,
                                            compAValue);
            }
        }



        DEVICE_DELAY_US(1000);


    }
    else
    {
        epwmInfo->epwmTimerIntCount++;
    }
}
