#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Declaremos los pines CE y el CSN
#define CE_PIN 9
#define CSN_PIN 10
 
//Variable con la dirección del canal por donde se va a transmitir
byte direccion[1] = {'h'};

//creamos el objeto radio (NRF24L01)
RF24 radio(CE_PIN, CSN_PIN);

//vector con los datos a enviar
int datos[3];

void setup()
{

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  //inicializamos el NRF24L01 
  radio.begin();
  //inicializamos el puerto serie
  Serial.begin(9600); //cambiar para usar leds1 a 115200 pero a 2000000 si es posta
 
//Abrimos un canal de escritura
 radio.openWritingPipe(direccion);
 
}
 
void loop()
{ 
 //cargamos los datos en la variable datos[]
 datos[0]=analogRead(0);
 datos[1]= analogRead(1);
 datos[2]= 9999;

Serial.print(analogRead(0));
Serial.print(" a y b ");
Serial.print(analogRead(1));
 //enviamos los datos
 bool ok = radio.write(datos, sizeof(datos));

  //reportamos por el puerto serial los datos enviados 
  if(ok)
  {
     Serial.print("serv "); 
     Serial.print(datos[0]); 
     Serial.print(" speed: "); 
     Serial.print(datos[1]); 
     Serial.print(" comprobado: "); 
     Serial.println(datos[2]); 
  }
  else
  {
     Serial.println("no se ha podido enviar");
  }
  delay(50);
}
