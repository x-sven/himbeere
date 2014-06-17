

//#include <ServoDecode.h>

//#define serialEvent_implemented

#include <Servo.h> 
#include "crc16.h"
#include "c_parser.h"
#include "c_dropbox.h"
#include "c_servochannels.h"

// ----- Global variables ---------
uint32_t current_micros;         // time in micro seconds for main loop
uint32_t current_irq_micros;     // time in micro seconds for "last" interrupt

uint32_t interval0  = 20;        // 20 ms -> 50 Hz main loop intervall
uint32_t preMillis0 = 0;         // storage for last main loop 

uint32_t load;                   // value for "spare time" as cpu load indicator

volatile boolean pc_int0=false;  // interrupt flag, true if interrupt (int0)  was triggered
volatile boolean pc_int1=false;  // interrupt flag, true if interrupt (int0)  was triggered
volatile boolean pc_ser =false;  // interrupt flag, true if a serial event (serialEvent)  was triggered

byte active_interrupts = 0;      // byte holding status bits for interrupts according to single input pins
unsigned char rx_byte  = 0;      // memory for byte received during last serial event

Servo servo_out1;         // Servo object for output 1
Servo servo_out2;         // Servo object for output 2
Servo servo_out3;         // Servo object for output 3
Servo servo_out4;         // Servo object for output 4

int time_in_1_rud = 0;    // micros of rising edge for rudder input channel 
int time_in_2_thr = 0;    // micros of rising edge for throttle input channel
int time_in_3_ele = 0;    // micros of rising edge for elevator input channel
int time_in_4_ail = 0;    // micros of rising edge for aileron input channel
int time_in_5_tbd = 0;    // micros of rising edge for TBD input channel
int time_in_6_mode = 0;   // micros of rising edge for mode input channel

cParser parser;
cDropbox dropbox;

cServoChannels channel_raspi(4);
cServoChannels channel_pilot(6);

// ----- I/O pin assignement---------
const int io_1_led_yellow = 7; // PD7
const int io_2_led_green  = 4; // PD4

const int out_switch = 8;      // PB0 - switch between receiver and fcci servo commands

const int in_1_rud = A0;       // PC0 - ADC0 - PCINT8
const int in_2_thr = A1;       // PC1 - ADC1 - PCINT9
const int in_3_ele = A2;       // PC2 - ADC2 - PCINT10
const int in_4_ail = A3;       // PC3 - ADC3 - PCINT11

const int in_5_tbd  = 2;       // PD2 - INT0 - PCINT18
const int in_6_mode = 3;       // PD3 - OC2B - INT1 - PCINT19

const int adc_6 = A6;          // ADC6
const int adc_7 = A7;          // ADC7

const int out_1_rud = 6;       // OCR0A - PD6
const int out_2_thr = 5;       // OCR0B - PD5
const int out_3_ele = 9;       // OCR1A - PB1
const int out_4_ail = 10;      // OCR1B - PB2


// ----- the interrupt routines  -----
ISR(PCINT1_vect) // PCINT 1 interrupt routine, triggered on pin change of A0...A3
{
  current_irq_micros=micros();
  pc_int0=true;
} //ISR

ISR(PCINT2_vect) // PCINT 2 interrupt routine, triggered on pin change of D2 & D3
{
  current_irq_micros=micros();
  pc_int1=true;

}

void serialEvent() // Function called when a char was received
{
  rx_byte = (char)Serial.read();
  pc_ser=true;
}

// ----- the setup routine runs once when you press reset -----
void setup() 
{                
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

  servo_out1.attach(out_1_rud);  
  servo_out1.writeMicroseconds(1500);
  servo_out2.attach(out_2_thr);  
  servo_out2.writeMicroseconds(1500);
  servo_out3.attach(out_3_ele);  
  servo_out3.writeMicroseconds(1500); 
  servo_out4.attach(out_4_ail);  
  servo_out4.writeMicroseconds(1500);

  // enable pin change interrupts for pins used for input
  PCICR = (1 << PCIE1) | (1 << PCIE2);  
  PCIFR = (1 << PCIF1) | (1 << PCIF2);

  PCMSK1 |= (1 << PCINT8) |(1 << PCINT9) | (1 << PCINT10)|(1 << PCINT11);   
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) ;
  sei();      
}

// ----- the main routine runs as end less while loop -----
void loop() 
{
  current_micros = micros();
  if ( current_micros - preMillis0 > interval0*1000 ) // slow down to desired update rate
  {
    preMillis0 = current_micros;  

    if(1500 < channel_pilot[mod_6]) // check how should fly the copter (all above 1500us)
    {  
      digitalWrite(io_1_led_yellow, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(out_switch, HIGH);        // RC-Receiver active
    }
    else
    {
      digitalWrite(io_1_led_yellow, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(out_switch, LOW);         // uC active

      if(parser.new_channels())
        channel_raspi = parser.get_channels();
        
      if(channel_raspi.valid())
      {
        // set servo/ ESC output
        servo_out1.writeMicroseconds(channel_raspi[rud_1]);
        servo_out2.writeMicroseconds(channel_raspi[thr_2]);
        servo_out3.writeMicroseconds(channel_raspi[ele_3]);
        servo_out4.writeMicroseconds(channel_raspi[ail_4]);
      }// channel_raspi valid
    }// if (mode)

    // set channel values in preparation to send them to raspi    
    dropbox.set_channels(channel_pilot);
    // estimate cpu load
    dropbox.set_load( ((float)(micros()-preMillis0))/10 ); // ToDo: we should indicate overload!?
    // commit parse errors
    dropbox.set_errors(parser.get_errors());
    // finally, send the message to raspi
    dropbox.send_message();
    
  } // end of "slow" update rate (e.g. 50Hz)

  //  ----------------------------------------------------------------------
  //  ----- now process  PC INT 0 FLAG (as fast as we can!?)           ----- 
  //        two steps: first remember time of rising edge, 
  //        then calc. channels pulse width after detecting falling edge
  //  ----------------------------------------------------------------------
  if(pc_int0){
    if(digitalRead(in_1_rud)==HIGH && !(active_interrupts&1 )) {
      time_in_1_rud = current_irq_micros;
      active_interrupts |= (1 << 0) ;
    }
    if(digitalRead(in_1_rud)==LOW && active_interrupts&1){
      channel_pilot[rud_1]=current_irq_micros-time_in_1_rud;
      active_interrupts &= ~(1 << 0) ;
    }
    if(digitalRead(in_2_thr)==HIGH && !(active_interrupts&2)){
      time_in_2_thr = current_irq_micros; 
      active_interrupts |= (1 << 1) ;  
    }
    if(digitalRead(in_2_thr)==LOW && active_interrupts&2){
      channel_pilot[thr_2]=current_irq_micros-time_in_2_thr;
      active_interrupts &= ~(1 << 1) ;  
    }
    if(digitalRead(in_3_ele)==HIGH && !(active_interrupts&4)){
      time_in_3_ele = current_irq_micros;
      active_interrupts |= (1 << 2) ;  
    }
    if(digitalRead(in_3_ele)==LOW && active_interrupts&4){
      channel_pilot[ele_3]=current_irq_micros-time_in_3_ele;
      active_interrupts &= ~(1 << 2) ; 
    }
    if(digitalRead(in_4_ail)==HIGH && !(active_interrupts&8)){
      time_in_4_ail = current_irq_micros;
      active_interrupts |= (1 << 3) ; 
    }
    if(digitalRead(in_4_ail)==LOW  && active_interrupts&8){
      channel_pilot[ail_4]=current_irq_micros-time_in_4_ail;
      active_interrupts &= ~(1 << 3) ; 
    }
    pc_int0=false;
  }

  //  ---------------------------------------------------------------------------
  //  ----- now process  PC INT 1 FLAG (as fast as we can!?)                ----- 
  //  ---------------------------------------------------------------------------  
  if(pc_int1){
    if(digitalRead(in_5_tbd)==HIGH && !(active_interrupts&16)){
      time_in_5_tbd = current_irq_micros;
      active_interrupts |= (1 << 4) ; 
    }
    if(digitalRead(in_5_tbd)==LOW  && active_interrupts&16){
      channel_pilot[tbd_5]=current_irq_micros-time_in_5_tbd;
      active_interrupts &= ~(1 << 4) ; 
    }
    if(digitalRead(in_6_mode)==HIGH  && !(active_interrupts&32) ){
      time_in_6_mode = current_irq_micros;
      active_interrupts |= (1 << 5) ; 
    }
    if(digitalRead(in_6_mode)==LOW  && active_interrupts&32){   
      channel_pilot[mod_6]=current_irq_micros-time_in_6_mode;
      active_interrupts &= ~(1 << 5) ;     
    }
    pc_int1=false;
  }

  //  ---------------------------------------------------------------------------
  //  ----- now process  PC Serial data (as fast as we can!?)               ----- 
  //  ---------------------------------------------------------------------------
  if(pc_ser)
  {
    parser.process_byte(rx_byte);
    pc_ser=false;
  }

  //-------------------------------------------------------------------------------
  //--------------------------------Timing Checker-----------------------------------
  //-------------------------------------------------------------------------------

  // ToDo: ???

  //current_micros = micros();
  //
  //if(current_micros-time_in_1_rud){}
  //if(current_micros-time_in_1_rud){}
  //if(current_micros-time_in_1_rud){}
  //if(current_micros-time_in_1_rud){}
  //


} // END MAIN LOOP











