
#include "msp430g2553.h"
#include "stdbool.h"

#include "msp430_uart.h"
#include "msp430_adc.h"
#include "msp430_printf.h"
#include "INA219.h"

bool          ADCDone;             // ADC Done flag
unsigned int  ADCValue = 0;                // Measured ADC Value


void uart_rx(char data);


unsigned char TxData[2];
unsigned char TXByteCtr,RXByteCtr;
unsigned char RxBuf[6]={0x00,0x00,0x00,0x00,0x00,0x00};

void init_I2C(void) {
          UCB0CTL1 |= UCSWRST;                      // Enable SW reset
          UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
          UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
          UCB0BR0 = 10;                             // fSCL = 1Mhz/10 = ~100kHz
          UCB0BR1 = 0;
          UCB0I2CSA = 0x40;                   // Slave Address is 069h
          UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
          IE2 |= UCB0RXIE + UCB0TXIE;    // Enable RX and TX interrupt
}

int i2c_notready(){
        if(UCB0STAT & UCBBUSY) return 1;
        else return 0;
}


void Transmit(char registerAddr, char data){
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C start condition with UCTR flag for transmit
    while((IFG2 & UCB0TXIFG) == 0);  		//UCB0TXIFG is set immidiately
    UCB0TXBUF = registerAddr;               //write registerAddr in TX buffer
    while((IFG2 & UCB0TXIFG) == 0);			// wait until TX buffer is empty and transmitted
    UCB0TXBUF = data;               		//Write data in register
    while((IFG2 & UCB0TXIFG) == 0);			// wait until TX buffer is empty and transmitted
    UCB0CTL1 |= UCTXSTP;            		// I2C stop condition
    IFG2 &= ~UCB0TXIFG;						// Clear TX interrupt flag
}

unsigned char Receive(char registerAddr){
		unsigned char receivedByte;
        while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
        UCB0CTL1 |= UCTR + UCTXSTT;             // I2C start condition with UCTR flag for transmit
        while((IFG2 & UCB0TXIFG) == 0);  	//UCB0TXIFG is set immidiately
        UCB0TXBUF = registerAddr;           //write registerAddr in TX buffer
        while((IFG2 & UCB0TXIFG) == 0);		// wait until TX buffer is empty and transmitted
        UCB0CTL1 &= ~UCTR ;                // Clear I2C TX flag for receive
        UCB0CTL1 |= UCTXSTT + UCTXNACK;    // I2C start condition with NACK for single byte reading
        while (UCB0CTL1 & UCTXSTT);             // Start condition sent? RXBuffer full?
        receivedByte = UCB0RXBUF;
        UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
        return receivedByte;
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
 IE2 |= UCB0RXIE;                               
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
 //dmsg(" AAAAAA ",8);
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
 void dnum(unsigned char i)
 {
	 //unsigned char v = i >> 8;
	// dint(v);
	// v = (i & 0xF0);
	// dint(v);
	 
 }
 void dint(unsigned char i)
 {
	 
	 unsigned char v = i + 0x30;
	 UARTSendArray(&v,1);
		 /*
	 if (i / 100){
		 unsigned char v = (i/100) + 0x30;
		 UARTSendArray(&v,1);
		 i = i - (i/100);
	 }
	 if (i / 10){
		 unsigned char v = (i/10) + 0x30;
		 UARTSendArray(&v,1);
		  i = i - (i/10);
	 }
	 
	 if (i){
		 unsigned char v = (i) + 0x30;
		 UARTSendArray(&v,1);
	 }
	 */
 }
 void dmsg(unsigned char* str, unsigned char len)
 {
  unsigned char tmp = 0xff;
  UARTSendArray(&tmp,1);
  tmp = 0x20;
  UARTSendArray(&tmp,1);

   //unsigned char* logo ="Neo Test\n\r";
  UARTSendArray(str,len);
  tmp = 0x20;
  UARTSendArray(&tmp,1);
  tmp = 0xff;
  UARTSendArray(&tmp,1);

 }
int main(void)
{
 WDTCTL = WDTPW + WDTHOLD; // Stop WDT
 BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1Mhz
 DCOCTL = CALDCO_1MHZ;

 
 P1DIR |= BIT0; // Set the LEDs on P1.0, P1.6 as outputs
 //P1OUT = BIT0; // Set P1.0
 P1OUT = 0;
 uart_init(uart_rx);
   __delay_cycles(100000);	
   printf("%s\r","Neo Test now");
 while(1);
 //dnum(123);

 //
//          |             |   |
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
      __delay_cycles(10000); // 1khz delay
         while ( i2c_notready() );       // wait for bus to be free

  __bis_SR_register(GIE);  
  __delay_cycles(100000);	
  //UARTSendArray(&RxBuf[0],2);
  I2C_Txbyte(0,0);
  //I2C_Rxbyte(0x00);
  //dmsg(" CCCCCC ",8);

  //I2C_Rxbyte(0x00);
  
  //UARTSendArray(&RxBuf[0],2);
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
return 0;
}



//For I2C_TX
 __attribute__((interrupt(USCIAB0TX_VECTOR)))
void USCIAB0TX_ISR(void)
{
	//P1OUT ^= BIT0;
	// P1OUT |= BIT0;
	//dmsg("USCIAB0TX_ISR",13);
  if (IFG2 & UCB0TXIFG)
  {
	 // dmsg("TXIFG",5);
   if (TXByteCtr)                            // Check TX byte counter
   {
	   // dmsg(&TXByteCtr,1);
    TXByteCtr--;
    UCB0TXBUF =TxData[TXByteCtr];                 // Load TX buffer                               // Decrement TX byte counter
    I2C_WriteMode();
   }
   else
   {
	//dmsg("TX EXIT",7);
    UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
    IFG2&=~UCB0TXIFG;                     // Clear USCI_B0 TX int flag
    //IE2 &=~UCB0TXIE;
    __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
   }
  }
  if ( IFG2 & UCB0RXIFG )
    {
		// dmsg("RXIFG",5);
     if (RXByteCtr)
     {
		 // dmsg(&RXByteCtr,1);
      RXByteCtr--;
      if(RXByteCtr==0)
      {
	   //dmsg("RXByteCtr=0",11);
       UCB0CTL1 |= UCTXSTP+UCTXNACK;//Send Stop condition
      }
      RxBuf[RXByteCtr]=UCB0RXBUF;
      UCB0CTL1 &=~UCTXNACK;
     }
     else
     {
	   //dmsg("RX EXIT",7);
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
	//UARTSendArray(&data, 1);
		  switch (data){
		  case 'A':
			//Measure_REF(INCH_10,0);
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
