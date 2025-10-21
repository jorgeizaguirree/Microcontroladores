int v1 = 0;
int v2 = 132;
int v3 = 310;
int v4 = 481;
int v5 = 720;
int v6 = 1023;
int valor;
int fi;
#define sbi(port,bit)   (port) |= (1 << (bit))
#define cbi(port,bit)   (port) &= ~(1 << (bit))
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(57600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(A1, INPUT);
}
int media(int a, int b){
  return (a + b)/2;
}

int encontrar_tecla(int v){
  if (v < media(v1,v2)){
    return 0;
  } 
  if (v < media(v2,v3)){
    return 1;
  }
  if (v < media(v3,v4)){
    return 2;
  }
  if (v < media(v4,v5)){
    return 3;
  }
  if (v < media(v5,v6)){
    return 4;
  }
  return 5;
}

// the loop function runs over and over again forever
void loop() {
  valor = v6;
  fi = 10;
  int boton = 5;
  while ((valor = analogRead(A1)) > media(v5,v6)){
  }
  if (valor < media(v5,v6)){
    boton = encontrar_tecla(valor);
    while(fi > 0){
      delay(1);
      if (boton == encontrar_tecla(analogRead(A1))){
        fi--;
      }else {
        fi = -1;
      }
    }
    if (fi == 0){
      Serial.print("Se ha pulsado la tecla ");
      Serial.print(boton);
      Serial.print(" (");
      Serial.print(valor);
      Serial.println(")  ... Buscando siguiente tecla");
    }  
  }     // wait for a second
  
}
