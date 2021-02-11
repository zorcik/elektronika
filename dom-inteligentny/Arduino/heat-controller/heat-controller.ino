//https://www.stm32duino.com/viewtopic.php?t=590
//https://github.com/stm32duino/wiki/wiki/API#i2C

void setup() {

  Serial1.setRx(PB7);
  Serial1.setTx(PB6);
  Serial1.begin(9600);

  

}

void loop() {
  // put your main code here, to run repeatedly:

}
