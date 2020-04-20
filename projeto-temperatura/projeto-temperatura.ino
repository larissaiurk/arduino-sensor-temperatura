#include <dht.h>

#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


// Digital
int releLampadaPin = 13;
int releVentiladorPin = 10;
int ledAzulPin = 9;
int btnT2Pin = 8;
int btnT1Pin = 7;
int ledVermelhoPin = 6;

// Analogico
dht DHT;
int dht11Pin = A0;
int potPin = 1;

int t1 = 30;
int t2 = 24;

int ledMax = 50;
int ledMin = 20;

// variaveis
int statusA = 0;
int statusAAux = 0;
int statusV = 0;
int statusVAux = 0;

int flag = 0;
float tempAux = 0;

int v = 0;
int a = 0;

float temperatureC = 0;

void setup()
{
  Serial.begin(9600);
  
  lcd.begin(16,2);
  
  pinMode(btnT1Pin, INPUT_PULLUP);
  pinMode(btnT2Pin, INPUT_PULLUP);
  
  pinMode(releLampadaPin, OUTPUT);
  pinMode(releVentiladorPin, OUTPUT);
  
  pinMode(ledAzulPin, OUTPUT);
  pinMode(ledVermelhoPin, OUTPUT);
  
}
 
void loop()
{

  //Pot temperatura
  int valPot = analogRead(potPin);
  valPot = map(valPot, 0, 1023, 10, 40);

  DHT.read11(dht11Pin);
  temperatureC = DHT.temperature;
    
  //lendo botoes temperatura max min
  int btnT1Val = digitalRead(btnT1Pin);
  int btnT2Val = digitalRead(btnT2Pin);
  
  int valRedLed = map(temperatureC, ledMin, ledMax, 255, 0);
  int valAzulLed = map(temperatureC, ledMax, ledMin, 255, 0);

  if(valRedLed > 255) {
    valRedLed = 255;
  } else if (valRedLed < 0) {
    valRedLed = 0;
  }

  if(valAzulLed > 255) {
    valAzulLed = 255;
  } else if (valAzulLed < 0) {
    valAzulLed = 0;
  }
 
  if(temperatureC > ledMax){
    analogWrite(ledVermelhoPin, 0);
    analogWrite(ledAzulPin, 255);
  } else {
    analogWrite(ledVermelhoPin, valRedLed);
  }

  if(temperatureC < ledMin) {
    analogWrite(ledVermelhoPin, 255);
    analogWrite(ledAzulPin, 0);
  } else {
    analogWrite(ledAzulPin, valAzulLed);
  } 
  
  // Lógica Botão
  if (btnT1Val == LOW) {
    if(t2 < valPot) {
      t1 = valPot;  
    }
  }
  
  if (btnT2Val == LOW) {
    if(t1 > valPot) {
      t2 = valPot;
    }    
  }
  
  // Calculando temperatura sensor
  if(temperatureC != tempAux) {
    tempAux = temperatureC;
    flag = 0;
  }
  
  // acionando relé ventilador
  if(temperatureC > t1) {
    digitalWrite(releVentiladorPin, LOW);
    statusV = 1;
    v=1;
  } else {
    digitalWrite(releVentiladorPin, HIGH);
    statusV = 2;
    v=0;
  }
  
  // acionando relé lâmpada
  if(temperatureC < t2) {
    digitalWrite(releLampadaPin, LOW);
    statusA = 1;
    a=1;
  } else {
    digitalWrite(releLampadaPin, HIGH);
    statusA = 2;
    a=0;
  }  

  if(statusV != statusVAux) {
      if(statusV == 1) {
        Serial.println("Ventilador acionado");
      } else {
        Serial.println("Ventilador desligado");
      }
    statusVAux = statusV;
  }  

  if(statusA != statusAAux) {
      if(statusA == 1) {
        Serial.println("Aquecedor acionado");
      } else {
        Serial.println("Aquecedor desligado");
      }
    
    statusAAux = statusA;
  }  

  //LCD
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.setCursor(2,0);
  lcd.print(temperatureC + String("C"));
  lcd.setCursor(9,0);
  lcd.print(String("V-") + String(v) + String(" A-") + String(a));
  
  lcd.setCursor(0,1);
  lcd.print("T1:" + String(t1)); 
  lcd.setCursor(7,1);
  lcd.print("T2:" + String(t2)); 

  lcd.setCursor(14,1);
  lcd.print(valPot);   


  // delay(1000); // Wait for 1000 millisecond(s)
}

void serialEvent() {
  String cmd = Serial.readStringUntil('-');
  String val = Serial.readStringUntil('-');
  String temp = Serial.readStringUntil('-');
  
  if(cmd == "get") {
    if(val == "T") {    
      Serial.println(temperatureC);
    } else if(val == "T1") {
      Serial.println(t1);  
    } else if(val == "T2") {
      Serial.println(t2); 
    } else if(val == "rele"){
      Serial.print("V-" + String(v)); Serial.println(" A-" + String(a));
    }
    Serial.println(String(val));
  }
  
  if(cmd == "set") {
    if(val == "T1"){
      if(temp.toInt() > t2) {
        t1 = temp.toInt();
      } else {
        Serial.println("ERRO: a temperatura T1 deve ser maior que T2");
      }
    }else if(val == "T2"){
      if(temp.toInt() < t1) {
        t2 = temp.toInt();
      } else {
        Serial.println("ERRO: a temperatura T2 deve ser menor que T2");
      }     
    }
  }
}
