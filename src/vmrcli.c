/**
 * @file vmrcli.c
 * @author Onyx and Iris (code@onyxandiris.online)
 * @brief A Voicemeeter Remote Command Line Interface
 * @version 0.14.1
 * @date 2024-07-06
 *
 * @copyright Copyright (c) 2024
 * https://github.com/onyx-and-iris/vmrcli/blob/main/LICENSE
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <windows.h>
#include "interface.h"
#include "wrapper.h"
#include "log.h"
#include "util.h"

#define USAGE "Usage: .\\vmrcli.exe [-h] [-v] [-i|-I] [-f] [-k] [-l] [-e] [-c] [-m] [-s] <api commands>\n" \
              "Where: \n"                                                                        \
              "\t-h, --help: Print the help message\n"                                          \
              "\t-v, --version: Print the version number\n"                                     \
              "\t-i, --interactive: Enable interactive mode\n"                                  \
              "\t-I, --no-prompt: Enable interactive mode without the '>>' prompt\n"             \
              "\t-f, --full-line: Do not split input on spaces\n"                               \
              "\t-k, --kind: The kind of Voicemeeter (basic, banana, potato)\n"                  \
              "\t-l, --log-level: Set log level, must be one of TRACE, DEBUG, INFO, WARN, ERROR, or FATAL\n" \
              "\t-e, --extra-output: Enable extra console output (toggle, set messages)\n"      \
              "\t-c, --config: Load a user configuration (give the full file path)\n"          \
              "\t-m, --macrobuttons: Launch the MacroButtons application\n"                     \
              "\t-s, --streamerview: Launch the StreamerView application"
#define OPTSTR ":hvk:msc:iIfl:e"
#define MAX_LINE 4096 /* Size of the input buffer */
#define RES_SZ 512    /* Size of the buffer passed to VBVMR_GetParameterStringW */
#define COUNT_OF(x) (sizeof(x) / sizeof(x[0]))
#define DELIMITERS " \t;,"
#define VERSION "0.14.1"

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

/**
 * @struct A struct to hold the program configuration, set by CLI args
 */
struct config_t {
    bool mflag;
    bool sflag;
    bool cflag;
    char *cvalue;
    bool iflag;
    bool with_prompt;
    bool fflag;
    bool eflag;
    int log_level;
    enum kind kind;
};

/**
 * @struct A struct to hold the program context, including the config and the iVMR interface pointer
 */
struct context_t {
    struct config_t config;
    PT_VMR vmr;
};

static void terminate(PT_VMR vmr, char *msg);
static void usage();
static enum kind set_kind(char *kval);
static void interactive(const struct context_t *context, char *delimiters);
static void parse_input(const struct context_t *context, char *input, char *delimiters);
static void parse_command(const struct context_t *context, char *command);
static void get(PT_VMR vmr, char *command, struct result *res);

int get_options(struct config_t *config, int argc, char *argv[])
{
    static const struct option options[] =
    {
        {"help",   no_argument,  0, 'h'},
        {"version",no_argument,  0, 'v'},
        {"kind",   required_argument,  0, 'k'},
        {"macrobuttons", no_argument,   0, 'm'},
        {"streamerview", no_argument,   0, 's'},
        {"config", required_argument,  0, 'c'},
        {"interactive", no_argument,    0, 'i'},
        {"no-prompt", no_argument,      0, 'I'},
        {"full-line", no_argument,      0, 'f'},
        {"log-level", required_argument,0, 'l'},
        {"extra-output", no_argument,   0, 'e'},
        {NULL,             0,                  NULL,  0 }
    };

    config->with_prompt = true;
    config->log_level = LOG_WARN;
    config->kind = BANANAX64;

    if (argc == 1)
    {
        usage();
    }

    log_set_level(config->log_level);

    opterr = 0;
    int opt;
    while ((opt = getopt_long(argc, argv, OPTSTR, options, NULL)) != -1)
    {
        switch (opt)
        {
        case 'v':
            printf("vmrcli version %s\n", VERSION);
            exit(EXIT_SUCCESS);
        case 'k':
            config->kind = set_kind(optarg);
            if (config->kind == UNKNOWN)
            {
                log_fatal("Unknown Voicemeeter kind '%s'", optarg);
                exit(EXIT_FAILURE);
            }
            break;
        case 'm':
            config->mflag = true;
            break;
        case 's':
            config->sflag = true;
            break;
        case 'c':
            config->cflag = true;
            config->cvalue = optarg;
            break;
        case 'I':
            config->with_prompt = false;
            [[fallthrough]];
        case 'i':
            config->iflag = true;
            break;
        case 'f':
            config->fflag = true;
            break;
        case 'l':
            config->log_level = log_level_from_string(optarg);
            if (config->log_level != -1)
            {
                log_set_level(config->log_level);
            }
            else
            {
                log_warn(
                    "-l arg out of range, expected TRACE, DEBUG, INFO, WARN, ERROR, or FATAL\n"
                    "Log level will default to LOG_WARN (3).\n");
            }
            break;
        case 'e':
            config->eflag = true;
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
    return optind;
}

int main(int argc, char *argv[])
{
    struct context_t context = {0};
    int optind = get_options(&context.config, argc, argv);

    log_set_level(context.config.log_level);

    context.vmr = create_interface();
    if (context.vmr == NULL)
    {
        exit(EXIT_FAILURE);
    }

    long rep = login(context.vmr, context.config.kind);
    if (rep != 0)
    {
        if (rep == -2)
            terminate(context.vmr, "Timeout logging into the API.");
        else
            terminate(context.vmr, "Error logging into the Voicemeeter API");
    }

    if (context.config.mflag)
    {
        run_voicemeeter(context.vmr, MACROBUTTONS);
        log_info("MacroButtons app launched");
    }

    if (context.config.sflag)
    {
        run_voicemeeter(context.vmr, STREAMERVIEW);
        log_info("StreamerView app launched");
    }

    if (context.config.cflag)
    {
        set_parameter_string(context.vmr, "command.load", context.config.cvalue);
        log_info("Profile %s loaded", context.config.cvalue);
        Sleep(300);
        clear(context.vmr, is_pdirty);
    }

    char *delimiter_ptr = DELIMITERS;
    if (context.config.fflag)
    {
        delimiter_ptr++; /* skip space delimiter */
    }

    if (context.config.iflag)
    {
        puts("Interactive mode enabled. Enter 'Q' to exit.");
        interactive(&context, delimiter_ptr);
    }
    else
    {
        for (int i = optind; i < argc; ++i)
        {
            parse_input(&context, argv[i], delimiter_ptr);
        }
    }

    rep = logout(context.vmr);
    if (rep != 0)
    {
        terminate(context.vmr, "Error logging out of the Voicemeeter API");
    }

    log_info("Successfully logged out of the Voicemeeter API");
    free(context.vmr);
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
        return IS_64_BIT ? BASICX64 : BASIC;
    else if (strcmp(kval, "banana") == 0)
        return IS_64_BIT ? BANANAX64 : BANANA;
    else if (strcmp(kval, "potato") == 0)
        return IS_64_BIT ? POTATOX64 : POTATO;
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
 * @param delimiters A string of delimiter characters to split each input line
 */
static void interactive(const struct context_t *context, char *delimiters)
{
    char input[MAX_LINE];
    size_t len;

    if (context->config.with_prompt)
        printf(">> ");
    while (fgets(input, MAX_LINE, stdin) != NULL)
    {
        input[(len = strcspn(input, "\n"))] = 0;
        if (len == 1 && toupper(input[0]) == 'Q')
            break;

        parse_input(context, input, delimiters);

        if (context->config.with_prompt)
            printf(">> ");
    }
}

/* Helper functions for parse_input */
static inline bool is_quote_char(char c) {
    return (c == '"' || c == '\'');
}

static inline bool is_delimiter_char(char c, const char *delimiters) {
    return strchr(delimiters, c) != NULL;
}

static char* skip_consecutive_delimiters(char *p, const char *delimiters) {
    while (*p != '\0' && is_delimiter_char(*p, delimiters)) {
        p++;
    }
    return p;
}

static bool add_char_to_token(char *token, size_t *token_len, char c, size_t max_len) {
    if (*token_len < max_len - 1) {
        token[(*token_len)++] = c;
        return true;
    }
    return false; // Buffer would overflow
}

/**
 * @brief Parse each input line into separate commands and execute them.
 * Commands are split based on the delimiters argument, but quoted strings are preserved as single commands.
 * See the test cases for examples of how input lines are parsed:
 * https://github.com/onyx-and-iris/vmrcli?tab=readme-ov-file#api-commands
 * @param vmr Pointer to the iVMR interface
 * @param input Each input line, from stdin or CLI args
 * @param delimiters A string of delimiter characters to split each input line
 */
static void parse_input(const struct context_t *context, char *input, char *delimiters)
{
    if (is_comment(input))
        return;

    char *current = input;
    char token[MAX_LINE];
    size_t token_length = 0;
    bool inside_quotes = false;
    char quote_char = '\0';

    while (*current != '\0')
    {
        if (!inside_quotes && is_quote_char(*current))
        {
            inside_quotes = true;
            quote_char = *current;
            current++;
            log_trace("Entering quotes with char '%c'", quote_char);
            continue;
        }
        else if (inside_quotes && *current == quote_char)
        {
            inside_quotes = false;
            quote_char = '\0';
            current++;
            log_trace("Exiting quotes");
            continue;
        }
        else if (!inside_quotes && is_delimiter_char(*current, delimiters))
        {
            if (token_length > 0)
            {
                token[token_length] = '\0';
                parse_command(context, token);
                token_length = 0;
            }
            
            current = skip_consecutive_delimiters(current, delimiters);
            continue;
        }
        else
        {
            if (!add_char_to_token(token, &token_length, *current, MAX_LINE))
            {
                log_error("Input token exceeds maximum length of %d characters", MAX_LINE - 1);
                return;
            }
            log_trace("Added char '%c' to token, current token: '%.*s'", *current, (int)token_length, token);
        }
        current++;
    }

    if (token_length > 0)
    {
        token[token_length] = '\0';
        parse_command(context, token);
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
static void parse_command(const struct context_t *context, char *command)
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
        set_parameters(context->vmr, qc_ptr->fullcommand);
        if (context->config.eflag) {
            printf("Setting %s\n", qc_ptr->fullcommand);
        }
        return;
    }

    if (command[0] == '!') /* toggle */
    {
        command++;
        struct result res = {.type = FLOAT_T};

        get(context->vmr, command, &res);
        if (res.type == FLOAT_T)
        {
            if (res.val.f == 1 || res.val.f == 0)
            {
                set_parameter_float(context->vmr, command, 1 - res.val.f);
                if (context->config.eflag) {
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
        char quoted_command[MAX_LINE];
        
        if (add_quotes_if_needed(command, quoted_command, MAX_LINE))
        {
            set_parameters(context->vmr, quoted_command);
            if (context->config.eflag) {
                printf("Setting %s\n", command);
            }
        }
        else
        {
            log_error("Command too long after adding quotes");
        }
    }
    else /* get */
    {
        struct result res = {.type = FLOAT_T};

        get(context->vmr, command, &res);
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