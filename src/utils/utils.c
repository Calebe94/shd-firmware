#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "utils.h"

void str_remove_junk_char(char *string) 
{
    char *t = string;
    while (*string) 
    {
        if (*string == '\n') 
        {
            // When a string has a new line character, remove and place a null char.
            *string='\0';
        }
        else if(*string == '+')
        {
            // When a plus char was found, remove and place a space.
            // HTML Form replace spaces with + char.
            *string=' ';
        }
        string++;
    }
}

char *str_replace(char *search , char *replace , char *subject)
{
	char  *p = NULL , *old = NULL , *new_subject = NULL ;
	int c = 0 , search_size;
	
	search_size = strlen(search);
	
	//Count how many occurences
	for(p = strstr(subject , search) ; p != NULL ; p = strstr(p + search_size , search))
	{
		c++;
	}
	
	//Final size
	c = ( strlen(replace) - search_size )*c + strlen(subject);
	
	//New subject with new size
	new_subject = malloc( c );
	
	//Set it to blank
	strcpy(new_subject , "");
	
	//The start position
	old = subject;
	
	for(p = strstr(subject , search) ; p != NULL ; p = strstr(p + search_size , search))
	{
		//move ahead and copy some text from original subject , from a certain position
		strncpy(new_subject + strlen(new_subject) , old , p - old);
		
		//move ahead and copy the replacement text
		strcpy(new_subject + strlen(new_subject) , replace);
		
		//The new start position after this search match
		old = p + search_size;
	}
	
	//Copy the part after the last search match
	strcpy(new_subject + strlen(new_subject) , old);
	
	return new_subject;
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

/* Function: urlDecode */
char *url_decode(const char *str) 
{
    int d = 0; /* whether or not the string is decoded */

    char *dStr = malloc(strlen(str) + 1);
    char eStr[] = "00"; /* for a hex code */

    strcpy(dStr, str);

    while(!d)
    {
        d = 1;
        int i; /* the counter for the string */

        for(i=0;i<strlen(dStr);++i)
        {
            if(dStr[i] == '%')
            {
                if(dStr[i+1] == 0)
                return dStr;

                if(isxdigit(dStr[i+1]) && isxdigit(dStr[i+2]))
                {
                    d = 0;

                    /* combine the next to numbers into one */
                    eStr[0] = dStr[i+1];
                    eStr[1] = dStr[i+2];

                    /* convert it to decimal */
                    long int x = strtol(eStr, NULL, 16);

                    /* remove the hex */
                    memmove(&dStr[i+1], &dStr[i+3], strlen(&dStr[i+3])+1);

                    dStr[i] = x;
                }
            }
        }
    }

    return dStr;
}
