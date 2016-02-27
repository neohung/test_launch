
#include "msp430g2553.h"
#include "stdbool.h"

#include "msp430_uart.h"
#include "msp430_adc.h"

#include "INA219.h"

bool          ADCDone;             // ADC Done flag
unsigned int  ADCValue = 0;                // Measured ADC Value

void Measure_REF(unsigned int chan, unsigned int ref);
void Measure(unsigned int chan);

void uart_rx(char data);


unsigned char TxData[2];
unsigned char TXByteCtr,RXByteCtr;
unsigned char RxBuf[6]={0x00};

void init_I2C(void) {
  UCB0CTL1 |= UCSWRST;                      // Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
  UCB0BR0 = 11;                             // fSCL = SMCLK/12 = ~100kHz
  UCB0BR1 = 0;
  P1SEL |= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0
  P1SEL2|= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0
  UCB0I2CSA = 0x40;
  UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation

}

void I2C_WriteMode(void)
{
 UCB0CTL1 |= UCTR;
 IFG2 &=~ UCB0TXIFG;
 IE2 &=~ UCB0RXIE;
 IE2 |= UCB0TXIE;
}
void I2C_ReadMode(void)
{
 UCB0CTL1 &=~ UCTR;
 IFG2 &=~ UCB0RXIFG;
 IE2 &=~ UCB0TXIE;
 IE2 |= UCB0RXIE;                                // 关闭发送中断，开启接收中断
}


void I2C_Txbyte(unsigned char Reg_addr,unsigned char Reg_data)
{
  while(UCB0STAT & UCBBUSY);
  I2C_WriteMode();
  TxData[1]=Reg_addr;
  TxData[0]=Reg_data;
  TXByteCtr=2;                                     // Load TX byte counter
  UCB0CTL1 |= UCTXSTT;                 // I2C TX, start condition
  //while (UCB0CTL1 & UCTXSTT);
  __bis_SR_register(CPUOFF + GIE);                   // Enter LPM0 w/ interrupts
  while (UCB0CTL1 & UCTXSTP);
}

void I2C_Rxbyte(unsigned char  Reg_addr)
{
 while(UCB0STAT & UCBBUSY);
 I2C_WriteMode();
 TxData[0]=Reg_addr;
 TXByteCtr=1;                                    // Load TX byte counter
 UCB0CTL1 |= UCTXSTT;                 // I2C TX, start condition
 __bis_SR_register(CPUOFF + GIE);                   // Enter LPM0 w/ interrupts
 while (UCB0CTL1 & UCTXSTP);
 I2C_ReadMode();
 RXByteCtr=1;
 UCB0CTL1 |= UCTXSTT;
 __bis_SR_register(CPUOFF + GIE);
 while (IFG2 & UCB0RXIFG);
 while (UCB0CTL1 & UCTXSTP);
}
/*
 adc_port1_3_init(&ADCValue);
 __enable_interrupt();
 while(1)
 {
	__delay_cycles(100000);				// Wait for ADC Ref to settle
	ADCDone = false;                         // Sets flag for main loop.
	adc_start();                		// Sampling and conversion start
	__bis_SR_register(CPUOFF + GIE);	// Low Power Mode 0 with interrupts enabled
	//ADCValue = ADCValue * 0.00293;
    if (ADCValue == 1023){
		char tmp;
		tmp = 0xAA;
		UARTSendArray(&tmp,1);
		tmp = 0xFF;
		UARTSendArray(&tmp,1);
		UARTSendArray(&ADCValue,2);
		tmp = 0xFF;
		UARTSendArray(&tmp,1);
		tmp = 0xAA;
		UARTSendArray(&tmp,1);	
		P1DIR |= BIT0;
	}else{
		P1DIR &= ~BIT0;
	}
	if (ADCValue == 0){
		adc_stop();
		__bis_SR_register(CPUOFF + GIE);
	}
	
	//UARTSendArray((ADCValue >> 8)& 0x00FF,1);
 }

 
 //LPM0_bits
 //
  TA0CTL|=TACLR+TAIE;  //开启中断并清零
   TA0CCR0 = 32768;
   TA0CTL = TASSEL_2+ID_3+ MC_1;  // SMCLK, div 8, up mode,
   TACCTL0 = CCIE;        // Enable interrupts for CCR0.

 */
void main(void)
{
 WDTCTL = WDTPW + WDTHOLD; // Stop WDT
 BCSCTL1 = CALBC1_1MHZ; // Set DCO to 1MHz
 DCOCTL = CALDCO_1MHZ; // Set DCO to 1MHz
 
 P1DIR |= BIT0; // Set the LEDs on P1.0, P1.6 as outputs
 //P1OUT = BIT0; // Set P1.0
 P1OUT = 0;
 uart_init(uart_rx);
   __delay_cycles(100000);	
 unsigned char* logo ="Neo Test";
  UARTSendArray(logo,8);
 //
//         /|\           /|\ /|\
//          |   TMP100   10k 10k     MSP430G2xx3
//          |   -------   |   |   -------------------
//          +--|Vcc SDA|<-|---+->|P1.7/UCB0SDA    XIN|-
//          |  |       |  |      |                   |
//          +--|A1,A0  |  |      |               XOUT|-
//             |       |  |      |                   |
//          +--|Vss SCL|<-+------|P1.6/UCB0SCL   P1.0|---> LED
//         \|/  -------          |                   |
//
//INA219_begin(INA219_ADDRESS);

  init_I2C();
     __bis_SR_register(GIE);  
  __delay_cycles(100000);	
  I2C_Rxbyte(0x00);
  I2C_Rxbyte(0x00);
  UARTSendArray(&RxBuf[0],2);
  while(1){
	  //__delay_cycles(100000);	
	  // UARTSendArray(&RxBuf[0],2);
	   __bis_SR_register(CPUOFF + GIE);  
  }
  
//Send I2C TX
  while (1)
  {
	__delay_cycles(100000);	
    TXByteCtr = 1;                          // Load TX byte counter
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    
	UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition
    //__bis_SR_register(CPUOFF + GIE);        // Enter LPM0 w/ interrupts
                                            // Remain in LPM0 until all data
                                            // is TX'd
   // TXData++;                               // Increment data byte


   // __delay_cycles(500000);                 // 0.5s Delay so the data is readable

  }

  //Read I2C_RX
  while (1)
      {
        while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
        UCB0CTL1 |= UCTXSTT;                    // I2C start condition
        while (UCB0CTL1 & UCTXSTT);             // Start condition sent?
        UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
        __bis_SR_register(CPUOFF + GIE);        // Enter LPM0 w/ interrupts
      }

 
 /*
while(1)
{
	//UARTSendArray("ABC", 3);
	//__delay_cycles(500000);     
	if(ADCDone)            // If the ADC is done with a measurement
    {
		ADCDone = false;            // Clear flag
		UARTSendArray(ADCValue & 0x00FF,1);
		UARTSendArray((ADCValue >> 8)& 0x00FF,1);
	}
	//LPM0_bits+
	__bis_SR_register(GIE); // Enter LPM0, interrupts enabled
}
*/
}



//For I2C_TX
 __attribute__((interrupt(USCIAB0TX_VECTOR)))
void USCIAB0TX_ISR(void)
{
	P1OUT ^= BIT0;
	// P1OUT |= BIT0;
  if (IFG2 & UCB0TXIFG)
  {
   if (TXByteCtr)                            // Check TX byte counter
   {
    TXByteCtr--;
    UCB0TXBUF =TxData[TXByteCtr];                 // Load TX buffer                               // Decrement TX byte counter
   }
   else
   {
    UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
    IFG2&=~UCB0TXIFG;                     // Clear USCI_B0 TX int flag
    //IE2 &=~UCB0TXIE;
    __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
   }
  }
  if ( IFG2 & UCB0RXIFG )
    {
     if (RXByteCtr)
     {
      RXByteCtr--;
      if(RXByteCtr==0)
      {
       UCB0CTL1 |= UCTXSTP+UCTXNACK;//Send Stop condition
      }
      RxBuf[RXByteCtr]=UCB0RXBUF;
      UCB0CTL1 &=~UCTXNACK;
     }
     else
     {
      //UCB0CTL1 |= UCTXSTP;
      IFG2&=~UCB0RXIFG;
       __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
     }
    }
}


/*
//For I2C RX
 // USCI_B0 Data ISR
 __attribute__((interrupt(USCIAB0TX_VECTOR)))
  void USCIAB0TX_ISR(void)
    {
      RXData = UCB0RXBUF;                       // Get RX data
      __bic_SR_register_on_exit(CPUOFF);        // Exit LPM0
    }
*/

void uart_rx(char data)
{
	UARTSendArray(&data, 1);
		  switch (data){
		  case 'A':
			Measure_REF(INCH_10,0);
		  break;
		  default:
		  break;
	  }

}

__attribute__((interrupt(TIMER0_A0_VECTOR)))
void neo_TimerA_procedure(void)
{ 

//P1OUT ^= BIT0;
//TA0CCR0 += 32768;    // Add offset to CCR0
//__bic_SR_register_on_exit(CPUOFF);
// Enable CPU so the main while loop continues

}


/****************************************************************************************
* Reads ADC 'chan' once using an internal reference, 'ref' determines if the
*   1.5V reference is used.
**************************************************************************************/
void Measure_REF(unsigned int chan, unsigned int ref)
{
    ADC10CTL0 &= ~ENC;                     // Disable ADC
    ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ref + ADC10IE;
                        // Use reference,
                           // 16 clock ticks, internal reference on
                     // ADC On, enable ADC interrupt, Internal = 'ref'
    ADC10CTL1 = ADC10SSEL_3 + chan;     // Set 'chan', SMCLK
    __delay_cycles (128);               // Delay to allow Ref to settle
    ADC10CTL0 |= ENC + ADC10SC;         // Enable and start conversion
}

/************************************************************************************
* Reads ADC 'chan' once using AVCC as the reference.
****************************************************************************************/
void Measure(unsigned int chan)
{
    ADC10CTL0 &= ~ENC;                       // Disable ADC
    ADC10CTL0 = ADC10SHT_3 + ADC10ON + ADC10IE;
                                        // 16 clock ticks, ADC On, enable ADC interrupt
    ADC10CTL1 = ADC10SSEL_3 + chan;        // Set 'chan', SMCLK
    ADC10CTL0 |= ENC + ADC10SC;         // Enable and start conversion
}
