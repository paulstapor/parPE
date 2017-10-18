
#include <include/symbolic_functions.h>
#include <include/amici.h>
#include <include/amici_model.h>
#include <string.h>
#include <include/tdata.h>
#include <include/udata.h>
#include "model_jakstat_adjoint_o2_w.h"

int x0_model_jakstat_adjoint_o2(N_Vector x0, void *user_data) {
int status = 0;
TempData *tdata = (TempData*) user_data;
Model *model = (Model*) tdata->model;
UserData *udata = (UserData*) tdata->udata;
realtype *x0_tmp = nullptr;
if(x0)
    x0_tmp = N_VGetArrayPointer(x0);
memset(x0_tmp,0,sizeof(realtype)*162);
realtype t = udata->tstart;
  x0_tmp[0] = tdata->p[4];
  x0_tmp[45] = 1.0;
return(status);

}

