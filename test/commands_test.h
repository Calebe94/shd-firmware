#ifndef _COMMANDS_TEST_H_
#define _COMMANDS_TEST_H_
#include <argtable2.h>

#include "minunit.h"

static char *program_name = "commands_test";
static char *help_command_string[] = {
    program_name,
    "--help"
};

int test_commands()
{
    struct arg_lit  *help    = arg_lit0(NULL,"help",                    "print this help and exit");
    struct arg_end  *end     = arg_end(20);
    void* argtable[] = {help, end};
    const char* progname = "test";
    int nerrors;
    int exitcode=0;

    /* verify the argtable[] entries were allocated sucessfully */
    if (arg_nullcheck(argtable) != 0)
    {
        /* NULL entries were detected, some allocations must have failed */
        printf("%s: insufficient memory\n",progname);
        exitcode=1;
    }
    nerrors = arg_parse(2, help_command_string, argtable);
    /* special case: '--help' takes precedence over error reporting */
    if (help->count > 0)
    {
        printf("Usage: %s", progname);
        arg_print_syntax(stdout,argtable,"\n");
        printf("This program demonstrates the use of the argtable2 library\n");
        printf("for parsing command line arguments. Argtable accepts integers\n");
        printf("in decimal (123), hexadecimal (0xff), octal (0o123) and binary\n");
        printf("(0b101101) formats. Suffixes KB, MB and GB are also accepted.\n");
        arg_print_glossary(stdout,argtable,"  %-25s %s\n");
        exitcode=0;
    }

    
    /* If the parser returned any errors then display them and exit */
    if (nerrors > 0)
    {
        /* Display the error details contained in the arg_end struct.*/
        arg_print_errors(stdout,end,progname);
        printf("Try '%s --help' for more information.\n",progname);
        exitcode=1;
    }
    
    return 0;
}
#endif
