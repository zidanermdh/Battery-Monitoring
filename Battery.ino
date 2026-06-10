#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6hauWMpiX"
#define BLYNK_TEMPLATE_NAME "Battery Monitoring"
#define BLYNK_AUTH_TOKEN "Ty0nxivwFIEUE6QtsaegZHvc2yJgenAJ"

#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>
#include <PZEM017v1.h>
#include <OneWire.h>
#include <DallasTemperature.h>

char ssid[] = "zii";
char pass[] = "12345678";

#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
#define ONE_WIRE_BUS 4

String GAS_URL = "https://script.google.com/macros/s/AKfycbz7QAKvAwVyv3i8YAup0j9B5yNb0yfTs8PbyJeXkZwXYo4Ung6uDQmwYZy2TlMMpobL/exec";

PZEM017v1 pzem(&Serial2);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
BlynkTimer timer;

void sendData()
{
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();

  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  if (!isnan(voltage)) Blynk.virtualWrite(V0, voltage);
  if (!isnan(current)) Blynk.virtualWrite(V1, current);
  if (!isnan(power)) Blynk.virtualWrite(V2, power);
  if (!isnan(energy)) Blynk.virtualWrite(V3, energy);

  if (temperature != DEVICE_DISCONNECTED_C)
  {
    Blynk.virtualWrite(V5, temperature);
  }

  Serial.print("Voltage : ");
  Serial.print(voltage);
  Serial.println(" V");

  Serial.print("Current : ");
  Serial.print(current);
  Serial.println(" A");

  Serial.print("Power   : ");
  Serial.print(power);
  Serial.println(" W");

  Serial.print("Energy  : ");
  Serial.print(energy);
  Serial.println(" kWh");

  Serial.print("Temp    : ");
  Serial.print(temperature);
  Serial.println(" C");

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    String url = GAS_URL +
                 "?voltage=" + String(voltage, 2) +
                 "&current=" + String(current, 2) +
                 "&power=" + String(power, 2) +
                 "&energy=" + String(energy, 4) +
                 "&temperature=" + String(temperature, 2);

    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.begin(url);

    int httpCode = http.GET();

    Serial.print("HTTP Code : ");
    Serial.println(httpCode);

    String payload = http.getString();

    Serial.print("Response  : ");
    Serial.println(payload);

    http.end();
  }

  Serial.println("-------------------------");
}

void setup()
{
  Serial.begin(115200);

  sensors.begin();

  Serial2.begin(9600, SERIAL_8N1, PZEM_RX_PIN, PZEM_TX_PIN);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(10000L, sendData);
}

void loop()
{
  Blynk.run();
  timer.run();
}
