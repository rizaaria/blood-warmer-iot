/*************************************************
 * BLOOD WARMER IoT
 * ESP32 + DS18B20 + OLED SSD1306 + Blynk
 *
 * DS18B20      -> GPIO15
 * Heater MOSFET-> GPIO13 (ACTIVE HIGH)
 * Buzzer       -> GPIO26
 * OLED SDA     -> GPIO21
 * OLED SCL     -> GPIO22
 *************************************************/

#define BLYNK_TEMPLATE_ID   ""
#define BLYNK_TEMPLATE_NAME "Blood Warmer"
#define BLYNK_AUTH_TOKEN    "

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===================== WIFI =====================
char ssid[] = "";
char pass[] = "";

// ===================== OLED =====================
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ===================== PIN ======================
#define DS18B20_PIN  15
#define HEATER_PIN   13
#define BUZZER_PIN   26
#define PWM_CHANNEL   0
#define PWM_FREQ   1000
#define PWM_RES       8

// ================== DS18B20 =====================
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

// ================== SETPOINT ====================
const float TEMP_HEATER_ON  = 33.0;
const float TEMP_HEATER_OFF = 34.0;

const float TEMP_LOW_ALARM  = 35.0;
const float TEMP_NORMAL     = 36.0;
const float TEMP_HIGH_ALARM = 42.0;

// ================== PWM HEATER ==================
const uint8_t HEATER_DUTY = 64;  // 25% daya, range 0-255

// ================== VARIABLE ====================
bool heaterState      = false;
bool suhuPernahNormal = false;

bool highTempSent    = false;
bool lowTempSent     = false;
bool sensorErrorSent = false;

unsigned long buzzerStartTime = 0;
bool buzzerActive = false;

float g_suhu1 = 0, g_suhu2 = 0;
bool  g_alarmHigh = false, g_alarmLow = false;

BlynkTimer timer;

// =================================================
// BUZZER NON-BLOCKING
// =================================================
void buzzerTick()
{
  if (!buzzerActive) return;

  unsigned long elapsed = millis() - buzzerStartTime;
  const unsigned long phaseLen = 150;
  int phase = elapsed / phaseLen;

  if (phase >= 4)
  {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerActive = false;
    return;
  }

  digitalWrite(BUZZER_PIN, (phase % 2 == 0) ? HIGH : LOW);
}

void triggerBuzzer()
{
  if (buzzerActive) return;
  buzzerActive    = true;
  buzzerStartTime = millis();
}

// =================================================
// KIRIM KE BLYNK
// =================================================
void timerSendBlynk()
{
  Blynk.virtualWrite(V0, g_suhu2);
  Blynk.virtualWrite(V1, g_suhu1);
  Blynk.virtualWrite(V2, heaterState ? 1 : 0);

  if (g_alarmHigh)
    Blynk.virtualWrite(V3, "HIGH TEMP");
  else if (g_alarmLow)
    Blynk.virtualWrite(V3, "LOW TEMP");
  else
    Blynk.virtualWrite(V3, "NORMAL");
}

// =================================================
// UPDATE OLED
// =================================================
void updateOLED(float suhu1, float suhu2, bool alarmHigh, bool alarmLow, bool error)
{
  display.clearDisplay();

  if (error)
  {
    display.setTextSize(2);
    display.setCursor(10, 8);
    display.println("SENSOR");
    display.setCursor(10, 30);
    display.println("ERROR!");
    display.setTextSize(1);
    display.setCursor(0, 54);
    display.println("DS18B20 NOT FOUND");
    display.display();
    return;
  }

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("== Blood Warmer ==");

  display.setTextSize(2);
  display.setCursor(0, 14);
  display.print("IN:");
  display.print(suhu1, 1);
  display.print("C");

  display.setCursor(0, 36);
  display.print("OT:");
  display.print(suhu2, 1);
  display.print("C");

  display.setTextSize(1);
  display.setCursor(0, 56);
  display.print(heaterState ? "HTR:ON " : "HTR:OFF");

  display.setCursor(72, 56);
  if (alarmHigh)
    display.print("[HIGH]");
  else if (alarmLow)
    display.print("[LOW] ");
  else
    display.print("[OK]  ");

  display.display();
}

// =================================================
// SPLASH SCREEN
// =================================================
void splashScreen()
{
  for (int x = -80; x <= 8; x += 4)
  {
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(x, 18);
    display.print("FAISAL");
    display.display();
    delay(25);
  }
  delay(1000);

  for (int x = 128; x >= 10; x -= 4)
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(15, 10);
    display.println(" ");
    display.setCursor(x, 35);
    display.print("");
    display.display();
    delay(20);
  }
  delay(1500);

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("BLOOD");
  display.setCursor(0, 20);
  display.println("WARMER");
  display.setTextSize(1);
  display.setCursor(0, 50);
  display.println("Temperature Monitor");
  display.display();
  delay(2500);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(20, 15);
  display.println("Initializing...");
  display.drawRect(10, 35, 108, 12, SSD1306_WHITE);
  for (int i = 0; i <= 104; i += 4)
  {
    display.fillRect(12, 37, i, 8, SSD1306_WHITE);
    display.display();
    delay(40);
  }
  delay(500);
  display.clearDisplay();
}

// =================================================
// SETUP
// =================================================
void setup()
{
  Serial.begin(115200);

  pinMode(HEATER_PIN, OUTPUT);
  digitalWrite(HEATER_PIN, LOW);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  sensors.begin();
  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED ERROR");
    while (1);
  }

  splashScreen();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(2000L, timerSendBlynk);
}

// =================================================
// LOOP
// =================================================
void loop()
{
  Blynk.run();
  timer.run();
  buzzerTick();

  sensors.requestTemperatures();
  float suhu1 = sensors.getTempCByIndex(0);
  float suhu2 = sensors.getTempCByIndex(1);

  // ==========================================
  // CEK SENSOR ERROR
  // ==========================================
  bool sensor1Error = (suhu1 == DEVICE_DISCONNECTED_C ||
                       suhu1 < -20.0f || suhu1 > 125.0f);
  bool sensor2Error = (suhu2 == DEVICE_DISCONNECTED_C ||
                       suhu2 < -20.0f || suhu2 > 125.0f);
  bool sensorError  = sensor1Error || sensor2Error;

  if (sensorError)
  {
    digitalWrite(HEATER_PIN, LOW);
    heaterState = false;
    updateOLED(0, 0, false, false, true);
    triggerBuzzer();

    if (!sensorErrorSent)
    {
      Blynk.virtualWrite(V3, "SENSOR ERROR");
      Blynk.logEvent("sensor_error", "DS18B20 disconnected");
      sensorErrorSent = true;
    }
    return;
  }

  sensorErrorSent = false;
  g_suhu1 = suhu1;
  g_suhu2 = suhu2;

  // ==========================================
  // SUHU PERNAH NORMAL
  // FIX: dicek duluan, termasuk kondisi suhu start tinggi
  // ==========================================
  if (suhu1 >= TEMP_NORMAL || suhu1 >= TEMP_HIGH_ALARM)
    suhuPernahNormal = true;

  // ==========================================
  // HEATER CONTROL
  // ==========================================
  if (suhu1 < TEMP_HEATER_ON)
    heaterState = true;

  if (suhu1 > TEMP_HEATER_OFF)
    heaterState = false;

  digitalWrite(HEATER_PIN, heaterState ? HIGH : LOW);

  // ==========================================
  // ALARM — FIX: >= agar 40.0 tepat ikut trigger
  // ==========================================
  bool alarmHigh = (suhu1 >= TEMP_HIGH_ALARM);
  bool alarmLow  = (suhuPernahNormal && suhu1 < TEMP_LOW_ALARM);

  g_alarmHigh = alarmHigh;
  g_alarmLow  = alarmLow;

  if (alarmHigh || alarmLow)
    triggerBuzzer();

  // ==========================================
  // NOTIF HIGH TEMP
  // ==========================================
  if (alarmHigh && !highTempSent)
  {
    Blynk.logEvent("high_temp", "Blood Warmer temperature above 40C");
    highTempSent = true;
  }
  if (suhu1 < 39.5f)
    highTempSent = false;

  // ==========================================
  // NOTIF LOW TEMP
  // ==========================================
  if (alarmLow && !lowTempSent)
  {
    Blynk.logEvent("low_temp", "Blood Warmer temperature below 35C");
    lowTempSent = true;
  }
  if (suhu1 > 35.5f)
    lowTempSent = false;

  // ==========================================
  // OLED
  // ==========================================
  updateOLED(suhu1, suhu2, alarmHigh, alarmLow, false);

  // ==========================================
  // SERIAL DEBUG
  // ==========================================
  // Serial.printf("S1:%.1f S2:%.1f Heater:%s Duty:%d Alarm:%s\n",
  //   suhu1, suhu2,
  //   heaterState ? "ON" : "OFF",
  //   heaterState ? HEATER_DUTY : 0,
  //   alarmHigh ? "HIGH" : alarmLow ? "LOW" : "OK"
  // );
}