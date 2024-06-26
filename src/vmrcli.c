#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include "cdll.h"
#include "vmr.h"
#include "log.h"

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
        char s[MAX_LINE];
    } val;
};

void help(void);
int set_kind(char *kval);
int init_voicemeeter(T_VBVMR_INTERFACE *vmr, int kind);
void interactive(T_VBVMR_INTERFACE *vmr);
void parse_command(T_VBVMR_INTERFACE *vmr, char *command);
struct result *get(T_VBVMR_INTERFACE *vmr, char *command, struct result *res);

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
        return EXIT_SUCCESS;
    }

    log_set_level(LOG_INFO);

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
            if ((dvalue = atoi(optarg)) && dvalue >= LOG_TRACE && dvalue <= LOG_FATAL)
            {
                log_set_level(dvalue);
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
        "Usage: ./vmrcli.exe [-i] [-k] <api commands>\n"
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
        fprintf(stderr, "Unknown Voicemeeter kind '%s'\n", kval);
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
            fputs("Voicemeeter is not installed", stderr);
        }
        else
        {
            fprintf(stderr, "Error loading Voicemeeter dll with code %d\n", rep);
        }
        return rep;
    }

    rep = login(vmr, kind);
    if (rep != 0)
    {
        fputs("Error logging into Voicemeeter", stderr);
        return rep;
    }

    return 0;
}

void interactive(T_VBVMR_INTERFACE *vmr)
{
    char input[MAX_LINE];
    char *p = input;

    while (fgets(input, MAX_LINE, stdin) != NULL)
    {
        input[strcspn(input, "\n")] = 0;
        if (strlen(input) == 1 && (strncmp(input, "Q", 1) == 0 || strncmp(input, "q", 1) == 0))
            break;

        while (*p)
        {
            char command[MAX_LINE];
            int i = 0;

            while (!isspace(*p) && *p != EOF)
                command[i++] = *p++;
            command[i] = '\0';
            p++; /* shift to next char */

            parse_command(vmr, command);
        }

        p = input; /* reset pointer */
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
            printf("%.2f\n", res.val.f);
            break;
        case STRING_T:
            puts(res.val.s);
            break;
        default:
            fputs("Unknown result...", stderr);
            break;
        }
    }
}

struct result *get(T_VBVMR_INTERFACE *vmr, char *command, struct result *res)
{
    clear_dirty(vmr);
    if (get_parameter_float(vmr, command, &res->val.f) != 0)
    {
        res->type = STRING_T;
        if (get_parameter_string(vmr, command, res->val.s) != 0)
        {
            res->val.s[0] = 0;
            fputs("Unknown parameter", stderr);
        }
    }

    return res;
}