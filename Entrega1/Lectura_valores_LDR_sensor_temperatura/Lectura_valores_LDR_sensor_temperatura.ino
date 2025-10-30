int valor;
int fi;
#define sbi(port, bit) (port) |= (1 << (bit))
#define cbi(port, bit) (port) &= ~(1 << (bit))
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(57600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(A1, INPUT);
}



// the loop function runs over and over again forever
void loop() {
  fi = 10;
  int boton = 5;
  valor = analogRead(A1);
  Serial.print("El sensor está dando valor: ");
  Serial.print((valor * 5.0 * 100.0)/1024.0);
  Serial.println(" ºC  ... Buscando siguiente valor");
  delay(50);
}
