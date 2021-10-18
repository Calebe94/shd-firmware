#include <Ticker.h>
#define TINY_GSM_MODEM_SIM7070
#include <TinyGsmClient.h>

#include "sim7070g.h"

Ticker tick;
TinyGsm modem(SerialAT);
TaskHandle_t sim7070g_task_handle;

static void sim7070g_parse_responses(String response)
{
    if(strstr(response.c_str(), "OK") != NULL)
    {
        ESP_LOGD(TAG, "Recebido OK");
    }
    else if(strstr(response.c_str(), "ERROR") != NULL)
    {
        ESP_LOGD(TAG, "Recebido ERROR");
    }
    else if(strstr(response.c_str(), "+CMTI:") != NULL)
    {
        ESP_LOGD(TAG, "Recebido um novo SMS");
        sim7070g_clear_sms_list();
        ESP_LOGD(TAG, "Limpando a lista de SMS");
    }
    else if(strstr(response.c_str(), "+CMGR:") != NULL)
    {
        ESP_LOGD(TAG, "Leitura de SMS");
    }
    else
    {
        ESP_LOGD(TAG, "Não fazer nada!");
    }
}

void sim7070g_init()
{
    pinMode(SIM7070G_INT, INPUT);

    attachInterrupt(SIM7070G_INT, []() {
        detachInterrupt(SIM7070G_INT);
        // If Modem starts normally, then set the onboard LED to flash once every 1 second
        tick.attach_ms(1000, []() {
            digitalWrite(SIM7070G_LED, !digitalRead(SIM7070G_LED));
        });
    }, CHANGE);
    // Onboard LED light, it can be used freely
    pinMode(SIM7070G_LED, OUTPUT);
    digitalWrite(SIM7070G_LED, LOW);
    // SIM7070G_POWER : This pin controls the power supply of the SIM7600
    pinMode(SIM7070G_POWER, OUTPUT);
    digitalWrite(SIM7070G_POWER, HIGH);

    // SIM7070G_PWR ： This Pin is the PWR-KEY of the SIM7600
    // The time of active low level impulse of PWRKEY pin to power on module , type 500 ms
    pinMode(SIM7070G_PWR, OUTPUT);
    digitalWrite(SIM7070G_PWR, HIGH);
    delay(500);
    digitalWrite(SIM7070G_PWR, LOW);
    delay(1000);
    SerialAT.begin(SIM7070G_BAUD, SERIAL_8N1, SIM7070G_RXD, SIM7070G_TXD);

    ESP_LOGD(TAG, "Initializing modem...");

    bool reply = false;
    int retry = 5;
    while (!(reply = sim7070g_turn_on()) && retry--);
    if (!modem.init())
    {
        ESP_LOGD(TAG, "Failed to restart modem, delaying 10s and retrying");
        return;
    }
#ifdef USE_SMS_COMMANDS
    if(reply)
    {
        xTaskCreate(sim7070g_event_handler_task, "sim7070g_event_handler_task", 8192, NULL, 1, &sim7070g_task_handle);
    }
#endif

#ifdef DEBUG
    ESP_LOGI(TAG, "***********************************************************");
    if(reply)
    {
        ESP_LOGI(TAG, " You can now send AT commands");
    }
    else
    {
        ESP_LOGD(TAG, " Failed to connect to the modem! Check the baud and try again.");
    }
    ESP_LOGI(TAG, "***********************************************************\n");
#endif
}

bool sim7070g_turn_on()
{
    bool reply = false;
    // Set-up modem  power pin
    ESP_LOGD(TAG, "\nStarting Up Modem...");
    pinMode(SIM7070G_PWR, OUTPUT);
    digitalWrite(SIM7070G_PWR, HIGH);
    delay(300);
    digitalWrite(SIM7070G_PWR, LOW);
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
                break;
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
    bool status = false;

    if(number != NULL && message != NULL)
    {
        ESP_LOGD(TAG, "Enviando SMS: %s - %s", number, message);
        modem.sendSMS(number, message);
    }

    return status;
}

void sim7070g_clear_sms_list()
{
    SerialAT.println("AT+CMGF=1");
    delay(500);
    SerialAT.println("AT+CMGD=4,1");
    delay(500);
}

String sim7070g_read_response()
{
    String response = "";
    if(SerialAT.available() > 0)
    {
        while(SerialAT.available() > 0)
        {
            response += (char)SerialAT.read();
        }
        ESP_LOGD(TAG, "Recebido: %s - %d", response.c_str(), response.length());
    }

    return response;
}

void sim7070g_suspend_event_handler()
{
    vTaskSuspend(sim7070g_task_handle);
}

void sim7070g_resume_event_handler()
{
    vTaskResume(sim7070g_task_handle);
}

void sim7070g_event_handler_task(void *argv)
{
    ESP_LOGD(TAG, "Iniciando event handler.");
    String response = "";
    for(;;)
    {
        if(SerialAT.available() > 0)
        {
            while(SerialAT.available() > 0)
            {
                char response_char = (char)SerialAT.read();
                Serial.write(response_char);
                response += response_char;
            }
        }

        if(response.endsWith("\r\n") && response.length() > 2)
        {
            response.trim();
            ESP_LOGD(TAG, "Recebido: %s - %d", response.c_str(), response.length());
            sim7070g_parse_responses(response);
            response = "";
        }
    }
}
