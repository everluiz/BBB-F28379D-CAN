#include "setup.h"
#include <math.h>
#include <stdio.h>



//------- DECLARACAO DE VARIAVEIS ----------


//------------------------------------------

__interrupt void isr_cpu_timer0(void);
__interrupt void isr_adc(void);
__interrupt void isr_CAN(void);

int main(void)
{


    // Step 1. Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    InitSysCtrl();
    EALLOW;
    // Step 3. Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    DINT;

    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    InitPieCtrl();

    // Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;

    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in F2837xD_DefaultIsr.c.
    InitPieVectTable();

    setup_GPIO();
    setup_ePWM();
    setup_ADC_A();
    setup_ADC_B();
    Setup_UART();


    EALLOW;
    PieVectTable.TIMER0_INT = &isr_cpu_timer0;   // Atribui a funcao de interrupcao a ser executada
    PieVectTable.ADCA1_INT = &isr_adc;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;           // habilita o Timer 0 na tabela de interrupcoes
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;           // habilita o ADC na tabela de interrupcoes
    EDIS;
    IER |= M_INT1;// habilita a linha 1 da tabela de interrupcao (timer e adc)

    InitCpuTimers();
    CpuTimer0Regs.TCR.bit.TIF = 0;                      // Inicializa o Timer0
    CpuTimer0Regs.TCR.bit.TIE = 0;
    CpuTimer0Regs.TCR.bit.TSS = 1;

    // Enable global Interrupts and higher priority real-time debug events:
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM


    while(1)
    {

    }
}

__interrupt void isr_cpu_timer0(void){
    //GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void isr_adc(void){
    //GpioDataRegs.GPATOGGLE.bit.GPIO10 = 1;


    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;          // Clear ADCINT1 flag for next SOC
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;         // Acknowledge interrupt to PIE
}
