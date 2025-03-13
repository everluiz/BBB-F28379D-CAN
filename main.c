#include "setup.h"
#include <math.h>
#include <stdio.h>



//------- DECLARACAO DE VARIAVEIS ----------

// CAN
extern tCANMsgObject TxCANMsgObj;
extern tCANMsgObject RxCANMsgObj;
extern unsigned char TxCANMsg[8];
extern unsigned char RxCANMsg[8];
unsigned char CAX_receive = 0;

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
    PieVectTable.CANB0_INT = &isr_CAN;

    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;           // habilita o Timer 0 na tabela de interrupcoes
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;           // habilita o ADC na tabela de interrupcoes
    PieCtrlRegs.PIEIER9.bit.INTx7 = 1;           // CAN B
    EDIS;
    IER |= M_INT1|M_INT9;// habilita a linha 1 e 9 da tabela de interrupcao (timer e adc, CAN)

    InitCpuTimers();
    CpuTimer0Regs.TCR.bit.TIF = 0;                      // Inicializa o Timer0
    CpuTimer0Regs.TCR.bit.TIE = 0;
    CpuTimer0Regs.TCR.bit.TSS = 1;
    ConfigCpuTimer(&CpuTimer0, 200, 1000000);

    // Enable global Interrupts and higher priority real-time debug events:
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM


    while(1)
    {

    }
}

__interrupt void isr_cpu_timer0(void){
    TxCANMsg[0] =  (((int8_t)(1500.0)) & 0xFF00) >> 8;
    TxCANMsg[1] =  ((int8_t)(1500.0)) & 0x00FF;

    TxCANMsg[2] =  (((int8_t)(500.0)) & 0xFF00) >> 8;
    TxCANMsg[3] =  ((int8_t)(-500.0)) & 0x00FF;

    TxCANMsg[4] =  (((int8_t)(-2000.0)) & 0xFF00) >> 8;
    TxCANMsg[5] =  ((int8_t)(-2000.0)) & 0x00FF;

    CANMessageSet(CANB_BASE, 0x01, &TxCANMsgObj, MSG_OBJ_TYPE_TX);
    GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void isr_adc(void){
    //GpioDataRegs.GPATOGGLE.bit.GPIO10 = 1;


    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;          // Clear ADCINT1 flag for next SOC
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;         // Acknowledge interrupt to PIE
}

__interrupt void isr_CAN(void){
    unsigned long ulStatus;
    ulStatus = CANIntStatus(CANB_BASE, CAN_INT_STS_CAUSE);
    if(ulStatus == CAN_INT_INT0ID_STATUS){
        ulStatus = CANStatusGet(CANB_BASE, CAN_STS_CONTROL);
        if(((ulStatus  & ~(CAN_ES_TXOK | CAN_ES_RXOK)) != 7) && ((ulStatus  & ~(CAN_ES_TXOK | CAN_ES_RXOK)) != 0)){
            // Set a flag to indicate some errors may have occurred.
        }
    }else if(ulStatus == 1){
        CANIntClear(CANB_BASE, 1);
    }else if(ulStatus == 2){
        // Receive msg to ID 0x02
        CANMessageGet(CANB_BASE, 2, &RxCANMsgObj, true);
        CAX_receive = 1;

        CANIntClear(CANB_BASE, 2);
    }else{
        // Spurious interrupt handling can go here.
    }
    CANGlobalIntClear(CANB_BASE, CAN_GLB_INT_CANINT0);
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}
