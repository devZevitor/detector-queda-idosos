#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

const int PINO_BUZZER = 8;
const int PINO_BOTAO  = 2;

const float LIMIAR_PICO     = 1.6;
const float LIMIAR_PARADA   = 1.2;
const unsigned long JANELA_QUEDA_MS = 5000;

const float ANGULO_MIN = 45.0;
const float ANGULO_MAX = 90.0;

float ax, ay, az;
float gx, gy, gz;
float norma;
float pitch = 0;
float anguloZ = 0;

bool picoDetectado  = false;
bool quedaDetectada = false;
bool buzzerLigado   = false;
unsigned long tempoPico = 0;
unsigned long ultimoTempo = 0;
unsigned long ultimoPrint = 0;

void iniciaMPU() {
  if (!mpu.begin()) {
    Serial.println("MPU6050 nao encontrado!");
    while (1) delay(10);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
}

void leMPU() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  ax = a.acceleration.x / SENSORS_GRAVITY_STANDARD;
  ay = a.acceleration.y / SENSORS_GRAVITY_STANDARD;
  az = a.acceleration.z / SENSORS_GRAVITY_STANDARD;
  gx = g.gyro.x * 180.0 / PI;
  gy = g.gyro.y * 180.0 / PI;
  gz = g.gyro.z * 180.0 / PI;
}

float calculaAnguloZ() {
  return atan2(sqrt(ax * ax + ay * ay), az) * 180.0 / PI;
}

bool dentroDaFaixa(float angulo) {
  float a = fabs(angulo);
  return a >= ANGULO_MIN && a <= ANGULO_MAX;
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
  anguloZ = calculaAnguloZ();
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

  anguloZ = 0.9 * anguloZ + 0.1 * calculaAnguloZ();

  bool picoAgora = (norma > LIMIAR_PICO) || (fabs(az) > LIMIAR_PICO);

  if (!quedaDetectada && picoAgora) {
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

  bool anguloConfirma = dentroDaFaixa(pitch) || dentroDaFaixa(anguloZ);

  if (quedaDetectada && !buzzerLigado && anguloConfirma) {
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
    Serial.print(" | Pitch(X): "); Serial.print(pitch, 1);
    Serial.print(" | Ang Z: "); Serial.print(anguloZ, 1);
    Serial.print(" | Queda: "); Serial.print(quedaDetectada);
    Serial.print(" | Buzzer: "); Serial.println(buzzerLigado);
  }

  delay(10);
}
