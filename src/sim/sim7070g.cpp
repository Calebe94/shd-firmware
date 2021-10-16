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
    ESP_LOGD(TAG, "\nStarting Up Modem...");
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, HIGH);
    delay(300);
    digitalWrite(PWR_PIN, LOW);
    delay(10000);
    int i = 10;
    ESP_LOGD(TAG, "\nTesting Modem Response...\n");
    ESP_LOGD(TAG, "****");
    while (i)
    {
        SerialAT.print("AT\r");
        delay(500);
        if (SerialAT.available())
        {
            String r = SerialAT.readString();
            ESP_LOGD(TAG, "%s", r);
            if ( r.indexOf("OK") >= 0 )
            {
                reply = true;
                break;;
            }
        }
        delay(500);
        i--;
    }
    ESP_LOGD(TAG, "****\n");
    return reply;
}

size_t sim7070g_read(char *data)
{
    uint32_t size = 0;
    if(SerialAT.available() && data != NULL)
    {
        String sim7070g_line = SerialAT.readString();
        const char *sim7070g_data = sim7070g_line.c_str();
        memcpy(data, sim7070g_data, SIM7070G_MAX_LENGTH);
        size = strlen(sim7070g_data);
    }
    return size;
}

void sim7070g_flush(void)
{
    SerialAT.flush();
}

bool sim7070g_send_sms(const char *number, const char *message)
{
    char number_command[25] = "";
    bool status = false;

    if(number != NULL && message != NULL)
    {
        snprintf(number_command, 25, "AT+CMGS=\"%s\"", number);
        ESP_LOGD(TAG, "%s", SerialAT.readString());
        SerialAT.println("AT+CMGF=1");
        delay(500);
        ESP_LOGD(TAG, "%s", SerialAT.readString());
        SerialAT.println(number_command);
        delay(500);
        SerialAT.print(message);
        SerialAT.write(0x1A);
        delay(500);
        while(SerialAT.available())
        {
            ESP_LOGD(TAG, "%s", SerialAT.readString());
        }
        status = true;
    }

    return status;
}
