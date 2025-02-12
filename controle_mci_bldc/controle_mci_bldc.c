#include "driverlib.h"
#include "device.h"
#include "board.h"

// Defines
#define EPWM1_TIMER_TBPRD   62500U
#define EPWM1_MAX_CMPA      57525U
#define EPWM1_MIN_CMPA      56025U

#define EPWM_CMP_UP         57525U//minima aceleração
#define EPWM_CMP_DOWN       56025U //Máxima acelaração
#define STEP                750 //define o passo do servo motor usado na aceleração (1500: de 0 a 1000)

#define VREF                3.3f  // Tensão de referência do ADC
#define ADC_MAX_COUNT       4095  // Valor máximo do ADC (12 bits)
#define REF                 2.63f //tensão equivalente a 48V

#define ADC_FILTER_SIZE 10
static float adcBuffer[ADC_FILTER_SIZE] = {0};
static uint8_t adcIndex = 0;

uint16_t compAValue;//Valor do duty cycle
uint16_t Vbus; // Tensão no barramento do sistema
uint16_t adcAResult0; //Leitura do adc
float voltage; // Tensão em Volts
float erro=0; //Erro do sistema
float erro1=0; //Erro do sistema

uint8_t i;

// Globals
typedef struct
{
    uint32_t epwmModule;
    uint16_t epwmCompADirection;
    uint16_t epwmCompBDirection;
    uint16_t epwmTimerIntCount;
    uint16_t epwmMaxCompA;
    uint16_t epwmMinCompA;
} epwmInformation;

epwmInformation epwm1Info;

// Function Prototypes
void initEPWM2();
__interrupt void adcA1ISR(void);
void initEPWM1(void);
__interrupt void epwm1ISR(void);
void updateCompare(epwmInformation *epwmInfo);

// Main
void main(void)
{
    Device_init();
    Device_initGPIO();
    Interrupt_initModule();
    Interrupt_initVectorTable();
    Interrupt_register(INT_EPWM1, &epwm1ISR);
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    Board_init();
    initEPWM1();
    initEPWM2();
    Interrupt_enable(INT_ADCA1);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    Interrupt_enable(INT_EPWM1);
    EINT;
    ERTM;
    EPWM_enableADCTrigger(EPWM2_BASE, EPWM_SOC_A);
    EPWM_setTimeBaseCounterMode(EPWM2_BASE, EPWM_COUNTER_MODE_UP);

    while(1);
}

// Função para configurar o ePWM2 para gerar o SOC.
void initEPWM2(void)
{
    EPWM_disableADCTrigger(EPWM2_BASE, EPWM_SOC_A);
    EPWM_setADCTriggerSource(EPWM2_BASE, EPWM_SOC_A, EPWM_SOC_TBCTR_U_CMPA);
    EPWM_setADCTriggerEventPrescale(EPWM2_BASE, EPWM_SOC_A, 1);
    EPWM_setCounterCompareValue(EPWM2_BASE, EPWM_COUNTER_COMPARE_A, 10713);
    EPWM_setTimeBasePeriod(EPWM2_BASE, 10714);
    EPWM_setClockPrescaler(EPWM2_BASE, EPWM_CLOCK_DIVIDER_8, EPWM_HSCLOCK_DIVIDER_14);
    EPWM_setTimeBaseCounterMode(EPWM2_BASE, EPWM_COUNTER_MODE_UP);

}

// ePWM1 ISR
__interrupt void epwm1ISR(void)
{
    updateCompare(&epwm1Info);
    EPWM_clearEventTriggerInterruptFlag(myEPWM1_BASE);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

// ADC A Interrupt 1 ISR utilizando o ePWM2
__interrupt void adcA1ISR(void)
{
    adcAResult0 = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);

    // Atualiza buffer circular
    adcBuffer[adcIndex] = (adcAResult0 * VREF) / ADC_MAX_COUNT;
    adcIndex = (adcIndex + 1) % ADC_FILTER_SIZE;

    // Calcula média dos últimos valores lidos
    voltage = 0;
    for (i = 0; i < ADC_FILTER_SIZE; i++)
        voltage += adcBuffer[i];

    voltage /= ADC_FILTER_SIZE;

    erro = REF - voltage;
    erro1 += erro;

    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    if (ADC_getInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER1))
    {
        ADC_clearInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER1);
        ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    }
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
// Configure ePWM1
void initEPWM1()
{
    epwm1Info.epwmCompADirection = EPWM_CMP_UP;
    epwm1Info.epwmCompBDirection = EPWM_CMP_DOWN;
    epwm1Info.epwmTimerIntCount = 0U;
    epwm1Info.epwmModule = myEPWM1_BASE;
    epwm1Info.epwmMaxCompA = EPWM1_MAX_CMPA;
    epwm1Info.epwmMinCompA = EPWM1_MIN_CMPA;

}

// Update Compare Values
void updateCompare(epwmInformation *epwmInfo)
{

    compAValue = EPWM_getCounterCompareValue(epwmInfo->epwmModule, EPWM_COUNTER_COMPARE_A);
    compAValue = (13500*erro1) - (18870*erro) + compAValue;

    // Limita o valor dentro do intervalo permitido
        if (compAValue > epwmInfo->epwmMaxCompA)
        {
            compAValue = epwmInfo->epwmMaxCompA;
        }
        else if (compAValue < epwmInfo->epwmMinCompA)
        {
            compAValue = epwmInfo->epwmMinCompA;
        }

        // Aplica o novo valor ao registrador do PWM
        EPWM_setCounterCompareValue(epwmInfo->epwmModule, EPWM_COUNTER_COMPARE_A, compAValue);

}
