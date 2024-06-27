#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "vmr.h"
#include "log.h"

long login(T_VBVMR_INTERFACE *iVMR, int kind)
{
    int rep;
    long v;

    rep = iVMR->VBVMR_Login();
    Sleep(20);
    if (rep == 1)
    {
        run_voicemeeter(iVMR, kind);
        switch (kind)
        {
        case BASIC:
            log_info("Launching Voicemeeter Basic GUI");
            break;
        case BANANA:
            log_info("Launching Voicemeeter Banana GUI");
            break;
        case POTATO:
            log_info("Launching Voicemeeter Potato GUI");
            break;
        case POTATOX64:
            log_info("Launching Voicemeeter Potato x64 GUI");
            break;
        }

        time_t endwait;
        int timeout = 2;

        endwait = time(NULL) + timeout;
        while (time(NULL) < endwait)
        {
            if ((rep = version(iVMR, &v)) == 0)
                break;
            Sleep(20);
        }
    }
    if (rep == 0)
    {
        version(iVMR, &v);
        long v1 = (v & 0xFF000000) >> 24,
             v2 = (v & 0x00FF0000) >> 16,
             v3 = (v & 0x0000FF00) >> 8,
             v4 = (v & 0x000000FF);
        char version_s[128];
        sprintf(version_s, "%i.%i.%i.%i", (int)v1, (int)v2, (int)v3, (int)v4);
        log_info("Successfully logged into the Voicemeeter API v%s", version_s);
        clear_dirty(iVMR);
    }
    return rep;
}

long logout(T_VBVMR_INTERFACE *iVMR)
{
    int rep;

    Sleep(20); /* give time for last command */
    rep = iVMR->VBVMR_Logout();
    if (rep == 0)
        log_info("Successfully logged out of the Voicemeeter API");
    return rep;
}

long run_voicemeeter(T_VBVMR_INTERFACE *iVMR, int kind)
{
    log_trace("VBVMR_RunVoicemeeter(%d)", kind);
    return iVMR->VBVMR_RunVoicemeeter((long)kind);
}

long type(T_VBVMR_INTERFACE *iVMR, long *type)
{
    return iVMR->VBVMR_GetVoicemeeterType(type);
}

long version(T_VBVMR_INTERFACE *iVMR, long *version)
{
    log_trace("VBVMR_GetVoicemeeterType(<long> *v)");
    return iVMR->VBVMR_GetVoicemeeterVersion(version);
}

bool is_pdirty(T_VBVMR_INTERFACE *iVMR)
{
    log_trace("VBVMR_IsParametersDirty()");
    return iVMR->VBVMR_IsParametersDirty() == 1;
}

long get_parameter_float(T_VBVMR_INTERFACE *iVMR, char *param, float *f)
{
    log_trace("VBVMR_GetParameterFloat(%s, <float> *f)", param, f);
    return iVMR->VBVMR_GetParameterFloat(param, f);
}

long get_parameter_string(T_VBVMR_INTERFACE *iVMR, char *param, char *s)
{
    log_trace("VBVMR_GetParameterStringA(%s, <char> *s)", param, s);
    return iVMR->VBVMR_GetParameterStringA(param, s);
}

long set_parameter_float(T_VBVMR_INTERFACE *iVMR, char *param, float val)
{
    log_trace("VBVMR_SetParameterFloat(%s, %.2f)", param, val);
    return iVMR->VBVMR_SetParameterFloat(param, val);
}

long set_parameter_string(T_VBVMR_INTERFACE *iVMR, char *param, char *s)
{
    log_trace("VBVMR_SetParameterStringA(%s, %s)", param, s);
    return iVMR->VBVMR_SetParameterStringA(param, s);
}

long set_parameters(T_VBVMR_INTERFACE *iVMR, char *command)
{
    log_trace("VBVMR_SetParameters(%s)", command);
    return iVMR->VBVMR_SetParameters(command);
}

bool is_mdirty(T_VBVMR_INTERFACE *iVMR)
{
    return iVMR->VBVMR_MacroButton_IsDirty() == 1;
}

long macrobutton_getstatus(T_VBVMR_INTERFACE *iVMR, long n, float *val, long mode)
{
    return iVMR->VBVMR_MacroButton_GetStatus(n, val, mode);
}

long macrobutton_setstatus(T_VBVMR_INTERFACE *iVMR, long n, float val, long mode)
{
    return iVMR->VBVMR_MacroButton_SetStatus(n, val, mode);
}

void clear_dirty(T_VBVMR_INTERFACE *iVMR)
{
    Sleep(30);
    while (is_pdirty(iVMR))
        Sleep(1);
}