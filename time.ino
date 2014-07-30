static byte timeServer[] = { 132, 163, 4, 101 };
const int timeZone = -3;     // Central European Time
time_t prevDisplay = 0; // when the digital clock was displayed
const int minutes_lapse = 30;
time_t last_post = 0;

void time_setup(){
  Serial.println("waiting for sync");
  setTime(getNtpTime());
  //setSyncProvider(getNtpTime);  
}

byte time_loop(){
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();  
    }
  }
  if((now()-last_post)> minutes_lapse){
    last_post = now();
    return 1;
  }
  return 0;
}


uint8_t ntpServer[4] = {  132, 163, 4, 101  }; //http://www.pool.ntp.org/zone/ar -> ar.pool.ntp.org
uint8_t ntpMyPort = 123; 
time_t getNtpTime() {   
  unsigned long timeFromNTP;   
  const unsigned long seventy_years = 2208988800UL;      
   Serial.println("getNtpTime");
  ether.ntpRequest(ntpServer, ntpMyPort);   
  while(true) {       
    word length = ether.packetReceive();       
    ether.packetLoop(length);       
    if(length > 0 && ether.ntpProcessAnswer(&timeFromNTP, ntpMyPort)) {
      Serial.print("Time from NTP: ");
      Serial.println(timeFromNTP);
      return timeFromNTP - seventy_years + timeZone * SECS_PER_HOUR;
    }
  }
  return 0;
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}
