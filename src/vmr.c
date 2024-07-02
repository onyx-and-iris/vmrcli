#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "vmr.h"
#include "log.h"
#include "util.h"

#define VERSION_STR_LEN 128
#define KIND_STR_LEN 64

/**
 * @brief Logs into the API.
 * Tests for valid connection for up to 2 seconds.
 * If successful initializes the dirty parameters.
 *
 * @param vmr
 * @param kind
 * @return long
 */
long login(T_VBVMR_INTERFACE *vmr, int kind)
{
    int rep;
    long v;

    rep = vmr->VBVMR_Login();
    if (rep == 1)
    {
        run_voicemeeter(vmr, kind);
        char kind_s[KIND_STR_LEN];
        log_info(
            "Launching Voicemeeter %s GUI",
            kind_as_string(kind_s, kind, KIND_STR_LEN));
    }

    int timeout = 2;
    time_t start = time(NULL);
    do
    {
        if ((rep = version(vmr, &v)) == 0)
        {
            char version_s[VERSION_STR_LEN];
            log_info(
                "Successfully logged into the Voicemeeter API v%s",
                version_as_string(version_s, v, VERSION_STR_LEN));
            break;
        }
        Sleep(50);
    } while (time(NULL) < start + timeout);

    if (rep == 0)
    {
        clear_dirty(vmr);
    }
    return rep;
}

/**
 * @brief Logs out of the API giving a short wait to allow a
 * final instruction to complete.
 *
 * @param vmr The API interface as a struct
 * @return long VBVMR_Logout return value
 */
long logout(T_VBVMR_INTERFACE *vmr)
{
    int rep;

    Sleep(20); /* give time for last command */
    rep = vmr->VBVMR_Logout();
    if (rep == 0)
        log_info("Successfully logged out of the Voicemeeter API");
    return rep;
}

long run_voicemeeter(T_VBVMR_INTERFACE *vmr, int kind)
{
    log_trace("VBVMR_RunVoicemeeter(%d)", kind);
    return vmr->VBVMR_RunVoicemeeter((long)kind);
}

long type(T_VBVMR_INTERFACE *vmr, long *type)
{
    log_trace("VBVMR_GetVoicemeeterType(<long> *t)");
    return vmr->VBVMR_GetVoicemeeterType(type);
}

long version(T_VBVMR_INTERFACE *vmr, long *version)
{
    log_trace("VBVMR_GetVoicemeeterVersion(<long> *v)");
    return vmr->VBVMR_GetVoicemeeterVersion(version);
}

bool is_pdirty(T_VBVMR_INTERFACE *vmr)
{
    log_trace("VBVMR_IsParametersDirty()");
    return vmr->VBVMR_IsParametersDirty() == 1;
}

long get_parameter_float(T_VBVMR_INTERFACE *vmr, char *param, float *f)
{
    log_trace("VBVMR_GetParameterFloat(%s, <float> *f)", param);
    return vmr->VBVMR_GetParameterFloat(param, f);
}

long get_parameter_string(T_VBVMR_INTERFACE *vmr, char *param, unsigned short *s)
{
    log_trace("VBVMR_GetParameterStringW(%s, <unsigned short> *s)", param);
    return vmr->VBVMR_GetParameterStringW(param, s);
}

long set_parameter_float(T_VBVMR_INTERFACE *vmr, char *param, float val)
{
    log_trace("VBVMR_SetParameterFloat(%s, %.1f)", param, val);
    return vmr->VBVMR_SetParameterFloat(param, val);
}

long set_parameter_string(T_VBVMR_INTERFACE *vmr, char *param, char *s)
{
    log_trace("VBVMR_SetParameterStringA(%s, %s)", param, s);
    return vmr->VBVMR_SetParameterStringA(param, s);
}

long set_parameters(T_VBVMR_INTERFACE *vmr, char *command)
{
    log_trace("VBVMR_SetParameters(%s)", command);
    return vmr->VBVMR_SetParameters(command);
}

bool is_mdirty(T_VBVMR_INTERFACE *vmr)
{
    return vmr->VBVMR_MacroButton_IsDirty() == 1;
}

long macrobutton_getstatus(T_VBVMR_INTERFACE *vmr, long n, float *val, long mode)
{
    return vmr->VBVMR_MacroButton_GetStatus(n, val, mode);
}

long macrobutton_setstatus(T_VBVMR_INTERFACE *vmr, long n, float val, long mode)
{
    return vmr->VBVMR_MacroButton_SetStatus(n, val, mode);
}

void clear_dirty(T_VBVMR_INTERFACE *vmr)
{
    Sleep(30);
    while (is_pdirty(vmr))
        Sleep(1);
}