#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


// Digital
int releLampadaPin = 13;
int releVentiladorPin = 10;

int btnT2Pin = 8;
int btnT1Pin = 7;

int ledVermelho = 6;
int ledAzul = 9;

// Analogico
int sensorTemp = 0; 
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
  
  pinMode(ledAzul, OUTPUT);
  pinMode(ledVermelho, OUTPUT);  
}
 
void loop()
{
  
  //Pot temperatura
  int valPot = analogRead(potPin);
  valPot = map(valPot, 0, 1023, 10, 40);
  
  //lendo botoes temperatura max min
  int btnT1Val = digitalRead(btnT1Pin);
  int btnT2Val = digitalRead(btnT2Pin);  
  
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
  
  int reading = analogRead(sensorTemp); 
  float voltage = reading * 5.0;
  voltage /= 1024.0; 
  temperatureC = (voltage - 0.5) * 100 ;
  //Serial.print(temperatureC); Serial.println(" degrees C"); 
  
  // acionando relé ventilador
  if(temperatureC > t1) {
    digitalWrite(releVentiladorPin, HIGH);
    statusV = 1;
    v=1;
  } else if(temperatureC < t1) {
    digitalWrite(releVentiladorPin, LOW);
    statusV = 2;
    v=0;
  }
  
  // acionando relé lâmpada
  if(temperatureC < t2) {
    digitalWrite(releLampadaPin, HIGH);
    statusA = 1;
    a=1;
  } else {
    digitalWrite(releLampadaPin, LOW);
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
  lcd.print("T1>:" + String(t1)); 
  lcd.setCursor(7,1);
  lcd.print("T2<:" + String(t2)); 

  lcd.setCursor(14,1);
  lcd.print(valPot);    
  
  delay(1000);
 
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