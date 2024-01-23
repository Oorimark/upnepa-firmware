#include <LiquidCrystal_I2C.h>

// set LCD address, number of columns and rows
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
    // initialize LCD
    lcd.init();

    // turn on LCD backlight
    lcd.backlight();
}

void loop()
{
    // set cursor to first column, first row
    lcd.setCursor(0, 0);

    // print message
    lcd.print("IP: 192.163.43.128");

    char buf[10];

    // get amplitude (maximum - or peak value)
    uint32_t v = get_max();

    // get actual voltage (ADC voltage reference = 1.1V)
    v = v * 1100 / 1023;

    // calculate the RMS value ( = peak/√2 )
    v /= sqrt(2);

    sprintf(buf, "%03u Volts", v);
    lcd.setCursor(0, 1);
    lcd.print(buf);

    delay(100);
}

// get maximum reading value
uint16_t get_max()
{
    uint16_t max_v = 0;
    for (uint8_t i = 0; i < 100; i++)
    {
        uint16_t r = analogRead(A0); // read from analog channel 3 (A3)
        if (max_v < r)
            max_v = r;
        delayMicroseconds(200);
    }
    return max_v;
}
