#include <Wire.h>


const int PINO_BUZZER = 8;
const int PINO_BOTAO  = 2;


const uint8_t MPU_ADDR = 0x68;
const float ESCALA_ACC  = 4096.0; 
const float ESCALA_GIRO = 65.5;    


const float LIMIAR_PICO     = 1.6; 
const float LIMIAR_PARADA   = 1.2;  
const unsigned long JANELA_QUEDA_MS = 5000;
const float LIMIAR_PITCH    = 45.0; 

float ax, ay, az;       
float gx, gy, gz;       
float norma;             
float pitch = 0;        


bool picoDetectado  = false;
bool quedaDetectada = false;   
bool buzzerLigado   = false;
unsigned long tempoPico = 0;
unsigned long ultimoTempo = 0;
unsigned long ultimoPrint = 0;

void escreveRegistrador(uint8_t reg, uint8_t valor) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(valor);
  Wire.endTransmission();
}

void iniciaMPU() {
  escreveRegistrador(0x6B, 0x00);
  escreveRegistrador(0x1C, 0x10);
  escreveRegistrador(0x1B, 0x08);
}

void leMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);               
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, (uint8_t)14);

  int16_t rawAx = (Wire.read() << 8) | Wire.read();
  int16_t rawAy = (Wire.read() << 8) | Wire.read();
  int16_t rawAz = (Wire.read() << 8) | Wire.read();
  Wire.read(); Wire.read();      
  int16_t rawGx = (Wire.read() << 8) | Wire.read();
  int16_t rawGy = (Wire.read() << 8) | Wire.read();
  int16_t rawGz = (Wire.read() << 8) | Wire.read();

  ax = rawAx / ESCALA_ACC;
  ay = rawAy / ESCALA_ACC;
  az = rawAz / ESCALA_ACC;
  gx = rawGx / ESCALA_GIRO;
  gy = rawGy / ESCALA_GIRO;
  gz = rawGz / ESCALA_GIRO;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  iniciaMPU();

  pinMode(PINO_BUZZER, OUTPUT);
  pinMode(PINO_BOTAO, INPUT_PULLUP);
  noTone(PINO_BUZZER);

  leMPU();
  pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
  ultimoTempo = millis();

  Serial.println("Detector de queda iniciado.");
}

void loop() {
  unsigned long agora = millis();
  float dt = (agora - ultimoTempo) / 1000.0;
  ultimoTempo = agora;

  
  leMPU();

  
  norma = sqrt(ax * ax + ay * ay + az * az);


  float pitchAcc = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
  pitch = 0.96 * (pitch + gy * dt) + 0.04 * pitchAcc;


  if (!quedaDetectada && norma > LIMIAR_PICO) {
    if (!picoDetectado) Serial.println(">> PICO detectado!");
    picoDetectado = true;
    tempoPico = agora;          
  }

  if (picoDetectado && !quedaDetectada) {
    if (norma < LIMIAR_PARADA) {
      quedaDetectada = true;
      picoDetectado = false;
      Serial.println(">> O idoso caiu");
    } else if (agora - tempoPico > JANELA_QUEDA_MS) {
      picoDetectado = false;    
    }
  }

  
  if (quedaDetectada && !buzzerLigado && fabs(pitch) > LIMIAR_PITCH) {
    buzzerLigado = true;
    tone(PINO_BUZZER, 1000);
    Serial.println(">> Buzzer ligado.");
  }

  if (buzzerLigado && digitalRead(PINO_BOTAO) == LOW) {
    delay(30);                  
    if (digitalRead(PINO_BOTAO) == LOW) {
      buzzerLigado = false;
      quedaDetectada = false;
      picoDetectado = false;
      noTone(PINO_BUZZER);
      Serial.println(">> Buzzer desligado pelo botao.");
      while (digitalRead(PINO_BOTAO) == LOW) delay(10); 
    }
  }

  if (agora - ultimoPrint > 250) {
    ultimoPrint = agora;
    Serial.print("Acc(g): ");
    Serial.print(ax, 2); Serial.print(", ");
    Serial.print(ay, 2); Serial.print(", ");
    Serial.print(az, 2);
    Serial.print(" | Gyro(s): ");
    Serial.print(gx, 1); Serial.print(", ");
    Serial.print(gy, 1); Serial.print(", ");
    Serial.print(gz, 1);
    Serial.print(" | Norma: "); Serial.print(norma, 2);
    Serial.print(" | Pitch: "); Serial.print(pitch, 1);
    Serial.print(" | Queda: "); Serial.print(quedaDetectada);
    Serial.print(" | Buzzer: "); Serial.println(buzzerLigado);
  }

  delay(10);
}