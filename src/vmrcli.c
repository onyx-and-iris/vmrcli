#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include "cdll.h"
#include "vmr.h"

#define MAX_LINE 1024

int init_voicemeeter(T_VBVMR_INTERFACE *vmr);
void interactive(T_VBVMR_INTERFACE *vmr);

int main(int argc, char *argv[])
{
    bool iflag = false;
    int c;

    while ((c = getopt(argc, argv, "i")) != -1)
    {
        switch (c)
        {
        case 'i':
            iflag = true;
            break;
        default:
            abort();
        }
    }

    static T_VBVMR_INTERFACE iVMR;
    T_VBVMR_INTERFACE *vmr = &iVMR;

    int rep = init_voicemeeter(vmr);
    if (rep)
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
    if (!rep)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}

int init_voicemeeter(T_VBVMR_INTERFACE *vmr)
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

    rep = login(vmr);
    if (rep != 0)
    {
        fputs("Error logging into Voicemeeter", stderr);
        return rep;
    }
    clear_dirty(vmr);

    return 0;
}

void interactive(T_VBVMR_INTERFACE *vmr)
{
    char input[MAX_LINE];

    while (fgets(input, MAX_LINE, stdin) != NULL)
    {
        if (strncmp(input, "Q", 1) == 0 || strncmp(input, "q", 1) == 0)
            break;

        set_parameters(vmr, input);
    }
}