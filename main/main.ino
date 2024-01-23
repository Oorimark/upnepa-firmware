// ----------------------------------------------------
// Development of Electrical Power Monitoring System
// devices used: ESP32-WROOM-32D
//
// Written by Mark Oori (created on: 04.01.2023)
//
// ---------------------------------------------------

#include <SPI.h>
#include <WiFi.h>
#include <Wire.h>
#include <ACS712.h>
#include <ZMPT101B.h>
#include <ArduinoJson.h>
#include <WebSocketsServer.h>
#include <LiquidCrystal_I2C.h>

#define SENSITIVITY 500.0f
#define ESP_VOLT 5
#define CURRENT_SENSOR_PIN 5
#define VOLTAGE_SENSOR_PIN 15
#define POWER_INDICATOR 18
#define WIFI_CONNECTED_INDICATOR 19

StaticJsonDocument<200> doc_tx;
StaticJsonDocument<200> doc_rx;
WebSocketsServer webSocket = WebSocketsServer(81);

// SSID and password of Wifi connection:
const char *ssid = "TYPE_YOUR_SSID_HERE";
const char *password = "12345678";

uint16_t maxADC = 1023;
float mVperAmpere = 100;       // 5A = 185; 20A = 100; 30A = 66
int broadcast_interval = 2000; // interval of broadcasting data
unsigned long previousMillis = 0;

LiquidCrystal_I2C lcd(0x27, 20, 4);               // set LCD address, number of columns and rows
ZMPT101B voltageSensor(VOLTAGE_SENSOR_PIN, 50.0); // frequency is 50 Hz.
ACS712 sensor = ACS712(CURRENT_SENSOR_PIN, ESP_VOLT, maxADC, mVperAmpere);

void setup()
{
    Serial.begin(115200);

    lcd.init();        // initialize LCD
    lcd.backlight();   // turn on LCD backlight
    webSocket.begin(); // starts websocket

    pinMode(POWER_INDICATOR, OUTPUT);
    pinMode(WIFI_CONNECTED_INDICATOR, OUTPUT);

    WiFi.softAP(ssid, password); // Set up the ESP32 as an access point
    voltageSensor.setSensitivity(SENSITIVITY);

    lcd.setCursor(0, 0);
    lcd.print("IP Address: ");
    lcd.setCursor(1, 0);
    lcd.print(WiFi.softAPIP());

    Serial.println("ESP32 Access Point Running");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
}

void loop()
{
    webSocket.loop();

    unsigned long now = millis();                                   // call millis  and Get snapshot of time
    if ((unsigned long)(now - previousMillis) > broadcast_interval) // How much time has passed, accounting for rollover with subtraction!
    {
        String jsonString = "";
        JsonObject object = doc_tx.to<JsonObject>();

        float voltage = voltageSensor.getRmsVoltage();
        float current = sensor.mA_AC_sampling(50, 1); // freq = 50Hz, cycle = 1

        object["voltage"] = voltage;
        object["current"] = current;

        serializeJson(doc_tx, jsonString);
        webSocket.broadcastTXT(jsonString);
        previousMillis = now;

        Serial.println(jsonString);

        lcd.setCursor(0, 0);
        lcd.print("Voltage: " + String(voltage) + 'V');
        lcd.setCursor(1, 0);
        lcd.print("Current: " + String(current) + 'A');
        lcd.setCursor(2, 0);
        lcd.print("Power: " + String(current * voltage) + 'W');

        if (voltage)
            digitalWrite(POWER_INDICATOR, HIGH);
        else
            digitalWrite(POWER_INDICATOR, LOW);
    }
}

void webSocketEvent(byte num, WStype_t type, uint8_t *payload, size_t length_)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        digitalWrite(WIFI_CONNECTED_INDICATOR, LOW);
        Serial.println("Client Disconnected");
        break;
    case WStype_CONNECTED:
        digitalWrite(WIFI_CONNECTED_INDICATOR, HIGH);
        Serial.println("Client Connected");
        break;
    }
}
