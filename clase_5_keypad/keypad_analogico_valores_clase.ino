#include



while(true) {
  valor = analogRead(A0);

  if(valor < 1000){
    Serial.print(0);
    Serial.print("    ");
    Serial.print(1050);
    Serial.print("    ");
    Serial.print(valor);
    Serial.println("    ");
    // delay(500)
  }
  if(!(i++%10)) Serial.println()
}
