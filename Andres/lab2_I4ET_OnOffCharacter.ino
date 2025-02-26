byte receivedByte = 0;


void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (Serial.available()>0) // Is there any character coming through the serial port?
  {
    //If there is a character, read it
    receivedByte = Serial.read(); 
    if (receivedByte == 70)               //If sending ON character "F"
    {
      digitalWrite(LED_BUILTIN, LOW);     
    }
    if (receivedByte == 78)               //If sending OFF character "N"
    {
      digitalWrite(LED_BUILTIN, HIGH);    
    }
  }
}
