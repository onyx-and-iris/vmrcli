#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include "cdll.h"
#include "vmr.h"

#define MAX_LINE 1024

void help(char *progname);
int set_kind(char *kval);
int init_voicemeeter(T_VBVMR_INTERFACE *vmr, int kind);
void interactive(T_VBVMR_INTERFACE *vmr);

int main(int argc, char *argv[])
{
    bool iflag = false;
    int opt;
    char *kvalue = "";
    int kind = BANANA;

    while ((opt = getopt(argc, argv, "k:ih")) != -1)
    {
        switch (opt)
        {
        case 'i':
            iflag = true;
            break;
        case 'k':
            kvalue = optarg;
            break;
        case 'h':
            help(argv[0]);
            exit(EXIT_SUCCESS);
        default:
            abort();
        }
    }

    static T_VBVMR_INTERFACE iVMR;
    T_VBVMR_INTERFACE *vmr = &iVMR;
    if (kvalue && kvalue[0] != '\0')
    {
        kind = set_kind(kvalue);
    }

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
            set_parameters(vmr, argv[i]);
        }
    }

    rep = logout(vmr);
    if (rep == 0)
    {
        puts("Successfully logged out of Voicemeeter API");
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

void help(char *progname)
{
    printf(
        "Usage: ./%s [-i] [-k] <api commands>\n"
        "Where: \n"
        "\ti: Enable interactive mode\n"
        "\tk: The kind of Voicemeeter (basic, banana, potato)\n",
        progname);
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

    while (fgets(input, MAX_LINE, stdin) != NULL)
    {
        if (strlen(input) == 2 && (strncmp(input, "Q", 1) == 0 || strncmp(input, "q", 1) == 0))
            break;

        set_parameters(vmr, input);
    }
}