/* First working Arduino Zero Sketch
 *  
 */

void setup() {
  pinMode(13,OUTPUT);
}

void loop() {
  digitalWrite(13,!digitalRead(13));
  delay(500);
}
