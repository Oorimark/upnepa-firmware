
/**
 * This program shows you how to use the basics of this library.
 */
#include <SPI.h> // include libraries
#include <ZMPT101B.h>

#define SENSITIVITY 500.0f

// ZMPT101B sensor output connected to analog pin A0
// and the voltage source frequency is 50 Hz.
ZMPT101B voltageSensor(21, 50.0);

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting program...");
    // Change the sensitivity value based on value you got from the calibrate
    // example.
    voltageSensor.setSensitivity(SENSITIVITY);
    delay(2000);
}

void loop()
{
    // read the voltage and then print via Serial
    float voltage = voltageSensor.getRmsVoltage();
    Serial.println("Voltage: " + String(voltage));
    delay(1000);
}
