#include <stdbool.h>
#include "voicemeeterRemote.h"

#ifndef __VMR_H__
#define __VMR_H__

enum kind
{
    BASIC = 1,
    BANANA,
    POTATO,
    POTATOX64 = 6
};

long login(T_VBVMR_INTERFACE *iVMR, int kind);
long logout(T_VBVMR_INTERFACE *iVMR);
long run_voicemeeter(T_VBVMR_INTERFACE *iVMR, int kind);
long type(T_VBVMR_INTERFACE *iVMR, long *type);
long version(T_VBVMR_INTERFACE *iVMR, long *version);

bool pdirty(T_VBVMR_INTERFACE *iVMR);
long get_parameter_float(T_VBVMR_INTERFACE *iVMR, char *param, float *f);
long get_parameter_string(T_VBVMR_INTERFACE *iVMR, char *param, char *s);
long set_parameter_float(T_VBVMR_INTERFACE *iVMR, char *param, float val);
long set_parameter_string(T_VBVMR_INTERFACE *iVMR, char *param, char *s);
long set_parameters(T_VBVMR_INTERFACE *iVMR, char *command);

bool mdirty(T_VBVMR_INTERFACE *iVMR);
long macrobutton_getstatus(T_VBVMR_INTERFACE *iVMR, long n, float *val, long mode);
long macrobutton_setstatus(T_VBVMR_INTERFACE *iVMR, long n, float val, long mode);

void clear_dirty(T_VBVMR_INTERFACE *iVMR);

#endif