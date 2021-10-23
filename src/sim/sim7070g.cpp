#include <Ticker.h>
#define TINY_GSM_MODEM_SIM7070
#include <TinyGsmClient.h>

#include "cmd/commands_handler.h"
#include "sim7070g_utils.h"
#include "sim7070g.h"

Ticker tick;
TinyGsm modem(SerialAT);
TaskHandle_t sim7070g_task_handle;
QueueHandle_t responses_queue;
QueueHandle_t sms_queue;

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
        xTaskCreate(sim7070g_event_handler_task, "sim7070g_event_handler_task", 4096, NULL, 5, &sim7070g_task_handle);
        responses_queue = xQueueCreate(10, sizeof(char)*SIM7070G_MAX_RESPONSE);
        xTaskCreate(sim7070g_responses_parser_task, "sim7070g_responses_parser_task", 4096, NULL, 5, NULL);
        xTaskCreate(sim7070g_send_sms_task_handler, "sim7070g_send_sms_task_handler", 4096, NULL, 5, NULL);
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

    sim7070g_set_cmgf_to_text();
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
        sms_t sms;
        strncpy(sms.phone, number, 15);
        strncpy(sms.message, message, SMS_MAX_CHAR);

        if(xQueueSend(sms_queue, (void *)&sms, (TickType_t) 10 ) == pdPASS )
        {
            status = true;
        }
    }
    return status;
}

static bool _sim7070g_send_sms(const char *number, const char *message)
{
    bool status = false;
    char number_cmd[35] = "";
    String res = "";

    if(number != NULL && message != NULL)
    {
        ESP_LOGD(__func__, "Enviando SMS: %s - %s", number, message);
        modem.sendAT(GF("+CMGF=1"));
        if (modem.waitResponse(1000L, res))
        {
            res.trim();
            modem.sendAT(GF("+CSCS=\"GSM\""));
            modem.waitResponse();
            res = "";
            sprintf(number_cmd, "+CMGS=\"%s\"", number);
            modem.sendAT(GF(number_cmd));

            int new_line_index = 0;
            if(modem.waitResponse(GF(">")))
            {
                for(int index = 0; index < strlen(message); index++)
                {
                    modem.stream.write(static_cast<char>(message[index]));
                    new_line_index++;
                    if(new_line_index == 30)
                    {
                        modem.stream.write(static_cast<char>('\n'));
                        new_line_index = 0;
                    }
                }
                //modem.stream.print(message);  // Actually send the message
                modem.stream.write(static_cast<char>(0x1A));  // Terminate the message
                modem.stream.flush();
                status = modem.waitResponse(60000L) == 1;
            }
        }
    }

    return status;
}

bool sim7070g_set_cmgf_to_text()
{
    bool status = false;
    String res = "";
    modem.sendAT(GF("+CMGF=1"));
    if (modem.waitResponse(1000L, res) != 1)
    {
        ESP_LOGE(TAG, "AT+CMGF=1 Não recebeu nada como resposta!");
        status = false;
    }
    else
    {
        res.trim();
        ESP_LOGD(TAG, "Resposta(AT+CMGF=1): %s", res.c_str());
        status = true;
    }
    return status;
}

bool sim7070g_clear_sms_list()
{
    String res;
    bool status = false;
    sim7070g_set_cmgf_to_text();

    modem.sendAT(GF("+CMGD=1,4"));
    res = "";
    if (modem.waitResponse(1000L, res) != 1)
    {
        ESP_LOGE(TAG, "AT+CMGD=1,4 Não recebeu nada como resposta!");
        status = false;
    }
    else
    {
        res.trim();
        ESP_LOGD(TAG, "Resposta(AT+CMGD=1,4): %s", res.c_str());
        status = true;
    }
    return status;
}

String sim7070g_list_all_sms()
{
    String res = "";
    modem.sendAT(GF("+CMGL=\"ALL\""));
    if (modem.waitResponse(1000L, res) != 1)
    {
        ESP_LOGE(TAG, "AT+CMGL=\"ALL\" Não recebeu nada como resposta!");
    }
    else
    {
        res.trim();
        ESP_LOGD(TAG, "Resposta(AT+CMGL=\"ALL\"): %s", res.c_str());
    }

    return res;
}

String sim7070g_read_sms_by_id(int slot)
{
    String res = "";
    char cmgr_command[11];
    snprintf(cmgr_command, 11, "+CMGR=%d", slot);
    modem.sendAT(GF(cmgr_command));
    if (modem.waitResponse(1000L, res) != 1)
    {
        ESP_LOGE(TAG, "AT%s Não recebeu nada como resposta!", cmgr_command);
    }
    else
    {
        res.trim();
        ESP_LOGD(TAG, "Resposta(AT%s): %s", cmgr_command, res.c_str());
    }

    return res;
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
    vTaskSuspend(NULL);
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
            //sim7070g_parse_responses(response);
            sim7070g_send_response_to_parser(response.c_str());
            response = "";
        }
        delay(50);
    }
}

bool sim7070g_send_response_to_parser(const char *response)
{
    bool status = false;
    if(response != NULL)
    {
        if(xQueueSend(responses_queue, (void *)response, (TickType_t) 10 ) == pdPASS )
        {
            status = true;
        }
    }
    return status;
}

void sim7070g_responses_parser_task(void *argv)
{
    ESP_LOGD(TAG, "Iniciando a tarefa de analise das respostas do SIM7070G...");
    for(;;)
    {
        char response_buffer[SIM7070G_MAX_RESPONSE];
        if(xQueueReceive(responses_queue, response_buffer, portMAX_DELAY))
        {
            if(strstr(response_buffer, "OK") != NULL)
            {
                ESP_LOGD(TAG, "Recebido OK");
            }
            else if(strstr(response_buffer, "ERROR") != NULL)
            {
                ESP_LOGD(TAG, "Recebido ERROR");
            }
            else if(strstr(response_buffer, "+CMTI:") != NULL)
            {
                ESP_LOGD(TAG, "Recebido um novo SMS");
                int id = sim7070g_from_cmti_get_id(response_buffer);
                if(id != -1)
                {
                    ESP_LOGD(TAG, "ID from CMTI: %d", id);
                    String sms = sim7070g_read_sms_by_id(id);
                    sms.replace("\r\nOK", "");
                    ESP_LOGD(TAG, "SMS: %s", sms.c_str());
                    String command = sim7070g_from_cmgr_get_message(sms.c_str());
                    ESP_LOGD(TAG, "SMS command: %s", command.c_str());
                    String phone = sim7070g_from_cmgr_get_phone(sms.c_str());
                    ESP_LOGD(TAG, "Phone number: %s", phone.c_str());
                    sms_command_t sms_command;
                    strncpy(sms_command.command, command.c_str(), CMD_MAX_BUF_SIZE);
                    strncpy(sms_command.phone, phone.c_str(), 15);
                    send_command_to_parser(sms_command);
                }
                ESP_LOGD(TAG, "Limpando a lista de SMS");
                sim7070g_clear_sms_list();
            }
            else if(strstr(response_buffer, "+CMGR:") != NULL)
            {
                ESP_LOGD(TAG, "Leitura de SMS");
            }
            else
            {
                ESP_LOGD(TAG, "Recebido: %s - não fazer nada!", response_buffer);
            }
        }
    }
}

void sim7070g_send_sms_task_handler(void *argv)
{
    ESP_LOGD(TAG, "Iniciando a tarefa de envio de SMS do SIM7070G...");
    sms_queue = xQueueCreate(20, sizeof(sms_t));
    for(;;)
    {
        sms_t sms;
        if(xQueueReceive(sms_queue, &sms, portMAX_DELAY))
        {
            if(_sim7070g_send_sms(sms.phone, sms.message))
            {
                ESP_LOGD(__func__, "Mensagem (%s) enviada para (%s) com sucesso!", sms.message, sms.phone);
            }
            else
            {
                ESP_LOGD(__func__, "Falha ao enviar (%s) para (%s)!", sms.message, sms.phone);
            }
        }
    }
}
