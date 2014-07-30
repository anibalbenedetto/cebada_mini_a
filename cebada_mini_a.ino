#include <EtherCard.h>
#include <Time.h>
#include <LedControl.h>

const int pin_ntc_a = A0;
const int pin_ntc_b = A1;
const int pin_out_cold = 2;
const int pin_out_heat = 3;
const int pin_ethernet_cs = 10;

int frec_ntc_loop = 1;

unsigned long time;

void setup() {
  
  time=millis();
  
  Serial.begin(9600);
  print_ram();
  Serial.println("\n[webClient]");
  
  //display_setup();
  //ntc_setup();
  print_ram();
  ethernet_setup();
  print_ram();
  Serial.print("free memory:");
  Serial.println(freeRam());
  //time_setup();
  thermostat_setup();
}


void loop() { 
  
  
  //if(time_loop()){
  //  ethernet_post_data();
  //}
  if ( millis() - time >5000){
    ntc_read_temperatures();
       
    
    //display_loop();
    
    time=millis();
  }
  ethernet_loop();
}

void print_ram(){
  Serial.print("free memory:");
  Serial.println(freeRam()); 
}

int freeRam () {  
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*
$D = word data type
$L = long data type
$S = c string
$F = progmem string
$E = byte from the eeprom.
*/
