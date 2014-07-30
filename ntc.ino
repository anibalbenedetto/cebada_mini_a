

const float TPto1 = 0; //Temperatura en ºC punto 1
const float RPto1 = 32960; // Ohms en punto 1
const float TPto2 = 100; // Temp. punto 2
const float RPto2 = 674; // Ohms en punto 2
const float resistor = 10000; //El valor en ohmnios de la resistencia del termistor a 25ºC
const float voltage = 5; // El voltaje real en el punto 5Vcc de tu placa Arduino
const float K= 273.15; //Para pasar a grados Kelvin
const float e = 2.718281828459045; //Constante matemática 
const float B = log(RPto2/RPto1)/(1/(TPto2+K)-(1/(TPto1+K))); //Valor Beta de tu termistor
const float unodivr = 1/(resistor * pow(e,(-B/298.15))); //Con pow elevamos e al resultado

float temp_a = 0;
float temp_b = 0;

void ntc_setup(){
  //nothing to do
}

float getTempa(){
  return temp_a;
}

float ntc_read_temperatures(){
  temp_a = ntc_read_temperature(pin_ntc_a);
  Serial.print("temp_a:");
  Serial.println(temp_a);
  //temp_b = ntc_read_temperature(pin_ntc_b);
}

float ntc_read_temperature(int pin){
  print_ram();
  float tmedia=0;
  int cnt=0, sensorValue=0;
  print_ram();
  for (cnt; cnt<3; cnt++){
    sensorValue = analogRead(pin);
    tmedia=tmedia+sensorValue;
    delay(5);
  }
  tmedia=tmedia/cnt; 
  print_ram();
  float v2 = (voltage*float(tmedia))/1024.0f;
  float r1a = (voltage*float(resistor))/v2;
  float r1 =r1a - resistor;

  float temp = B/log(r1*unodivr);
  temp= temp-273.15;
  print_ram();
  //grados=(int)temp;
  //decimas=(temp-grados)*10;
  return temp;
}
