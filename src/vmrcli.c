/**
 * @file vmrcli.c
 * @author Onyx and Iris (code@onyxandiris.online)
 * @brief A Voicemeeter Remote Command Line Interface
 * @version 0.10.0
 * @date 2024-07-06
 *
 * @copyright Copyright (c) 2024
 * https://github.com/onyx-and-iris/vmrcli/blob/main/LICENSE
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <windows.h>
#include "interface.h"
#include "wrapper.h"
#include "log.h"
#include "util.h"

#define USAGE "Usage: .\\vmrcli.exe [-h] [-i|-I] [-k] [-D] [-v] [-c] [-m] [-s] <api commands>\n" \
              "Where: \n"                                                                        \
              "\th: Prints the help message\n"                                                   \
              "\ti: Enable interactive mode, use (-I) to disable the '>>' prompt\n"              \
              "\tk: The kind of Voicemeeter (basic, banana, potato)\n"                           \
              "\tD: Set log level 0=TRACE, 1=DEBUG, 2=INFO, 3=WARN, 4=ERROR, 5=FATAL\n"          \
              "\tv: Enable extra console output (toggle, set messages)\n"                        \
              "\tc: Load a user configuration (give the full file path)\n"                       \
              "\tm: Launch the MacroButtons application\n"                                       \
              "\ts: Launch the StreamerView application"
#define OPTSTR ":hk:msc:iID:v"
#define MAX_LINE 4096 /* Size of the input buffer */
#define RES_SZ 512    /* Size of the buffer passed to VBVMR_GetParameterStringW */
#define COUNT_OF(x) (sizeof(x) / sizeof(x[0]))
#define DELIMITERS " \t;,"

/**
 * @enum The kind of values a get call may return.
 */
enum restype : int
{
    FLOAT_T,
    STRING_T,
};

/**
 * @struct A struct used for:
 * - tracking the type of value stored
 * - storing the result of a get call
 */
struct result
{
    enum restype type;
    union val
    {
        float f;
        wchar_t s[RES_SZ];
    } val;
};

static bool vflag = false;

static void terminate(PT_VMR vmr, char *msg);
static void usage();
static enum kind set_kind(char *kval);
static void interactive(PT_VMR vmr, bool with_prompt);
static void parse_input(PT_VMR vmr, char *input);
static void parse_command(PT_VMR vmr, char *command);
static void get(PT_VMR vmr, char *command, struct result *res);

int main(int argc, char *argv[])
{
    bool iflag = false,
         mflag = false,
         sflag = false,
         cflag = false,
         with_prompt = true;
    int opt;
    int dvalue;
    char *cvalue;
    enum kind kind = BANANAX64;

    if (argc == 1)
    {
        usage();
    }

    log_set_level(LOG_WARN);

    opterr = 0;
    while ((opt = getopt(argc, argv, OPTSTR)) != -1)
    {
        switch (opt)
        {
        case 'k':
            kind = set_kind(optarg);
            if (kind == UNKNOWN)
            {
                log_fatal("Unknown Voicemeeter kind '%s'", optarg);
                exit(EXIT_FAILURE);
            }
            break;
        case 'm':
            mflag = true;
            break;
        case 's':
            sflag = true;
            break;
        case 'c':
            cflag = true;
            cvalue = optarg;
            break;
        case 'I':
            with_prompt = false;
            [[fallthrough]];
        case 'i':
            iflag = true;
            break;
        case 'D':
            dvalue = atoi(optarg);
            if (dvalue >= LOG_TRACE && dvalue <= LOG_FATAL)
            {
                log_set_level(dvalue);
            }
            else
            {
                log_warn(
                    "-D arg out of range, expected value from 0 up to 5\n"
                    "Log level will default to LOG_WARN (3).\n");
            }
            break;
        case 'v':
            vflag = true;
            break;
        case '?':
            log_fatal("unknown option -- '%c'\n"
                      "Try .\\vmrcli.exe -h for more information.",
                      optopt);
            exit(EXIT_FAILURE);
        case ':':
            log_fatal("missing argument for option -- '%c'\n"
                      "Try .\\vmrcli.exe -h for more information.",
                      optopt);
            exit(EXIT_FAILURE);
        case 'h':
            [[fallthrough]];
        default:
            usage();
        }
    }

    PT_VMR vmr = create_interface();
    if (vmr == NULL)
    {
        exit(EXIT_FAILURE);
    }

    long rep = login(vmr, kind);
    if (rep != 0)
    {
        if (rep == -2)
            terminate(vmr, "Timeout logging into the API.");
        else
            terminate(vmr, "Error logging into the Voicemeeter API");
    }

    if (mflag)
    {
        run_voicemeeter(vmr, MACROBUTTONS);
        log_info("MacroButtons app launched");
    }

    if (sflag)
    {
        run_voicemeeter(vmr, STREAMERVIEW);
        log_info("StreamerView app launched");
    }

    if (cflag)
    {
        set_parameter_string(vmr, "command.load", cvalue);
        log_info("Profile %s loaded", cvalue);
        Sleep(300);
        clear(vmr, is_pdirty);
    }

    if (iflag)
    {
        puts("Interactive mode enabled. Enter 'Q' to exit.");
        interactive(vmr, with_prompt);
    }
    else
    {
        for (int i = optind; i < argc; i++)
        {
            parse_input(vmr, argv[i]);
        }
    }

    rep = logout(vmr);
    if (rep != 0)
    {
        terminate(vmr, "Error logging out of the Voicemeeter API");
    }

    log_info("Successfully logged out of the Voicemeeter API");
    free(vmr);
    return EXIT_SUCCESS;
}

/**
 * @brief Write fatal error log, free dyn allocated memory then exit
 *
 * @param vmr Pointer to the iVMR interface
 * @param msg Fatal error message
 */
static void terminate(PT_VMR vmr, char *msg)
{
    log_fatal(msg);
    free(vmr);
    exit(EXIT_FAILURE);
}

/**
 * @brief Prints the help message
 */
static void usage()
{
    puts(USAGE);
    exit(EXIT_SUCCESS);
}

/**
 * @brief Set the kind of Voicemeeter based on the value of -k flag.
 * For 64 bit systems the value is promoted to X64.
 *
 * @param kval Value of the -k flag
 * @return enum kind
 */
static enum kind set_kind(char *kval)
{
    if (strcmp(kval, "basic") == 0)
        return sizeof(void *) == 8 ? BASICX64 : BASIC;
    else if (strcmp(kval, "banana") == 0)
        return sizeof(void *) == 8 ? BANANAX64 : BANANA;
    else if (strcmp(kval, "potato") == 0)
        return sizeof(void *) == 8 ? POTATOX64 : POTATO;
    else
        return UNKNOWN;
}

/**
 * @brief Continuously read lines from stdin.
 * Break if 'Q' is entered on the interactive prompt.
 * Each line is passed to parse_input()
 *
 * @param vmr Pointer to the iVMR interface
 * @param with_prompt If true, prints the interactive prompt '>>'
 */
static void interactive(PT_VMR vmr, bool with_prompt)
{
    char input[MAX_LINE];
    size_t len;

    if (with_prompt)
        printf(">> ");
    while (fgets(input, MAX_LINE, stdin) != NULL)
    {
        input[strcspn(input, "\n")] = 0;
        if ((len = strlen(input)) == 1 && toupper(input[0]) == 'Q')
            break;

        parse_input(vmr, input);
        memset(input, 0, len); /* reset input buffer */

        if (with_prompt)
            printf(">> ");
    }
}

/**
 * @brief Returns early if input is a comment
 * Walks through each line split by " \t;," delimiters.
 * Each token is passed to parse_command()
 *
 * @param vmr Pointer to the iVMR interface
 * @param input Each input line, from stdin or CLI args
 */
static void parse_input(PT_VMR vmr, char *input)
{
    if (is_comment(input))
        return;

    char *token, *p;

    token = strtok_r(input, DELIMITERS, &p);
    while (token != NULL)
    {
        parse_command(vmr, token);
        token = strtok_r(NULL, DELIMITERS, &p);
    }
}

/**
 * @brief Execute each command according to type.
 * See command type definitions in:
 * https://github.com/onyx-and-iris/vmrcli?tab=readme-ov-file#api-commands
 *
 * @param vmr Pointer to the iVMR interface
 * @param command Each token from the input line as its own command string
 */
static void parse_command(PT_VMR vmr, char *command)
{
    log_debug("Parsing %s", command);

    static const struct quickcommand quickcommands[] = {
        {.name = "lock", .fullcommand = "command.lock=1"},
        {.name = "unlock", .fullcommand = "command.lock=0"},
        {.name = "show", .fullcommand = "command.show=1"},
        {.name = "hide", .fullcommand = "command.show=0"},
        {.name = "restart", .fullcommand = "command.restart=1"}};

    struct quickcommand *qc_ptr = command_in_quickcommands(command, quickcommands, (int)COUNT_OF(quickcommands));
    if (qc_ptr != NULL)
    {
        set_parameters(vmr, qc_ptr->fullcommand);
        if (vflag)
        {
            printf("Setting %s\n", qc_ptr->fullcommand);
        }
        return;
    }

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
            if (res.val.s[0] != '\0')
                printf("%s: %ls\n", command, res.val.s);
            break;
        default:
            break;
        }
    }
}

/**
 * @brief Get the value of a float or string parameter.
 * Stores its type and value into a result struct
 *
 * @param vmr Pointer to the iVMR interface
 * @param command A parsed 'get' command as a string
 * @param res Pointer to a struct holding the result of the API call.
 */
static void get(PT_VMR vmr, char *command, struct result *res)
{
    clear(vmr, is_pdirty);
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