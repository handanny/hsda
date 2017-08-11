char data0 = 0;
char data1 = 0;
String dataValue0 = "";
String dataValue1 = "";

void setup(){
  Serial.begin(9600);
}

void loop(){
  while(Serial.available())
  {
    data0 = Serial.parseInt();
    data1 = Serial.parseInt();
    Serial.print(" Gas : ");
    Serial.print(data0);
    Serial.print("   Alcohol : ");
    Serial.print(data1);
    Serial.print('\n');
    delay(500);
  }
}
