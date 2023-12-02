#include "debug.h"


u16 pbufa[2] = {0};
u16 pbufb[2] = {0};

void Input_Capture_Init(u16 arr, u16 psc)
{
    GPIO_InitTypeDef        GPIO_InitStructure = {0};
    TIM_ICInitTypeDef       TIM_ICInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_TIM1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_9);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x00;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;

    TIM_PWMIConfig(TIM1, &TIM_ICInitStructure);

    TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);
    TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);
    TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);

    TIM_DMAConfig(TIM1, TIM_DMABase_CCR1, TIM_DMABurstLength_1Transfer);
    TIM_DMACmd(TIM1, TIM_DMA_CC1, ENABLE );
    TIM_DMAConfig(TIM1, TIM_DMABase_CCR2, TIM_DMABurstLength_1Transfer);
    TIM_DMACmd(TIM1, TIM_DMA_CC2, ENABLE );

    TIM_Cmd(TIM1, ENABLE);
}

void TIM1_DMA_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);

    DMA_Cmd(DMA_CHx, ENABLE);
}


int main(void)
{
    SystemCoreClockUpdate();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

    Input_Capture_Init(0xFFFF, 48 - 1);

    TIM1_DMA_Init(DMA1_Channel2, (u32)(&TIM1->CH1CVR), (u32)pbufa, 1);
    TIM1_DMA_Init(DMA1_Channel3, (u32)(&TIM1->CH2CVR), (u32)pbufb, 1);


    while(1){
        printf( "Val:%d\t%d\r\n", pbufa[0], pbufb[0] );
        Delay_Ms(1);
    }
}

//void TIM1_CC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


//void TIM1_CC_IRQHandler(void)
//{
//    if( TIM_GetITStatus( TIM1, TIM_IT_CC1 ) != RESET )
//    {
//        prd = TIM_GetCapture1( TIM1 );
//
//        TIM_SetCounter( TIM1, 0 );
//    }
//
//    if( TIM_GetITStatus( TIM1, TIM_IT_CC2 ) != RESET )
//    {
//        dty = TIM_GetCapture2( TIM1 );
//    }
//
//    TIM_ClearITPendingBit( TIM1, TIM_IT_CC1 | TIM_IT_CC2 );
//}
