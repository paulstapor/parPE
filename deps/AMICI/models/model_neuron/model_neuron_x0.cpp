
#include <include/symbolic_functions.h>
#include <include/amici.h>
#include <include/amici_model.h>
#include <string.h>
#include <include/tdata.h>
#include <include/udata.h>
#include "model_neuron_w.h"

int x0_model_neuron(N_Vector x0, void *user_data) {
int status = 0;
TempData *tdata = (TempData*) user_data;
Model *model = (Model*) tdata->model;
UserData *udata = (UserData*) tdata->udata;
realtype *x0_tmp = nullptr;
if(x0)
    x0_tmp = N_VGetArrayPointer(x0);
memset(x0_tmp,0,sizeof(realtype)*2);
realtype t = udata->tstart;
  x0_tmp[0] = udata->k[0];
  x0_tmp[1] = udata->k[0]*tdata->p[1];
return(status);

}

