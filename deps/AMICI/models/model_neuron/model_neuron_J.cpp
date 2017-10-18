
#include <include/symbolic_functions.h>
#include <include/amici.h>
#include <include/amici_model.h>
#include <string.h>
#include <include/tdata.h>
#include <include/udata.h>
#include "model_neuron_dwdx.h"
#include "model_neuron_w.h"

int J_model_neuron(long int N, realtype t, realtype cj, N_Vector x, N_Vector dx, N_Vector xdot, DlsMat J, void *user_data, N_Vector tmp1, N_Vector tmp2, N_Vector tmp3) {
int status = 0;
TempData *tdata = (TempData*) user_data;
Model *model = (Model*) tdata->model;
UserData *udata = (UserData*) tdata->udata;
realtype *x_tmp = nullptr;
if(x)
    x_tmp = N_VGetArrayPointer(x);
realtype *dx_tmp = nullptr;
if(dx)
    dx_tmp = N_VGetArrayPointer(dx);
realtype *xdot_tmp = nullptr;
if(xdot)
    xdot_tmp = N_VGetArrayPointer(xdot);
int ix;
memset(J->data,0,sizeof(realtype)*4);
status = w_model_neuron(t,x,NULL,tdata);
status = dwdx_model_neuron(t,x,NULL,user_data);
  J->data[0+0*2] = x_tmp[0]*(2.0/2.5E1)+5.0;
  J->data[0+1*2] = -1.0;
  J->data[1+0*2] = tdata->p[0]*tdata->p[1];
  J->data[1+1*2] = -tdata->p[0];
for(ix = 0; ix<4; ix++) {
   if(amiIsNaN(J->data[ix])) {
       J->data[ix] = 0;
       if(!tdata->nan_J) {
           warnMsgIdAndTxt("AMICI:mex:fJ:NaN","AMICI replaced a NaN value in Jacobian and replaced it by 0.0. This will not be reported again for this simulation run.");
           tdata->nan_J = TRUE;
       }
   }
   if(amiIsInf(J->data[ix])) {
       warnMsgIdAndTxt("AMICI:mex:fJ:Inf","AMICI encountered an Inf value in Jacobian! Aborting simulation ... ");
       return(-1);
   }
}
return(status);

}

