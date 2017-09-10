
#include <include/symbolic_functions.h>
#include <include/amici.h>
#include <include/amici_model.h>
#include <string.h>
#include <include/tdata.h>
#include <include/udata.h>
#include "model_nested_events_w.h"

int deltax_model_nested_events(realtype t, int ie, N_Vector x, N_Vector xdot, N_Vector xdot_old, TempData *tdata) {
int status = 0;
Model *model = (Model*) tdata->model;
UserData *udata = (UserData*) tdata->udata;
realtype *x_tmp = N_VGetArrayPointer(x);
realtype *xdot_tmp = N_VGetArrayPointer(xdot);
realtype *xdot_old_tmp = N_VGetArrayPointer(xdot_old);
memset(tdata->deltax,0,sizeof(realtype)*1);
status = w_model_nested_events(t,x,NULL,tdata);
              switch(ie) { 
              case 0: {
  tdata->deltax[0] = tdata->p[1];

              } break;

              case 2: {
  tdata->deltax[0] = tdata->p[1];

              } break;

              } 
return(status);

}


