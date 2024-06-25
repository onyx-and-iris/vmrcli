#include <stdbool.h>
#include "voicemeeterRemote.h"

#ifndef __VMR_H__
#define __VMR_H__

long login(T_VBVMR_INTERFACE *iVMR);
long logout(T_VBVMR_INTERFACE *iVMR);
long run_voicemeeter(T_VBVMR_INTERFACE *iVMR);
long type(T_VBVMR_INTERFACE *iVMR, long *type);
long version(T_VBVMR_INTERFACE *iVMR, long *version);

bool pdirty(T_VBVMR_INTERFACE *iVMR);
long set_parameters(T_VBVMR_INTERFACE *iVMR, char *command);

bool mdirty(T_VBVMR_INTERFACE *iVMR);
long macrobutton_getstatus(T_VBVMR_INTERFACE *iVMR, long n, float *val, long mode);
long macrobutton_setstatus(T_VBVMR_INTERFACE *iVMR, long n, float val, long mode);

void clear_dirty(T_VBVMR_INTERFACE *iVMR);

#endif