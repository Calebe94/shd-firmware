#include <stdio.h>
#include "web_common.h"

static const char * response_html_template = "<!DOCTYPE html>" 
    "<html lang='en'>"
    "<head>"
        "<meta charset='UTF-8'/>"
        "<meta http-equiv='X-UA-Compatible' content='IE=edge,chrome=1'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "<title>%s</title>"
        "<link rel='stylesheet' href='/modest.css'>"
        "<meta http-equiv='refresh' content='2;url=/settings.html'>"
    "</head>"
    "<body>"
    "<section>"
            "<div id='card'>"
                "<div id='upper-side' class='%s'>"
                    "<h3 id='status'>%s</h3>"
                "</div>"
                "<div id='lower-side'>"
                    "<p id='message'>Você será redirecionado automáticamente em 3 segundos.</p>"
                    "<a href='/settings.html' class='%s' id='contBtn'>Retornar</a>"
                "</div>"
            "</div>"
        "</section>"
    "</body>"
"</html>";

void web_create_success_response(char * response, char *title, char *message)
{
    snprintf(response, 1024, response_html_template, title, "success", message, "success");
}

void web_create_failure_response(char * response, char *title, char *message)
{
    snprintf(response, 1024, response_html_template, title, "failed", message, "failed");
}
