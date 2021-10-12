#include "sim7070g.h"

void sim7070g_init()
{
    // Onboard LED light, it can be used freely
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    // POWER_PIN : This pin controls the power supply of the SIM7600
    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, HIGH);

    // PWR_PIN ： This Pin is the PWR-KEY of the SIM7600
    // The time of active low level impulse of PWRKEY pin to power on module , type 500 ms
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, HIGH);
    delay(500);
    digitalWrite(PWR_PIN, LOW);
    delay(1000);
    SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
}

bool sim7070g_turn_on()
{
    bool reply = false;
    // Set-up modem  power pin
    Serial.println("\nStarting Up Modem...");
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, HIGH);
    delay(300);
    digitalWrite(PWR_PIN, LOW);
    delay(10000);
    int i = 10;
    Serial.println("\nTesting Modem Response...\n");
    Serial.println("****");
    while (i)
    {
        SerialAT.print("AT\r");
        delay(500);
        if (SerialAT.available())
        {
            String r = SerialAT.readString();
            Serial.println(r);
            if ( r.indexOf("OK") >= 0 )
            {
                reply = true;
                break;;
            }
        }
        delay(500);
        i--;
    }
    Serial.println("****\n");
    return reply;
}
