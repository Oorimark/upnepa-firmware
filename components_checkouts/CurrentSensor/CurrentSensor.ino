/*ACS712 Current Sensor Demo
   By miliohm.com
*/

#include "ACS712.h"

#define POWER_INDICATOR 18
#define CURRENT_SENSOR_PIN 5
#define ESP_VOLT 5

uint16_t maxADC = 1023;
float mVperAmpere = 100;     

// ACS712_20A for 20 Amp type
// ACS712_30A for 30 Amp type

ACS712 sensor = ACS712(CURRENT_SENSOR_PIN, ESP_VOLT, maxADC, mVperAmpere);

void setup()
{
    pinMode(POWER_INDICATOR, OUTPUT);
    Serial.begin(115200);
    Serial.println("Running...");
}

void loop()
{
    digitalWrite(POWER_INDICATOR, HIGH);
    float I = sensor.mA_AC_sampling(50, 1); // freq = 50Hz, cycle = 1

    // ignoring the value below 0.09
    if (I < 0.09)
    {
        I = 0;
    }
    Serial.println(I);
    delay(300);
    digitalWrite(POWER_INDICATOR, LOW);
    delay(500);
}
