#ifndef STEADYSTATEPROBLEM_H
#define STEADYSTATEPROBLEM_H

#include "optimizationProblem.h"
#include "include/amici_interface_cpp.h"
#include <hdf5.h>

class SteadystateProblem : public OptimizationProblem
{
public:
    SteadystateProblem();

    int evaluateObjectiveFunction(const double *parameters, double *objFunVal, double *objFunGrad);

    int intermediateFunction(int alg_mod,
                             int iter_count,
                             double obj_value,
                             double inf_pr, double inf_du,
                             double mu,
                             double d_norm,
                             double regularization_size,
                             double alpha_du, double alpha_pr,
                             int ls_trials);


    void logObjectiveFunctionEvaluation(const double *parameters,
                                                double objectiveFunctionValue,
                                                const double *objectiveFunctionGradient,
                                                int numFunctionCalls,
                                                double timeElapsed);

    void logOptimizerFinished(double optimalCost,
                                const double *optimalParameters,
                                double masterTime,
                                int exitStatus);

    ~SteadystateProblem();

    void requireSensitivities(bool sensitivitiesRequired);
    void readFixedParameters(int conditionIdx);
    void readMeasurement(int conditionIdx);

    UserData *udata;
    ExpData *edata;

protected:

    void setupUserData(int conditionIdx);
    void setupExpData(int conditionIdx);

    hid_t fileId;

};

#endif // STEADYSTATEPROBLEM_H
