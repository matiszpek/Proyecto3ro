#include <Servo.h>


Servo servo;

void setup() {
  pinMode(A8, INPUT);
  pinMode(A9, INPUT);
  pinMode(A10, INPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(A3,INPUT);
  Serial.begin(9600);

servo.attach(9);

}

void loop() {
  int y = map(analogRead(A8), 0, 1023, 10, 215);
  int x = map(analogRead(A9), 0, 1023, 10, 215);
  int t = map(x,112.5,0,y,0);
  int c = map(x,112.5,225,y,0);

  int aux = analogRead(A10);

  int a=12;
  int b = 13;


  servo.write(aux);
  
  if (digitalRead(7) == LOW && digitalRead(A3) == LOW) {  // Adelante
    if(x<=146&&x>=109){//Recto
    analogWrite(a,y);
    digitalWrite(38,HIGH);
    digitalWrite(39,LOW);
      
    analogWrite(b,y);
    digitalWrite(42,HIGH);
    digitalWrite(43,LOW);
    Serial.print("recto:");
    }
    else if(x>146){//Derecha
    analogWrite(a,c);
    digitalWrite(38,HIGH);
    digitalWrite(39,LOW);
      
    analogWrite(b,y);
    digitalWrite(42,HIGH);
    digitalWrite(43,LOW);
    Serial.print("derecha:");
    }
    else if(x<109){//izquierda
    analogWrite(a,y);
    digitalWrite(38,HIGH);
    digitalWrite(39,LOW);
      
    analogWrite(b,t);
    digitalWrite(42,HIGH);
    digitalWrite(43,LOW);
    Serial.print("izquierda:");
    }
  Serial.print(y);
  Serial.print(":");
  Serial.print(x);
  Serial.print(":");
  Serial.print(c);
  Serial.print(":");
  Serial.print(t);
  Serial.println(":");
  }
  else if (digitalRead(7) == HIGH && digitalRead(A3) == LOW) {//Reversa
  if(x<=146&&x>=109){//Recto
    analogWrite(a,y);
    digitalWrite(38,LOW);
    digitalWrite(39,HIGH);
      
    analogWrite(b,y);
    digitalWrite(42,LOW);
    digitalWrite(43,HIGH);
    Serial.print("recto:");
    }
    else if(x>145){//Derecha
    analogWrite(a,c);
    digitalWrite(38,LOW);
    digitalWrite(39,HIGH);
      
    analogWrite(b,y);
    digitalWrite(42,LOW);
    digitalWrite(43,HIGH);
    Serial.print("derecha:");
    }
    else if(x<109){//izquierda
    analogWrite(a,y);
    digitalWrite(38,LOW);
    digitalWrite(39,HIGH);
      
    analogWrite(b,t);
    digitalWrite(42,LOW);
    digitalWrite(43,HIGH);
    Serial.print("izquierda:");
    }
  }
  else if(digitalRead(7) == HIGH && digitalRead(A3) == HIGH){
    analogWrite(a,y);
    digitalWrite(38,LOW);
    digitalWrite(39,HIGH);
      
    analogWrite(b,y);
    digitalWrite(42,LOW);
    digitalWrite(43,HIGH);
    Serial.print("recto:");
  }
  else{
    analogWrite(a,y);
    digitalWrite(38,HIGH);
    digitalWrite(39,LOW);
      
    analogWrite(b,y);
    digitalWrite(42,HIGH);
    digitalWrite(43,LOW);
  }
  
}

