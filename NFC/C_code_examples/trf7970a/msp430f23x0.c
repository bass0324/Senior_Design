/****************************************************************
* FILENAME: msp430f23x0.c
*
* BRIEF: Contains MSP430F2370 specific functions.
*
* Copyright (C) 2010 Texas Instruments, Inc.
*
* AUTHOR(S): Reiser Peter		DATE: 02 DEC 2010
*
* EDITED BY:
* *
*
****************************************************************/

#include "msp430f23x0.h"
#include "trf797x.h"

//===============================================================

extern u08_t i_reg;
extern u08_t irq_flag;

//===============================================================
// NAME: void Msp430f23x0DileyMillisecond (u08_t n_ms)
//
// BRIEF: Is used to create delays.
//
// INPUTS:
//	Parameters:
//		u08_t		n_ms		delay time in ms
//	
// OUTPUTS:
//
// PROCESS:	[1] do loop of 1 ms duration as often as required
//
// CHANGE:
// DATE  		WHO	DETAIL
// 23Nov2010	RP	Original Code
//===============================================================

void
Msp430f23x0DelayMillisecond(u32_t n_ms)
{
    while (n_ms--)
    {	
    	__delay_cycles(DELAY_1ms);		// clock speed in Hz divined by 1000
    }
}

//===============================================================
// NAME: void Msp430f23x0CounterSet (void)
//
// BRIEF: Is used to set the timerA.
//
// INPUTS:
//	
// OUTPUTS:
//
// PROCESS:	[1] set timerA
//
// CHANGE:
// DATE  		WHO	DETAIL
// 23Nov2010	RP	Original Code
//===============================================================

void
Msp430f23x0CounterSet(void)
{
	TACTL |= TACLR;
	TACTL &= ~TACLR;				// reset the timerA
	TACTL |= TASSEL0 + ID1 + ID0;	// ACLK, div 8, interrupt enable, timer stoped
	
	TAR = 0x0000;
	TACCTL0 |= CCIE;				// compare interrupt enable
}

//===============================================================
// NAME: void Msp430f23x0OscSel (u08_t mode)
//
// BRIEF: Is used to select the oscilator.
//
// INPUTS:
//	Parameters:
//		u08_t		mode		crystal oscillator or DCO for
//						main clock
//	
// OUTPUTS:
//
// PROCESS:	[1] select the oscilator
//
// CHANGE:
// DATE  		WHO	DETAIL
// 23Nov2010	RP	Original Code
//===============================================================

void
Msp430f23x0OscSel(u08_t mode)
{

  	u08_t ii1 = 0;

  	if (mode == 0x00) 				          		// select crystal oscilator
    {	
    	BCSCTL1 |= XTS + XT2OFF;             		// ACLK = LFXT1 HF XTAL
        BCSCTL3 |= LFXT1S1;                       	// 3 � 16MHz crystal or resonator
        // the TRF796x sys_clk pin works as ocillator
        // it is set to 6.78 MHz (= 13.56 MHz / 2) in Trf797xInitialSettings()
		// turn external oscillator on
    	do
    	{
      		IFG1 &= ~OFIFG;                   		// Clear OSCFault flag
      		for (ii1 = 0xFF; ii1 > 0; ii1--) 		// Time delay for flag to set
      		{
      		}
    	} while ((IFG1 & OFIFG) == OFIFG);    		// OSCFault flag still set?
    	
    	BCSCTL2 |= SELM1 + SELM0 + SELS;    		// MCLK = SMCLK = HF LFXT1 (safe)

    	return;
    	}

  	else                           					//select DCO for main clock
    {	
    	DCOCTL |= DCO0 + DCO1 + DCO2;
    	BCSCTL1 |= XT2OFF + XTS + RSEL0 + RSEL1 + RSEL2;

    	BCSCTL2 &= ~(SELM1 + SELM0 + SELS + DCOR);

    	return;
    }
    												//_BIC_SR(OSCOFF);
}

//===============================================================
// NAME: void Msp430f23x0TimerAHandler (void)
//
// BRIEF: Is used to handle the interrupts generated by timerA.
//
// INPUTS:
//	
// OUTPUTS:
//	Globals:
//		u08_t		i_reg		indicates TIMEOUTs
//
//
// PROCESS:	[1] handle the interrupts generated by timerA
//
// CHANGE:
// DATE  		WHO	DETAIL
// 23Nov2010	RP	Original Code
//===============================================================

#pragma vector=TIMERA0_VECTOR
__interrupt void
Msp430f23x0TimerAHandler(void)
{	
	u08_t irq_status[4];

	STOP_COUNTER;
	
	irq_flag = 0x03;
	
	Trf797xReadIrqStatus(irq_status);

	*irq_status = *irq_status & 0xF7;				// set the parity flag to 0

	if(*irq_status == 0x00 || *irq_status == 0x80)
	{  	
		i_reg = 0x00;								// timer interrupt
	}
	else
	{	
		i_reg = 0x01;
	}
	//__low_power_mode_off_on_exit();	
}
