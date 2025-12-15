#include "TRSensors.h"


// KONFIGURACJA SENSORÓW
#define NUM_SENSORS 5
TRSensors trs = TRSensors();
unsigned int sensorValues[NUM_SENSORS];

// Pinout silników
#define L_IN1 A0
#define L_IN2 A1
#define R_IN1 A2
#define R_IN2 A3
#define L_PWM 5
#define R_PWM 6


bool is_active = false;  // START/STOP

int Vref = 35;           
float Kp = 0.1;          
float Ki = 0.0;            
float Kd = 0.8;          

float d_alpha = 0.5; 

//Zmienne do pidu
float integral = 0;
float prev_error = 0;
float d_filtered = 0;
const int I_limit = 2000; 

//Do komunikacji
String inputString = "";         
boolean stringComplete = false;  

//Czas

unsigned long last_pid_time = 0;
const int Ts = 50;    //50 ms           
const float Ts_sec = Ts / 1000.0;  //50 ms -> sekundy

void setMotorPWM(int left, int right);

void setup() {
  Serial.begin(9600);
  inputString.reserve(200); 

  pinMode(L_IN1, OUTPUT); pinMode(L_IN2, OUTPUT);
  pinMode(R_IN1, OUTPUT); pinMode(R_IN2, OUTPUT);
  pinMode(L_PWM, OUTPUT); pinMode(R_PWM, OUTPUT);

  // Domyślnie kierunek przód, ale silniki STOP
  digitalWrite(L_IN1, LOW); digitalWrite(L_IN2, HIGH);
  digitalWrite(R_IN1, HIGH); digitalWrite(R_IN2, LOW);
  analogWrite(L_PWM, 0);
  analogWrite(R_PWM, 0);

  Serial.println("System gotowy. Polacz sie i wyslij 'K' aby skalibrowac.");
}

void setMotorPWM(int left, int right) {
  int max_speed_limit = 163; 

  if (left > max_speed_limit) left = max_speed_limit;
  if (left < 0) left = 0;

  if (right > max_speed_limit) right = max_speed_limit;
  if (right < 0) right = 0;

  analogWrite(L_PWM, left);
  analogWrite(R_PWM, right);
}

// Funkcja parsująca komendy
void processCommand() {
  inputString.trim(); 
  
  if (inputString.length() > 0) {
    // START / STOP 'P'
    if (inputString.equalsIgnoreCase("P")) {
      is_active = !is_active; 
      if (is_active) Serial.println("START: Jazda po linii");
      else {
        Serial.println("STOP: Zatrzymanie");
        setMotorPWM(0, 0); 
        integral = 0;      
        prev_error = 0;
      }
    }
    // KALIBRACJA 'K'
    else if (inputString.equalsIgnoreCase("K")) {
        is_active = false;
        setMotorPWM(0, 0);
        
        Serial.println("KALIBRACJA START: Ruszaj robotem nad linia przez 5s!");
        
        unsigned long calStart = millis();
        while (millis() - calStart < 5000) {
           trs.calibrate();
        }
        
        Serial.println("KALIBRACJA KONIEC. Mozesz wpisac 'P' aby jechac.");
        
        //pokaż wyniki
        for (int i = 0; i < NUM_SENSORS; i++) {
            Serial.print(trs.calibratedMin[i]); Serial.print(" ");
            Serial.println(trs.calibratedMax[i]);
        }
        
    }
    // Ustawienia pidu
    else {
        // Logika dla komend z wartościami
        char cmd = inputString.charAt(0);
        if (inputString.length() > 2) {
            float value = inputString.substring(2).toFloat();
            
            if (inputString.startsWith("Kp")) { Kp = value; Serial.print("Kp: "); Serial.println(Kp); }
            else if (inputString.startsWith("Ki")) { Ki = value; Serial.print("Ki: "); Serial.println(Ki); }
            else if (inputString.startsWith("Kd")) { Kd = value; Serial.print("Kd: "); Serial.println(Kd); }
            else if (inputString.startsWith("Vr")) { Vref = (int)value; Serial.print("Vref: "); Serial.println(Vref); }
        }
    }
  }
  // Wyczyść bufor
  inputString = "";
  stringComplete = false;
}

void loop() {
  // 1. Odbiór danych
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n' || inChar == '\r') {
      stringComplete = true; 
    } else {
      inputString += inChar;
    }
  }

  if (stringComplete) {
    processCommand();
  }

  // 2. PID LOOP
  unsigned long currentMillis = millis();
  
  if (is_active && (currentMillis - last_pid_time >= Ts)) {
    last_pid_time = currentMillis;

    int position = trs.readLine(sensorValues);
    int error = position - 2000; 

    float P = Kp * error;

    integral += error * Ts_sec;
    if (integral > I_limit) integral = I_limit; //anti windup
    if (integral < -I_limit) integral = -I_limit; //anti windup
    float I = Ki * integral;

    float d_raw = (error - prev_error) / Ts_sec;
    d_filtered = d_alpha * d_raw + (1.0 - d_alpha) * d_filtered;
    float D = Kd * d_filtered;

    prev_error = error;
    float u = P + I + D;

    int speedL = Vref - u;
    int speedR = Vref + u;

    setMotorPWM(speedL, speedR);
  } 
}