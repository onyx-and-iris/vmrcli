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
enum kind set_kind(char *kval);
int init_voicemeeter(T_VBVMR_INTERFACE *vmr, int kind);
void interactive(T_VBVMR_INTERFACE *vmr);
void parse_input(T_VBVMR_INTERFACE *vmr, char *input, int len);
void parse_command(T_VBVMR_INTERFACE *vmr, char *command);
void get(T_VBVMR_INTERFACE *vmr, char *command, struct result *res);

bool vflag = false;

int main(int argc, char *argv[])
{
    bool iflag = false;
    int opt;
    char *kvalue = "";
    int dvalue;
    enum kind kind = BANANAX64;

    if (argc == 1)
    {
        help();
        exit(EXIT_SUCCESS);
    }

    log_set_level(LOG_WARN);

    while ((opt = getopt(argc, argv, "k:ihD:v")) != -1)
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
        case 'v':
            vflag = true;
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
            parse_input(vmr, argv[i], strlen(argv[i]));
        }
    }

    rep = logout(vmr);
    if (rep == 0)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}

/**
 * @brief prints the help dialogue
 *
 */
void help()
{
    puts(
        "Usage: ./vmrcli.exe [-h] [-i] [-k] [-D] [-v] <api commands>\n"
        "Where: \n"
        "\th: Prints the help dialogue\n"
        "\ti: Enable interactive mode\n"
        "\tk: The kind of Voicemeeter (basic, banana, potato)\n"
        "\tD: Set log level 0=TRACE, 1=DEBUG, 2=INFO, 3=WARN, 4=ERROR, 5=FATAL\n"
        "\tv: Enable extra console output (toggle, set messages)");
}

/**
 * @brief Set the kind object
 *
 * @param kval
 * @return enum kind
 */
enum kind set_kind(char *kval)
{
    if (strcmp(kval, "basic") == 0)
    {
        if (sizeof(void *) == 8)
            return BASICX64;
        else
            return BASIC;
    }
    else if (strcmp(kval, "banana") == 0)
    {
        if (sizeof(void *) == 8)
            return BANANAX64;
        else
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
    char input[MAX_LINE];
    size_t len;

    printf(">> ");
    while (fgets(input, MAX_LINE, stdin) != NULL)
    {
        input[strcspn(input, "\n")] = 0;
        len = strlen(input);
        if (len == 1 && toupper(input[0]) == 'Q')
            break;

        parse_input(vmr, input, len);

        memset(input, '\0', MAX_LINE); /* reset input buffer */
        printf(">> ");
    }
}

void parse_input(T_VBVMR_INTERFACE *vmr, char *input, int len)
{
    char *token;

    replace_multiple_space_with_one(input, len);
    token = strtok(input, " ");
    while (token != NULL)
    {
        parse_command(vmr, token);
        token = strtok(NULL, " ");
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
            if (res.val.f == 1 || res.val.f == 0)
            {
                set_parameter_float(vmr, command, 1 - res.val.f);
                if (vflag)
                {
                    printf("Toggling %s\n", command);
                }
            }
            else
                log_warn("%s does not appear to be a boolean parameter", command);
        }
        return;
    }

    if (strchr(command, '=') != NULL) /* set */
    {
        set_parameters(vmr, command);
        if (vflag)
        {
            printf("Setting %s\n", command);
        }
    }
    else /* get */
    {
        struct result res = {.type = FLOAT_T};

        get(vmr, command, &res);
        switch (res.type)
        {
        case FLOAT_T:
            printf("%s: %.1f\n", command, res.val.f);
            break;
        case STRING_T:
            printf("%s: %ls\n", command, res.val.s);
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