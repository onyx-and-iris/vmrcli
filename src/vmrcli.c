#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include "cdll.h"
#include "vmr.h"
#include "log.h"
#include "util.h"

#define MAX_LINE 512

enum
{
    FLOAT_T,
    STRING_T,
};

struct result
{
    int type;
    union val
    {
        float f;
        wchar_t s[MAX_LINE];
    } val;
};

void help(void);
int set_kind(char *kval);
int init_voicemeeter(T_VBVMR_INTERFACE *vmr, int kind);
void interactive(T_VBVMR_INTERFACE *vmr);
void parse_command(T_VBVMR_INTERFACE *vmr, char *command);
void get(T_VBVMR_INTERFACE *vmr, char *command, struct result *res);

int main(int argc, char *argv[])
{
    bool iflag = false;
    int opt;
    char *kvalue = "";
    int dvalue;
    int kind = BANANA;

    if (argc == 1)
    {
        help();
        exit(EXIT_SUCCESS);
    }

    log_set_level(LOG_WARN);

    while ((opt = getopt(argc, argv, "k:ihD:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            iflag = true;
            break;
        case 'k':
            kvalue = optarg;
            kind = set_kind(kvalue);
            break;
        case 'h':
            help();
            exit(EXIT_SUCCESS);
        case 'D':
            dvalue = atoi(optarg);
            if (dvalue >= LOG_TRACE && dvalue <= LOG_FATAL)
            {
                log_set_level(dvalue);
            }
            else
            {
                log_error(
                    "-D arg out of range, expected value from 0 up to 5\n"
                    "Log level will default to LOG_WARN (3).\n");
            }
            break;
        default:
            abort();
        }
    }

    T_VBVMR_INTERFACE iVMR;
    T_VBVMR_INTERFACE *vmr = &iVMR;

    int rep = init_voicemeeter(vmr, kind);
    if (rep != 0)
    {
        exit(EXIT_FAILURE);
    }

    if (iflag)
    {
        puts("Interactive mode enabled. Enter 'Q' to exit.");
        interactive(vmr);
    }
    else
    {
        for (int i = optind; i < argc; i++)
        {
            parse_command(vmr, argv[i]);
        }
    }

    rep = logout(vmr);
    if (rep == 0)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}

void help()
{
    puts(
        "Usage: ./vmrcli.exe [-i] [-k] [-D] <api commands>\n"
        "Where: \n"
        "\ti: Enable interactive mode\n"
        "\tk: The kind of Voicemeeter (basic, banana, potato)\n"
        "\tD: Set log level 0=TRACE, 1=DEBUG, 2=INFO, 3=WARN, 4=ERROR, 5=FATAL");
}

int set_kind(char *kval)
{
    if (strcmp(kval, "basic") == 0)
    {
        return BASIC;
    }
    else if (strcmp(kval, "banana") == 0)
    {
        return BANANA;
    }
    else if (strcmp(kval, "potato") == 0)
    {
        if (sizeof(void *) == 8)
            return POTATOX64;
        else
            return POTATO;
    }
    else
    {
        log_error("Unknown Voicemeeter kind '%s'\n", kval);
        exit(EXIT_FAILURE);
    }
}

int init_voicemeeter(T_VBVMR_INTERFACE *vmr, int kind)
{
    int rep = initialize_dll_interfaces(vmr);
    if (rep < 0)
    {
        if (rep == -100)
        {
            log_error("Voicemeeter is not installed");
        }
        else
        {
            log_error("Error loading Voicemeeter dll with code %d\n", rep);
        }
        return rep;
    }

    rep = login(vmr, kind);
    if (rep != 0)
    {
        log_error("Error logging into Voicemeeter");
        return rep;
    }

    return 0;
}

void interactive(T_VBVMR_INTERFACE *vmr)
{
    char input[MAX_LINE], command[MAX_LINE];
    char *p = input;
    int i;
    size_t len;

    printf(">> ");
    while (fgets(input, MAX_LINE, stdin) != NULL)
    {
        input[strcspn(input, "\n")] = 0;
        len = strlen(input);
        if (len == 1 && toupper(input[0]) == 'Q')
            break;

        replace_multiple_space_with_one(input, len);
        while (*p)
        {
            if (isspace(*p))
            {
                p++;
                continue;
            }
            log_trace("commands still in buffer: %s", p);

            i = 0;
            while (*p && !isspace(*p))
                command[i++] = *p++;
            command[i] = '\0';
            p++; /* shift to next char */

            if (command[0] != '\0')
                parse_command(vmr, command);
            memset(command, '\0', MAX_LINE);
        }

        p = input;                     /* reset pointer */
        memset(input, '\0', MAX_LINE); /* reset input buffer */
        printf(">> ");
    }
}

void parse_command(T_VBVMR_INTERFACE *vmr, char *command)
{
    log_debug("Parsing %s", command);

    if (command[0] == '!') /* toggle */
    {
        command++;
        struct result res = {.type = FLOAT_T};

        get(vmr, command, &res);
        if (res.type == FLOAT_T)
        {
            set_parameter_float(vmr, command, 1 - res.val.f);
        }
        return;
    }

    if (strchr(command, '=') != NULL) /* set */
    {
        set_parameters(vmr, command);
    }
    else /* get */
    {
        struct result res = {.type = FLOAT_T};

        get(vmr, command, &res);
        switch (res.type)
        {
        case FLOAT_T:
            printf("%.1f\n", res.val.f);
            break;
        case STRING_T:
            printf("%ls\n", res.val.s);
            break;
        default:
            break;
        }
    }
}

void get(T_VBVMR_INTERFACE *vmr, char *command, struct result *res)
{
    clear_dirty(vmr);
    if (get_parameter_float(vmr, command, &res->val.f) != 0)
    {
        res->type = STRING_T;
        if (get_parameter_string(vmr, command, res->val.s) != 0)
        {
            res->val.s[0] = 0;
            log_error("Unknown parameter '%s'", command);
        }
    }
}