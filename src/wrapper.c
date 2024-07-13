/**
 * @file wrapper.c
 * @author Onyx and Iris (code@onyxandiris.online)
 * @brief Provides public functions that wrap the iVMR calls
 * @version 0.10.0
 * @date 2024-07-06
 *
 * @copyright Copyright (c) 2024
 * https://github.com/onyx-and-iris/vmrcli/blob/main/LICENSE
 */

#include <windows.h>
#include "wrapper.h"
#include "log.h"
#include "util.h"

#define KIND_STR_LEN 64
#define VERSION_STR_LEN 32
#define LOGIN_TIMEOUT 2

/**
 * @brief Logs into the API.
 * Tests for valid connection for up to 2 seconds.
 * If successful initializes the dirty parameters.
 *
 * @param vmr Pointer to the iVMR interface
 * @param kind The kind of Voicemeeter Gui to launch.
 * @return long
 *  0: OK (no error).
 * -2: Login timed out.
 */
long login(PT_VMR vmr, int kind)
{
    long rep;
    long v;

    log_trace("VBVMR_Login()");
    rep = vmr->VBVMR_Login();
    if (rep == 1)
    {
        run_voicemeeter(vmr, kind);
        char kind_s[KIND_STR_LEN];
        log_info(
            "Launching Voicemeeter %s GUI",
            kind_as_string(kind_s, kind, KIND_STR_LEN));
    }

    time_t start = time(NULL);
    do
    {
        if ((rep = version(vmr, &v)) == 0)
        {
            char version_s[VERSION_STR_LEN];
            log_info(
                "Successfully logged into the Voicemeeter API v%s",
                version_as_string(version_s, v, VERSION_STR_LEN));
            clear(vmr, is_pdirty);
            break;
        }
        Sleep(50);
    } while (difftime(time(NULL), start) < LOGIN_TIMEOUT);

    return rep;
}

/**
 * @brief Logs out of the API giving a short wait to allow a
 * final instruction to complete.
 *
 * @param vmr Pointer to the iVMR interface
 * @return long See:
 * https://github.com/onyx-and-iris/vmrcli/blob/main/include/VoicemeeterRemote.h#L56
 */
long logout(PT_VMR vmr)
{
    Sleep(20); /* give time for last command */
    log_trace("VBVMR_Logout()");
    return vmr->VBVMR_Logout();
}

/**
 * @brief Launches Voicemeeter or other utility apps
 *
 * @param vmr Pointer to the iVMR interface
 * @param kind The kind of app to launch
 * @return long See:
 * https://github.com/onyx-and-iris/vmrcli/blob/main/include/VoicemeeterRemote.h#L66
 */
long run_voicemeeter(PT_VMR vmr, int kind)
{
    log_trace("VBVMR_RunVoicemeeter(%d)", kind);
    return vmr->VBVMR_RunVoicemeeter((long)kind);
}

/**
 * @brief Get Voicemeeter type
 *
 * @param vmr Pointer to the iVMR interface
 * @param type Pointer to a long object receiving the type
 * @return long See:
 * https://github.com/onyx-and-iris/vmrcli/blob/main/include/VoicemeeterRemote.h#L107
 */
long type(PT_VMR vmr, long *type)
{
    log_trace("VBVMR_GetVoicemeeterType(<long> *t)");
    return vmr->VBVMR_GetVoicemeeterType(type);
}

/**
 * @brief Get Voicemeeter version
 *
 * @param vmr Pointer to the iVMR interface
 * @param version Pointer to a long object receiving the version
 * @return long See:
 * https://github.com/onyx-and-iris/vmrcli/blob/main/include/VoicemeeterRemote.h#L122
 */
long version(PT_VMR vmr, long *version)
{
    log_trace("VBVMR_GetVoicemeeterVersion(<long> *v)");
    return vmr->VBVMR_GetVoicemeeterVersion(version);
}

/**
 * @brief Polling function, use it to determine if there are parameter
 * states to be updated.
 *
 * @param vmr Pointer to the iVMR interface
 * @return true New parameters yet to be updated
 * @return false No new parameters, safe to make a get call
 */
bool is_pdirty(PT_VMR vmr)
{
    log_trace("VBVMR_IsParametersDirty()");
    return vmr->VBVMR_IsParametersDirty() == 1;
}

/**
 * @brief Get the parameter float object
 *
 * @param vmr Pointer to the iVMR interface
 * @param param The parameter to be queried
 * @param f Pointer to a float object receiving the value
 * @return long See:
 * https://github.com/onyx-and-iris/vmrcli/blob/main/include/VoicemeeterRemote.h#L159
 */
long get_parameter_float(PT_VMR vmr, char *param, float *f)
{
    log_trace("VBVMR_GetParameterFloat(%s, <float> *f)", param);
    return vmr->VBVMR_GetParameterFloat(param, f);
}

/**
 * @brief Get the parameter string object
 *
 * @param vmr Pointer to the iVMR interface
 * @param param The parameter to be queried
 * @param s Pointer to a character buffer receiving the string value
 * @return long See:
 * https://github.com/onyx-and-iris/vmrcli/blob/main/include/VoicemeeterRemote.h#L173
 */
long get_parameter_string(PT_VMR vmr, char *param, wchar_t *s)
{
    log_trace("VBVMR_GetParameterStringW(%s, <wchar_t> *s)", param);
    return vmr->VBVMR_GetParameterStringW(param, s);
}

/**
 * @brief Set the parameter float object
 *
 * @param vmr Pointer to the iVMR interface
 * @param param The parameter to be updated
 * @param val The new value
 * @return long See:
 * https://github.com/onyx-and-iris/vmrcli/blob/main/include/VoicemeeterRemote.h#L309
 */
long set_parameter_float(PT_VMR vmr, char *param, float val)
{
    log_trace("VBVMR_SetParameterFloat(%s, %.1f)", param, val);
    return vmr->VBVMR_SetParameterFloat(param, val);
}

/**
 * @brief Set the parameter string object
 *
 * @param vmr Pointer to the iVMR interface
 * @param param The parameter to be updated
 * @param s Pointer to a char[] object containing the new value
 * @return long See:
 * https://github.com/onyx-and-iris/vmrcli/blob/main/include/VoicemeeterRemote.h#L327
 */
long set_parameter_string(PT_VMR vmr, char *param, char *s)
{
    log_trace("VBVMR_SetParameterStringA(%s, %s)", param, s);
    return vmr->VBVMR_SetParameterStringA(param, s);
}

/**
 * @brief Run a script possibly containing multiple instructions
 *
 * @param vmr Pointer to the iVMR interface
 * @param command Pointer to a char[] object containing the script
 * @return long See:
 * https://github.com/onyx-and-iris/vmrcli/blob/main/include/VoicemeeterRemote.h#L351
 */
long set_parameters(PT_VMR vmr, char *command)
{
    log_trace("VBVMR_SetParameters(%s)", command);
    return vmr->VBVMR_SetParameters(command);
}

/**
 * @brief Polling function, use it to determine if there are macrobutton
 * states to be updated.
 *
 * @param vmr Pointer to the iVMR interface
 * @return true Macrobutton states yet to be udpated
 * @return false No new macrobutton states
 */
bool is_mdirty(PT_VMR vmr)
{
    log_trace("VBVMR_MacroButton_IsDirty()");
    return vmr->VBVMR_MacroButton_IsDirty() >= 0;
}

/**
 * @brief Get the current status of macrobutton[n].{mode}
 *
 * @param vmr Pointer to the iVMR interface
 * @param n Index of the macrobutton
 * @param val Pointer to a float object the current value will be stored in
 * @param mode The mode (stateonly, state, trigger)
 * @return long See:
 * https://github.com/onyx-and-iris/vmrcli/blob/main/include/VoicemeeterRemote.h#L663
 */
long macrobutton_getstatus(PT_VMR vmr, long n, float *val, long mode)
{
    log_trace("VBVMR_MacroButton_GetStatus(%ld, <float> *v, %ld)", n, mode);
    return vmr->VBVMR_MacroButton_GetStatus(n, val, mode);
}

/**
 * @brief Set the current status of macrobutton[n].{mode}
 *
 * @param vmr Pointer to the iVMR interface
 * @param n Index of the macrobutton
 * @param val Value to be updated
 * @param mode The mode (stateonly, state, trigger)
 * @return long See:
 * https://github.com/onyx-and-iris/vmrcli/blob/main/include/VoicemeeterRemote.h#L677
 */
long macrobutton_setstatus(PT_VMR vmr, long n, float val, long mode)
{
    log_trace("VBVMR_MacroButton_SetStatus(%ld, %d, %ld)", n, (int)val, mode);
    return vmr->VBVMR_MacroButton_SetStatus(n, val, mode);
}

/**
 * @brief Continuously polls an is_{}dirty function until it clears.
 *
 * @param vmr Pointer to the iVMR interface
 * @param f Pointer to a polling function
 */
void clear(PT_VMR vmr, bool (*f)(PT_VMR))
{
    Sleep(30);
    while (f(vmr))
        Sleep(1);
}
