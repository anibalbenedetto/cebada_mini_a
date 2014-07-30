

int temp_max;
int temp_min;

void thermostat_setup(){
  pinMode(pin_out_cold, OUTPUT);
  pinMode(pin_out_heat, OUTPUT);
  
  digitalWrite(pin_out_cold, LOW);
  digitalWrite(pin_out_heat, LOW);
}

void thermostat_loop(int temp){
  //todo: esperar x minutos para comenzar a operar
  if( temp > temp_max ){
    //need cold
    digitalWrite(pin_out_heat, LOW);
    digitalWrite(pin_out_cold, HIGH);
  }
  else if(temp < temp_min){
    //need hot
    digitalWrite(pin_out_cold, LOW);
    digitalWrite(pin_out_heat, HIGH);
  }
}
