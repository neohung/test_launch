/* --COPYRIGHT--,
 * Copyright (c) 2013, Barışcan Kayaoğlu
 * All rights reserved.
 *
 * --COPYRIGHT--*/
//******************************************************************************
//  MSP430G2xx3 I2C Demo - USCI_B0 I2C Master TX/RX single byte from ITG-3200
//  gyroscope.
//
//  Description: This demo connects a MSP430Gxx3 launchpad to ITG-3200 via
//  USCI_B0 I2C module. The launchpad act as a master and itg-300 act as a
//  slave. Master reads 6 byte consecutively and send it to the vitual COM
//  Port on Usb with USCI_A0 Uart module. DCO = 1MHz
//
//                                /|\  /|\
//                  ITG-3200      10k  10k     MSP430G2xx3
//                   slave         |    |        master
//             -----------------   |    |     -----------------
//            |              SDA|<-|----+---->|P1.7/UCB0SDA     |
//            |              CLK|<-|-.GND     |                 |
//            |              GND|<-|-'        |                 |
//            |              SCL|<-+--------->|P1.6/UCB0SCL     |
//            |              VCC|<--.         |      P1.1/UCA0TX|-------> COM Port (9600 baud)
//            |              VIO|<--'3.3V     |                 |
//            |              INT|             |                 |
//  Barışcan Kayaoğlu
//  May 2013
//  Built with CCS Version 5.3.0.00090
//******************************************************************************
#include <msp430g2553.h>

int TXByteCtr; // This counter used for counting bytes before the stop condition
char xRate_L;  // Variable to hold X_Low register value
char xRate_H;  // Variable to hold X_High register value
char yRate_L;  // Variable to hold Y_Low register value
char yRate_H;  // Variable to hold Y_High register value
char zRate_L;  // Variable to hold Z_Low register value
char zRate_H;  // Variable to hold Z_High register value
int i2c_State; // This counter held the state machine condition of I2C
int Rx = 0;    // Flag for the interrupt vector. 1 for Receive 0 for Transmit
int init = 0;  // Flag for the interrupt vector. 1 for Writing to gyro for config 0 for reading

//Gyro configuration addresses
char WHO_AM_I = 0x00;   // Gyro register where it helds the slave address 0x69 | 0x68
char SMPLRT_DIV= 0x15; // Gyro register where it helds the divider value for sample rate
char DLPF_FS = 0x16;  // Gyro register where it helds the low pass filter config

//Gyro Memory addresses;
char GYRO_XOUT_H = 0x1D;
char GYRO_XOUT_L = 0x1E;
char GYRO_YOUT_H = 0x1F;
char GYRO_YOUT_L = 0x20;
char GYRO_ZOUT_H = 0x21;
char GYRO_ZOUT_L = 0x22;
char TEMP_OUT_H = 0x1B;
char TEMP_OUT_L = 0x1C;
char itgAddress = 0x69;

//Gyro configuration constants
char DLPF_CFG_0 = 1<<0;        // 1
char DLPF_CFG_1 = 1<<1;       // 10
char DLPF_CFG_2 = 1<<2;      // 100
char DLPF_FS_SEL_0 = 1<<3;  // 1000
char DLPF_FS_SEL_1 = 1<<4; //10000

void init_I2C(void);
void Transmit(void);
void Receive(void);
void initUart(void);
void initGyro(void);

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1Mhz
  DCOCTL = CALDCO_1MHZ;
  //__delay_cycles(15000);                  // Wait gyro to wake up
  P1SEL |= BIT1 + BIT2 + BIT6 + BIT7;       // Assign I2C pins to USCI_B0 // Assign Uart pins to USCI_A0
  P2SEL |= BIT1 + BIT2 + BIT6 + BIT7;       // Assign I2C pins to USCI_B0 // Assign Uart pins to USCI_A0
  init_I2C();
  initUart();
  initGyro();
  init = 0;                                 // Flag to reading from configuration writing

  while(1) {
      switch(i2c_State) {
          case 0: // Wait state
            __delay_cycles(8000); // 10 ms
            i2c_State++;
            break;
          case 1: // Read X_High state
            Rx = 0; // Receive flag = transmit
            TXByteCtr = 1; // TXByteCtr 1 for sending register address and stop
            Transmit();  // I2C start condition with Write operation

            Rx = 1; // Receive flag = receive
            Receive();  // I2C start condition with Read operation
            i2c_State++;
            break;
          case 2: // Read X_Low state
            Rx = 0; // Receive flag = transmit
            TXByteCtr = 1; // TXByteCtr 1 for sending register address and stop
            Transmit();  // I2C start condition with Write operation

            Rx = 1; // Receive flag = receive
            Receive();  // I2C start condition with Read operation
            i2c_State++;
            break;
          case 3: // Read Y_High state
            Rx = 0; // Receive flag = transmit
            TXByteCtr = 1; // TXByteCtr 1 for sending register address and stop
            Transmit();  // I2C start condition with Write operation

            Rx = 1; // Receive flag = receive
            Receive();  // I2C start condition with Read operation
            i2c_State++;
            break;
          case 4: // Read Y_Low state
            Rx = 0; // Receive flag = transmit
            TXByteCtr = 1; // TXByteCtr 1 for sending register address and stop
            Transmit();  // I2C start condition with Write operation

            Rx = 1; // Receive flag = receive
            Receive();  // I2C start condition with Read operation
            i2c_State++;
            break;
          case 5: // Read Z_High state
            Rx = 0; // Receive flag = transmit
            TXByteCtr = 1; // TXByteCtr 1 for sending register address and stop
            Transmit();  // I2C start condition with Write operation

            Rx = 1; // Receive flag = receive
            Receive();  // I2C start condition with Read operation
            i2c_State++;
            break;
          case 6: // Read Z_Low state
            Rx = 0; // Receive flag = transmit
            TXByteCtr = 1; // TXByteCtr 1 for sending register address and stop
            Transmit();  // I2C start condition with Write operation

            Rx = 1; // Receive flag = receive
            Receive();  // I2C start condition with Read operation
            i2c_State++;
            break;
          case 7:
            UCA0TXBUF = 0x58; // X
            while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
            UCA0TXBUF = 0x3A; // :
            while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
            UCA0TXBUF = xRate_H;
            while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
            UCA0TXBUF = xRate_L;
            while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
            UCA0TXBUF = 0x09; // tab
            while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
            i2c_State++;
            break;
          case 8:
            UCA0TXBUF = 0x59; // Y
            while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
            UCA0TXBUF = 0x3A; // :
            while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
            UCA0TXBUF = yRate_H;
            while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
            UCA0TXBUF = yRate_L;
            while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
            UCA0TXBUF = 0x09; // tab
            while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
            i2c_State++;
            break;
          case 9:
            UCA0TXBUF = 0x5A; // Z
            while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
            UCA0TXBUF = 0x3A; // :
            while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
            UCA0TXBUF = zRate_H;
            while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
            UCA0TXBUF = zRate_L;
            while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
            UCA0TXBUF = 0x5C; // \
            while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
            UCA0TXBUF = 0x6E; // n
            while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
            i2c_State = 0;
            break;
      } // End of state machine
  }
} // End of Main


 void readX_H() {
      if(Rx == 1){                              // Master Recieve
          xRate_H = UCB0RXBUF;                  // Get RXBuffer value to xRate_H
      }

      else {                                     // Master Transmit
          if (TXByteCtr) {                       // TX byte counter = 1 : send gyro address
              UCB0TXBUF = GYRO_XOUT_H;           // Load TX buffer // request from X_High register
              TXByteCtr--;                       // Decrement TX byte counter
          }
          else {                                      // Tx byte counter = 0 : register address sent
              UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
              IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX interrupt flag
          }
      }
  }

  void readX_L() {
      if(Rx == 1){                              // Master Recieve
          xRate_L = UCB0RXBUF;                  // Get RXBuffer value to xRate_L
      }

      else {                                     // Master Transmit
          if (TXByteCtr) {                       // TX byte counter = 1 : send gyro address
              UCB0TXBUF = GYRO_XOUT_L;           // Load TX buffer // request from X_Low register
              TXByteCtr--;                       // Decrement TX byte counter
          }
          else {                                      // Tx byte counter = 0 : register address sent
              UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
              IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX interrupt flag
          }
      }
  }

  void readY_H() {
      if(Rx == 1) {                              // Master Recieve
          yRate_H = UCB0RXBUF;                   // Get RXBuffer value to yRate_H
      }

      else {                                         // Master Transmit
          if (TXByteCtr) {                           // TX byte counter = 1 : send gyro address
              UCB0TXBUF = GYRO_YOUT_H;               // Load TX buffer // request from Y_High register
              TXByteCtr--;                           // Decrement TX byte counter
          }
          else {                                        // Tx byte counter = 0 : register address sent
              UCB0CTL1 |= UCTXSTP;                      // I2C stop condition
              IFG2 &= ~UCB0TXIFG;                       // Clear USCI_B0 TX interrupt flag
          }
      }
  }

  void readY_L() {
      if(Rx == 1){                              // Master Recieve
          yRate_L = UCB0RXBUF;                  // Get RXBuffer value to yRate_L
      }

      else {                                     // Master Transmit
          if (TXByteCtr) {                       // TX byte counter = 1 : send gyro address
              UCB0TXBUF = GYRO_YOUT_L;           // Load TX buffer // request from Y_Low register
              TXByteCtr--;                       // Decrement TX byte counter
          }
          else {                                      // Tx byte counter = 0 : register address sent
              UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
              IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX interrupt flag
          }
      }
  }


  void readZ_H() {
      if(Rx == 1){                              // Master Recieve
          zRate_H = UCB0RXBUF;                  // Get RXBuffer value to zRate_H
      }

      else {                                     // Master Transmit
          if (TXByteCtr) {                       // TX byte counter = 1 : send gyro address
              UCB0TXBUF = GYRO_ZOUT_H;           // Load TX buffer // request from Z_High register
              TXByteCtr--;                       // Decrement TX byte counter
          }
          else {                                      // Tx byte counter = 0 : register address sent
              UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
              IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX interrupt flag
          }
      }
  }

  void readZ_L() {
      if(Rx == 1){                              // Master Recieve
          zRate_L = UCB0RXBUF;                   // Get RXBuffer value to zRate_L
      }

      else {                                     // Master Transmit
         if (TXByteCtr)  {                       // TX byte counter = 1 : send gyro address
             UCB0TXBUF = GYRO_ZOUT_L;            // Load TX buffer // request from Z_Low register
             TXByteCtr--;                        // Decrement TX byte counter
         }
         else {                                       // Tx byte counter = 0 : register address sent
              UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
              IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX interrupt flag
          }
      }
  }

  void initGyro(void) {
    TXByteCtr = 2;  // 2 bytes will occur. First register address send, Second Data send
    init = 1;       // config flag to 1 : Sending Divider value
    Transmit();     // Start condition with Writing mode
    init = 2;       // config flag to 2 : Sending Low pass filter value
    TXByteCtr = 2;  // Restart byte counter
    Transmit();     // Start condition with Writing mode
  }

  void init_I2C(void) {
      UCB0CTL1 |= UCSWRST;                      // Enable SW reset
      UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
      UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
      UCB0BR0 = 10;                             // fSCL = 1Mhz/10 = ~100kHz
      UCB0BR1 = 0;
      UCB0I2CSA = itgAddress;                   // Slave Address is 069h
      UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
      IE2 |= UCB0RXIE + UCB0TXIE;               // Enable RX and TX interrupt
  }
  void initUart(void) {
      UCA0CTL1 |= UCSSEL_2;                     // Use SMCLK
      UCA0BR0 = 104;                            // 1MHz 9600
      UCA0BR1 = 0;                              // 1MHz 9600
      UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
      UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  }
  void Transmit(void){
      while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
      UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX with start condition
      __bis_SR_register(CPUOFF + GIE);        // Enter LPM0 w/ interrupts
  }
  void Receive(void){
        while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
        UCB0CTL1 &= ~UCTR ;                     // Clear I2C TX flag
        UCB0CTL1 |= UCTXSTT;                    // I2C start condition
        while (UCB0CTL1 & UCTXSTT);             // Start condition sent?
        UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
        __bis_SR_register(CPUOFF + GIE);        // Enter LPM0 w/ interrupts
  }

  void twosCompToDecimal() {

  }

#pragma vector = USCIAB0TX_VECTOR       // USCI TX interrupt vector. I2C module share RX and TX interrupts.
__interrupt void USCIAB0TX_ISR(void)    // This will occur either when RXBuffer is full or TXBuffer is sent.
{
    if(init == 1) {                 // Gyro writing for configuration. Sending LPF register address
        if (TXByteCtr == 2) {       // byte counter = 2 : sending register address
            UCB0TXBUF = DLPF_FS;    // Low pass filter value register address to TXBuffer
            TXByteCtr--;            // Decrement TX byte counter
        }
        else if(TXByteCtr == 1) {                                       // byte counter = 1 : sending config value
              UCB0TXBUF = (DLPF_FS_SEL_0|DLPF_FS_SEL_1|DLPF_CFG_0);     // config value to TXBuffer
              TXByteCtr--;                                              // Decrement TX byte counter
        }
        else {                                      // byte counter = 0 : stop condition
            UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
            IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX int flag
            __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
        }
    }
    else if(init == 2) {                // Gyro writing for configuration. Sending Sample Rate Divider register address
        if (TXByteCtr == 2) {           // byte counter = 1 : sending config value
            UCB0TXBUF = SMPLRT_DIV;     // Sample Rate Divider value to TXBuffer
            TXByteCtr--;                // Decrement TX byte counter
        }
        else if(TXByteCtr == 1) {           // byte counter = 1 : sending config value
            UCB0TXBUF = 9;                  // config value to TXBuffer
            TXByteCtr--;                    // Decrement TX byte counter
        }
        else {                                      // byte counter = 0 : stop condition
            UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
            IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX int flag
            __bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
        }
    }
    else {  // Gyro Reading operation.
      switch(i2c_State) {
          case 1: // Read X High
              readX_H();
            __bic_SR_register_on_exit(CPUOFF);        // Exit LPM0
              break;
          case 2: // Read X Low
            readX_L();
            __bic_SR_register_on_exit(CPUOFF);        // Exit LPM0
              break;
          case 3: // Read Y High
            readY_H();
            __bic_SR_register_on_exit(CPUOFF);        // Exit LPM0
              break;
          case 4: // Read Y Low
            readY_L();
            __bic_SR_register_on_exit(CPUOFF);        // Exit LPM0
              break;
         case 5: // Read Z High
            readZ_H();
            __bic_SR_register_on_exit(CPUOFF);        // Exit LPM0
              break;
         case 6: // Read Z Low
            readZ_L();
            __bic_SR_register_on_exit(CPUOFF);        // Exit LPM0
              break;
      }
  }

}