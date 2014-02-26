

//#include <ServoDecode.h>

//#define serialEvent_implemented

#include "ppmrcin.h"
#include <Servo.h> 


unsigned int ppm_min=900;
unsigned int ppm_max=2100;

unsigned long currentMillis;
unsigned long interval0=5;
unsigned long preMillis0;
unsigned long load;

const int io_1_led_yellow = 7; // PD7
const int io_2_led_green  = 4; // PD4

const int out_switch = 8; // PB0

const int in_1_rud = A0; // PC0 - ADC0 - PCINT8
const int in_2_thr = A1; // PC1 - ADC1 - PCINT9
const int in_3_ele = A2; // PC2 - ADC2 - PCINT10
const int in_4_ail = A3; // PC3 - ADC3 - PCINT11

const int in_5_tbd  = 2; // PD2 - INT0  - PCINT18
const int in_6_mode = 3; // PD3 - OC2B - INT1 -PCINT19

const int adc_6 = A6; // ADC6
const int adc_7 = A7; // ADC7

const int out_1_rud = 6;  // OCR0A - PD6
const int out_2_thr = 5;  // OCR0B - PD5
const int out_3_ele = 9;  // OCR1A - PB1
const int out_4_ail = 10; // OCR1B - PB2


Servo servo_out1;
Servo servo_out2;
Servo servo_out3;
Servo servo_out4;



int time_in_1_rud = 0;
int time_in_2_thr = 0;
int time_in_3_ele = 0;
int time_in_4_ail = 0;
int time_in_5_tbd = 0;
int time_in_6_mode = 0;

unsigned int channel_1_rud;
unsigned int channel_2_thr;
unsigned int channel_3_ele;
unsigned int channel_4_ail;
unsigned int channel_5_tbd;
unsigned int channel_6_mode;

unsigned int serial_rx_1_rud;
unsigned int serial_rx_2_thr;
unsigned int serial_rx_3_ele;
unsigned int serial_rx_4_ail;

uint8_t buffer[14];

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(io_1_led_yellow, OUTPUT);   
  pinMode(io_2_led_green, OUTPUT);
  pinMode(out_switch, OUTPUT);  
  pinMode(out_1_rud, OUTPUT);
  pinMode(out_2_thr, OUTPUT);
  pinMode(out_3_ele, OUTPUT);
  pinMode(out_4_ail, OUTPUT);

  pinMode(in_1_rud,INPUT_PULLUP);
  pinMode(in_2_thr,INPUT_PULLUP);
  pinMode(in_3_ele,INPUT_PULLUP);
  pinMode(in_4_ail,INPUT_PULLUP);
  pinMode(in_5_tbd,INPUT_PULLUP);
  pinMode(in_6_mode,INPUT_PULLUP);

  pinMode(in_1_rud, INPUT);  
  pinMode(in_2_thr, INPUT);  
  pinMode(in_3_ele, INPUT);  
  pinMode(in_4_ail, INPUT);  
  pinMode(in_5_tbd, INPUT); 
  pinMode(in_6_mode, INPUT);   


  Serial.begin(38400); 
  Serial.println("Serial init"); 


  servo_out1.attach(out_1_rud);  
  servo_out1.writeMicroseconds(1500);
  servo_out2.attach(out_2_thr);  
  servo_out2.writeMicroseconds(1500);
  servo_out3.attach(out_3_ele);  
  servo_out3.writeMicroseconds(1500); 
  servo_out4.attach(out_4_ail);  
  servo_out4.writeMicroseconds(1500);
  Serial.println("Servos init"); 

  memset(buffer, 0, 14);
  buffer[0] = 0xff;
  buffer[1] = 0xfe;

  //   TCCR2A = 0x00;           // COM2A1=0, COM2A0=0 => Disconnect Pin OC0A from Timer/Counter 2 
  //   TCCR2B = 0x02;           // 16MHz clock with prescaler means TCNT1 increments every .5 uS (cs11 bit set)
  //   TIMSK2 = _BV (TOIE2);   // enable input capture and overflow interrupts for timer 2


  PCICR = (1 << PCIE1) | (1 << PCIE2);  
  PCIFR  = (1 << PCIF1) | (1 << PCIF2);

  PCMSK1 |= (1 << PCINT8) |(1 << PCINT9) | (1 << PCINT10)|(1 << PCINT11);   
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) ;
  sei();      
}


void loop() {

  currentMillis = micros();

  if(currentMillis - preMillis0 > interval0*1000)
  {
    preMillis0 = currentMillis;  

    if(channel_6_mode>1500){  
      digitalWrite(io_1_led_yellow, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(out_switch, HIGH);  // RC-Receiver active

    }
    else{
      digitalWrite(io_1_led_yellow, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(out_switch, LOW);  // uC active

      servo_out1.writeMicroseconds(serial_rx_1_rud);
      servo_out2.writeMicroseconds(serial_rx_2_thr);
      servo_out3.writeMicroseconds(serial_rx_3_ele);
      servo_out4.writeMicroseconds(serial_rx_4_ail);

    } 

    //  data.value=1234;
    //  low=input & 0xff
    //  high=(input>>8) & 0xff
    if(channel_1_rud<ppm_min || channel_1_rud >ppm_max)
      channel_1_rud=0;
    if(channel_2_thr<ppm_min || channel_2_thr >ppm_max)
      channel_2_thr=0; 
    if(channel_3_ele<ppm_min || channel_3_ele >ppm_max)
      channel_3_ele=0;  
    if(channel_4_ail<ppm_min || channel_4_ail >ppm_max)
      channel_4_ail=0;
    if(channel_5_tbd<ppm_min || channel_5_tbd >ppm_max)
      channel_5_tbd=0;
    if(channel_6_mode<ppm_min || channel_6_mode >ppm_max)
      channel_6_mode=0;


    buffer[2] = (channel_1_rud>>8) & 0xff;
    buffer[3] = channel_1_rud & 0xff  ;

    //data.value=channel_2_thr;
    buffer[4] = (channel_2_thr>>8) & 0xff;
    buffer[5] = channel_2_thr & 0xff  ;

    buffer[6] = (channel_3_ele>>8) & 0xff;
    buffer[7] = channel_3_ele & 0xff  ;

    buffer[8] = (channel_4_ail>>8) & 0xff;
    buffer[9] = channel_4_ail & 0xff  ;

    buffer[10] = (channel_5_tbd>>8) & 0xff;
    buffer[11] = channel_5_tbd & 0xff  ;

    buffer[12] = 0;
    buffer[13] = load  ;
    
    Serial.write((uint8_t*)&buffer, 14);

    
    load = ((float)(micros()-preMillis0))/10;
  } //close if intervallcheck
}


char active_interrupts=0;

ISR(PCINT1_vect) //our ISR
{
  //  cli();
  /*  Serial.print(digitalRead(in_1_rud)); 
   Serial.print(digitalRead(in_2_thr)); 
   Serial.print(digitalRead(in_3_ele));
   Serial.print(digitalRead(in_4_ail));
   Serial.print(digitalRead(in_5_tbd));
   Serial.println(digitalRead(in_6_mode));*/

  if(digitalRead(in_1_rud)==HIGH && !(active_interrupts & (1 << 0))) {
    time_in_1_rud = micros();
    active_interrupts |= (1 << 0) ;
  }
  if(digitalRead(in_1_rud)==LOW && active_interrupts& (1 << 0)){
    channel_1_rud=micros()-time_in_1_rud;
    active_interrupts &= ~(1 << 0) ;
  }


  if(digitalRead(in_2_thr)==HIGH && !(active_interrupts&2)){
    time_in_2_thr = micros();
    active_interrupts |= (1 << 1) ;  
  }
  if(digitalRead(in_2_thr)==LOW && active_interrupts&2){
    channel_2_thr=micros()-time_in_2_thr;
    active_interrupts &= ~(1 << 1) ;  
  }


  if(digitalRead(in_3_ele)==HIGH && !(active_interrupts&4)){
    time_in_3_ele = micros();
    active_interrupts |= (1 << 2) ;  
  }
  if(digitalRead(in_3_ele)==LOW && active_interrupts&4){
    channel_3_ele=micros()-time_in_3_ele;
    active_interrupts &= ~(1 << 2) ; 
  }


  if(digitalRead(in_4_ail)==HIGH && !(active_interrupts&8)){
    time_in_4_ail = micros();
    active_interrupts |= (1 << 3) ; 
  }
  if(digitalRead(in_4_ail)==LOW  && active_interrupts&8){
    channel_4_ail=micros()-time_in_4_ail;
    active_interrupts &= ~(1 << 3) ; 
  }

  // sei();
} //ISR

ISR(PCINT2_vect) {
  //  Serial.println(active_interrupts);

  if(digitalRead(in_5_tbd)==HIGH && !(active_interrupts&16)){
    time_in_5_tbd = micros();
    active_interrupts |= (1 << 4) ; 
  }
  if(digitalRead(in_5_tbd)==LOW  && active_interrupts&16){
    channel_5_tbd=micros()-time_in_5_tbd;
    active_interrupts &= ~(1 << 4) ; 
  }  

  if(digitalRead(in_6_mode)==HIGH  && !(active_interrupts&32) ){
    time_in_6_mode = micros();
    active_interrupts |= (1 << 5) ; 
  }
  if(digitalRead(in_6_mode)==LOW  && active_interrupts&32){   
    channel_6_mode=micros()-time_in_6_mode;
    active_interrupts &= ~(1 << 5) ;     
  }

}

unsigned char state=0;
unsigned char counter_byte=0;
unsigned char rx_message[10]={
  0};
unsigned char rx_byte=0;


void serialEvent() {

  while (Serial.available()) {
    rx_byte = (char)Serial.read();
  }


  switch(state){
  case 0:
    if(rx_byte==0xFF){
      state=1;
    }
    counter_byte=0;  
    break; 

  case 1:
    if(rx_byte==0xFE)
      state=2;
    else 
      state=0; 
    counter_byte=0;
    break;

  case 2:
    if(rx_byte<0xFE){
      rx_message[counter_byte]=rx_byte;
      counter_byte++;    
    }
    else
      state=0;
    if(counter_byte>7){
      state=3;
      counter_byte=0;
    }
    break;  
  case 3:
    //CRC Check;
    serial_rx_1_rud=(rx_message[0] << 8) + rx_message[1];
    serial_rx_2_thr=(rx_message[2] << 8) + rx_message[3];
    serial_rx_3_ele=(rx_message[4] << 8) + rx_message[5];
    serial_rx_4_ail=(rx_message[6] << 8) + rx_message[7];
    state=0;

    /* Serial.print(serial_rx_1_rud,DEC); 
     Serial.print(" ");
     Serial.print(serial_rx_2_thr,DEC); 
     Serial.print(" ");
     Serial.print(serial_rx_3_ele,DEC); 
     Serial.print(" ");
     Serial.println(serial_rx_4_ail,DEC); */
    break;
  }
}

ISR(TIMER1_OVF_vect){
  Serial.println("OVERFLOWED"); 

}


