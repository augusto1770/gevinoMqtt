//      Definizione  ingressi e uscite e seriali GEVINO civile 1.1        

#include <Arduino.h>   // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function

#define     res_RS485_TxEn    digitalWrite(A0, LOW);
#define     set_RS485_TxEn    digitalWrite(A0, HIGH);
 
Uart Serial2 (&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);
void SERCOM1_Handler()  {   Serial2.IrqHandler(); }
Uart Serial3 (&sercom2, 3, 4, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM2_Handler()  {   Serial3.IrqHandler(); }

#define In1  digitalRead(A1)
#define In2  digitalRead(A2)
#define In3  digitalRead(A3)
#define In4  digitalRead(A4)
#define K1 6
#define K2 7
#define K3 8
#define K4 9
#define K5 42  // PA03
#define K6 38  // PA13
#define K7 A5
#define ResSim800  27  // PA28

#define setK1 digitalWrite(K1, HIGH)
#define resK1 digitalWrite(K1, LOW)
#define setK2 digitalWrite(K2, HIGH)
#define resK2 digitalWrite(K2, LOW)
#define setK3 digitalWrite(K3, HIGH)
#define resK3 digitalWrite(K3, LOW)
#define setK4 digitalWrite(K4, HIGH)
#define resK4 digitalWrite(K4, LOW)
#define setK5 digitalWrite(K5, HIGH)
#define resK5 digitalWrite(K5, LOW)
#define setK6 digitalWrite(K6, HIGH)
#define resK6 digitalWrite(K6, LOW)
#define setK7 digitalWrite(K7, HIGH)
#define resK7 digitalWrite(K7, LOW)

#define setLed  digitalWrite(LED_BUILTIN, HIGH) // Yellow front Led
#define resLed  digitalWrite(LED_BUILTIN, LOW)  // Yellow front Led
#define setResSim800  digitalWrite(ResSim800, HIGH)
#define resResSim800  digitalWrite(ResSim800, LOW)

void gevino_io_setup( void ){

//  Assign pins 10 & 11 SERCOM functionality
  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(11, PIO_SERCOM);
//  Assign pins 3 & 4 SERCOM functionality
  pinPeripheral(3, PIO_SERCOM_ALT);
  pinPeripheral(4, PIO_SERCOM_ALT);

  pinMode(A0, OUTPUT);       // TxEnable RS485
  res_RS485_TxEn;

  pinMode(A1, INPUT); // In01
  pinMode(A2, INPUT); // In02
  pinMode(A3, INPUT); // In03
  pinMode(A4, INPUT); // In04
  
// Define Output
  pinMode(K1, OUTPUT);       // Set K1 digital pin as Output
  pinMode(K2, OUTPUT);
  pinMode(K3, OUTPUT);
  pinMode(K4, OUTPUT);
  pinMode(K5, OUTPUT);
  pinMode(K6, OUTPUT);
  pinMode(K7, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT); // Yellow front Led
  pinMode(ResSim800, OUTPUT);
  resK1;
  resK2;
  resK3;
  resK4;
  resK5;
  resK6;  
  resK7;  
}

void testLeds( void ){       // Accende la maggior parte dei led per il collaudo
// Define Output

// Rx Tx leds Serial1 RS485
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);
  pinMode(1, OUTPUT);
  digitalWrite(1, LOW);
  pinMode(A0, OUTPUT);
  digitalWrite(A0, HIGH);
// Rx Tx leds Serial2  M-BUS
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
  pinMode(11, OUTPUT);
  digitalWrite(11, LOW);

  pinMode(LED_BUILTIN, OUTPUT); // Yellow front Led
  setLed;
  delay(200);
  resLed;

  pinMode(26, OUTPUT); // Tx Led
  digitalWrite(26, LOW);
  delay(200);
  digitalWrite(26, HIGH);
 
  pinMode(25, OUTPUT); // Rx Led
  digitalWrite(25, LOW);
  delay(200);
  digitalWrite(25, HIGH);

  pinMode(A1, OUTPUT); // In01
  digitalWrite(A1, HIGH);
  delay(200);
  digitalWrite(A1, LOW);
  
  pinMode(A2, OUTPUT); // In02
  digitalWrite(A2, HIGH);
  delay(200);
  digitalWrite(A2, LOW);
  
  pinMode(A3, OUTPUT); // In03
  digitalWrite(A3, HIGH);
  delay(200);
  digitalWrite(A3, LOW);
  
  pinMode(A4, OUTPUT); // In04
  digitalWrite(A4, HIGH);
  delay(200);
  digitalWrite(A4, LOW);
  
  pinMode(K1, OUTPUT);       // Set K1 digital pin as Output
  setK1;
  delay(200);
  resK1;
  
  pinMode(K2, OUTPUT);
  setK2;
  delay(200);
  resK2;
  
  pinMode(K3, OUTPUT);
  setK3;
  delay(200);
  resK3;
  
  pinMode(K4, OUTPUT);
  setK4;
  delay(200);
  resK4;
   
  pinMode(K5, OUTPUT);
  setK5;
  delay(200);
  resK5;
  
  pinMode(K6, OUTPUT);
  setK6;
  delay(200);
  resK6;

  pinMode(K7, OUTPUT);
  setK7;
  delay(200);
  resK7;
   
  delay(200);
  
  setLed;
  digitalWrite(26, HIGH);
  digitalWrite(25, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  digitalWrite(A4, HIGH);
  setK1;
  setK2;
  setK3;
  setK4;
  setK5;
  setK6;
  setK7;

  delay(2000);

  resLed;
  digitalWrite(26, LOW);
  digitalWrite(25, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  resK1;
  resK2;
  resK3;
  resK4;
  resK5;
  resK6;

}
