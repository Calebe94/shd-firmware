/***************************
 * INCLUDES
****************************/
#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include "sim/sim7070g.h"
#include "sensor/flowsensor.h"
#include "settings/settings.h"
#include "esp_log.h"
#include "esp32-hal-log.h"

#define uS_TO_S_FACTOR          1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP           60          /* Time ESP32 will go to sleep (in seconds) */

void setup()
{
    // Set console baud rate
    SerialMon.begin(115200);
    delay(10);

    if(!SPIFFS.begin(false))
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    flowsensor_init();
    sim7070g_init();

    Serial.println("Wait...");

    int retry = 5;
    bool reply= false;
    while (!(reply = sim7070g_turn_on()) && retry--);

    if (reply)
    {
        Serial.println(F("***********************************************************"));
        Serial.println(F(" You can now send AT commands"));
        Serial.println(F(" Enter \"AT\" (without quotes), and you should see \"OK\""));
        Serial.println(F(" If it doesn't work, select \"Both NL & CR\" in Serial Monitor"));
        Serial.println(F(" DISCLAIMER: Entering AT commands without knowing what they do"));
        Serial.println(F(" can have undesired consiquinces..."));
        Serial.println(F("***********************************************************\n"));
    }
    else
    {
        Serial.println(F("***********************************************************"));
        Serial.println(F(" Failed to connect to the modem! Check the baud and try again."));
        Serial.println(F("***********************************************************\n"));
    }
}

void loop()
{
    while (SerialAT.available())
    {
        Serial.write(SerialAT.read());
    }

    while (Serial.available())
    {
        SerialAT.write(Serial.read());
    }

    /*
    SerialAT.println("AT");

    delay(500);

    Serial.println(SerialAT.readString());

    SerialAT.println("AT+CMGF=1");

    delay(500);

    Serial.println(SerialAT.readString());

    SerialAT.println("AT+CMGS=\"41998271302\"");

    delay(500);

    //Serial.println(SerialAT.readString());

    SerialAT.print("TESTE");

    SerialAT.write(0x1A);

    delay(500);

    Serial.println(SerialAT.readString());

    delay(60*1000);
*/
}


