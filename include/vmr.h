#ifndef __VMR_H__
#define __VMR_H__

#include <stdbool.h>
#include "voicemeeterRemote.h"

enum kind
{
    UNKNOWN = -1,
    BASIC = 1,
    BANANA,
    POTATO,
    BASICX64,
    BANANAX64,
    POTATOX64,
    MACROBUTTONS = 11,
    STREAMERVIEW
};

long login(T_VBVMR_INTERFACE *vmr, int kind);
long logout(T_VBVMR_INTERFACE *vmr);
long run_voicemeeter(T_VBVMR_INTERFACE *vmr, int kind);
long type(T_VBVMR_INTERFACE *vmr, long *type);
long version(T_VBVMR_INTERFACE *vmr, long *version);

bool is_pdirty(T_VBVMR_INTERFACE *vmr);
long get_parameter_float(T_VBVMR_INTERFACE *vmr, char *param, float *f);
long get_parameter_string(T_VBVMR_INTERFACE *vmr, char *param, unsigned short *s);
long set_parameter_float(T_VBVMR_INTERFACE *vmr, char *param, float val);
long set_parameter_string(T_VBVMR_INTERFACE *vmr, char *param, char *s);
long set_parameters(T_VBVMR_INTERFACE *vmr, char *command);

bool is_mdirty(T_VBVMR_INTERFACE *vmr);
long macrobutton_getstatus(T_VBVMR_INTERFACE *vmr, long n, float *val, long mode);
long macrobutton_setstatus(T_VBVMR_INTERFACE *vmr, long n, float val, long mode);

void clear_dirty(T_VBVMR_INTERFACE *vmr);

#endif /* __VMR_H__ */