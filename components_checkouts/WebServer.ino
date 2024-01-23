// --------------------------------------------------
//
// Code setting up a simple webserver on the ESP32.
// device used for tests: ESP32-WROOM-32D
//
// Written by mo thunderz (last update: 27.08.2022)
//
// --------------------------------------------------

#include <WiFi.h>
#include <WebServer.h> // Make sure tools -> board is set to ESP32, otherwise you will get a "WebServer.h: No such file or directory" error
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// SSID and password of Wifi connection:
const char *ssid = "TYPE_YOUR_SSID_HERE";
const char *password = "12345678";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

String webSocketJson = "<!DOCTYPE html><html><head><title> Page Title </title></head><body style='background-color: #EEEEEE;'><span style='color: #003366;'><h1>Random number generator</h1><p> The first random number is: <span id='rand_1'>-</span></p><p> The second random number is: <span id='rand_2'>-</span></p></span><button id='send_btn'> Send Data</button></body><script>var socket;document.getElementById('send_btn').addEventListener('click', sendBack);function init() {socket = new WebSocket('ws://' + window.location.hostname + ':81/');socket.onmessage = function(event) {processCommand(event);};};function processCommand(event){const obj = JSON.parse(event.data);document.getElementById('rand_1').innerHtml = obj.voltage;document.getElementById('rand_2').innerHtml = obj.current;console.log(event.data);};function sendBack(){const obj = {name: 'mark', age: 23};Socket.send(JSON.stringify(obj));};window.onload = function(event){init();};</script></html>";

int randomval = random(100);
int interval = 1000;              // virtual delay
unsigned long previousMillis = 0; // Tracks the time since last event fired

StaticJsonDocument<200> doc_tx;
StaticJsonDocument<200> doc_rx;

void setup()
{
    Serial.begin(115200);

    // Set up the ESP32 as an access point
    WiFi.softAP(ssid, password);

    Serial.println("ESP32 Access Point Running");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", []()
              { server.send(200, "text/html", webSocketJson); });
    server.begin(); // init the server
    webSocket.begin();
}

void loop()
{
    server.handleClient(); // webserver methode that handles all Client
    webSocket.loop();

    unsigned long now = millis(); // call millis  and Get snapshot of time
    if ((unsigned long)(now - previousMillis) > interval)
    { // How much time has passed, accounting for rollover with subtraction!
        String jsonString = "";
        JsonObject object = doc_tx.to<JsonObject>();

        object["voltage"] = random(100);
        object["current"] = random(100);

        serializeJson(doc_tx, jsonString);
        Serial.println(jsonString);

        webSocket.broadcastTXT(jsonString);
        previousMillis = now;
    }
}

void webSocketEvent(byte num, WStype_t type, uint8_t *payload, size_t length_)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.println("Client Disconnected");
        break;
    case WStype_CONNECTED:
        Serial.println("Client Connected");
        break;
    case WStype_TEXT:
        DeserializationError error = deserializeJson(doc_rx, payload);
        if (error)
        {
            Serial.print("deserializeJson() failed");
            return;
        }
        else
        {
            const char *name_ = doc_rx["name"];
            const char *age = doc_rx["age"];

            Serial.println("Name: " + String(name_));
            Serial.println("Age: " + String(age));
        }
        Serial.println("");
        break;
    }
}