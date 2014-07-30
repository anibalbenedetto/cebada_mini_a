/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(2,3,4,1);

/* we always wait a bit between updates of the display */
unsigned long delaytime=250;

void display_setup() {
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);
}


/*
 This method will display the characters for the
 word "Arduino" one after the other on digit 0. 
 */
void writeArduinoOn7Segment() {
  lc.setChar(0,0,'a',false);
  delay(delaytime);
  lc.setRow(0,0,0x05);
  delay(delaytime);
  lc.setChar(0,0,'d',false);
  delay(delaytime);
  lc.setRow(0,0,0x1c);
  delay(delaytime);
  lc.setRow(0,0,B00010000);
  delay(delaytime);
  lc.setRow(0,0,0x15);
  delay(delaytime);
  lc.setRow(0,0,0x1D);
  delay(delaytime);
  lc.clearDisplay(0);
  delay(delaytime);
} 

/*
  This method will scroll all the hexa-decimal
 numbers and letters on the display. You will need at least
 four 7-Segment digits. otherwise it won't really look that good.
 */
void scrollDigits() {
  for(int i=0;i<13;i++) {
    lc.setChar(0,0,'a',false);
  }
  lc.clearDisplay(0);
  delay(delaytime);
}

void display_loop() { 
  //Serial.println("Updating display");
  print_ram();
  lc.clearDisplay(0);
  
  float temp = getTempa();
  
  byte grados=(int)temp;
  
  byte digOne = 0, digTwo = 0, digThree = 0, digFour = 0;
 	
  digOne = grados/10;
  digTwo = (grados - (digOne*10));
  digThree = (byte)((temp-grados)*10);
  digFour = (byte)((temp-grados)*100);
  digFour = digFour - (digThree*10); 
 
  lc.setDigit(0,0,digOne,false);
  lc.setDigit(0,1,digTwo,true);
  lc.setDigit(0,2,digThree,false);
  lc.setDigit(0,3,digFour,false);
  print_ram();
}
