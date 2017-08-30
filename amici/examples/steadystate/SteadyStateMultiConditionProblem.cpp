#include "SteadyStateMultiConditionProblem.h"
#include "optimizationOptions.h"
#include <amici_model.h>
#include <cstdio>
#include <misc.h>
Model *getModel();

SteadyStateMultiConditionDataProvider::SteadyStateMultiConditionDataProvider(
    Model *model, std::string hdf5Filename)
    : MultiConditionDataProvider(model, hdf5Filename) {

    udata = model->getNewUserData();

    setupUserData(udata);
}

int SteadyStateMultiConditionDataProvider::updateFixedSimulationParameters(
    int conditionIdx, UserData *udata) const {
    hdf5Read2DDoubleHyperslab(fileId, "/data/k", model->nk, 1, 0, conditionIdx,
                              udata->k);
    return 0;
}

ExpData *SteadyStateMultiConditionDataProvider::getExperimentalDataForCondition(
    int conditionIdx, const UserData *udata) const {
    ExpData *edata = new ExpData(udata, model);

    hdf5Read3DDoubleHyperslab(fileId, "/data/ymeasured", 1, model->ny,
                              udata->nt, conditionIdx, 0, 0, edata->my);

    double ysigma = AMI_HDF5_getDoubleScalarAttribute(fileId, "data", "sigmay");
    fillArray(edata->sigmay, model->nytrue * udata->nt, ysigma);

    return edata;
}

void SteadyStateMultiConditionDataProvider::setupUserData(
    UserData *udata) const {

    udata->nt = 20;

    hsize_t length;
    AMI_HDF5_getDoubleArrayAttribute(fileId, "data", "t", &udata->ts, &length);
    assert(length == (unsigned)udata->nt);
    udata->qpositivex = new double[model->nx];
    fillArray(udata->qpositivex, model->nx, 1);

    // calculate sensitivities for all parameters
    udata->plist = new int[model->np];
    udata->nplist = model->np;
    for (int i = 0; i < model->np; ++i)
        udata->plist[i] = i;
    udata->p = new double[model->np];

    // set model constants
    udata->k = new double[model->nk];
    updateFixedSimulationParameters(0, udata);
    udata->maxsteps = 1e5;

    udata->pscale = AMICI_SCALING_LOG10;

    udata->sensi = AMICI_SENSI_ORDER_FIRST;
    udata->sensi_meth = AMICI_SENSI_FSA;
}

UserData *SteadyStateMultiConditionDataProvider::getUserData() const {
    UserData *udata = model->getNewUserData();
    setupUserData(udata);
    return udata;
}

SteadyStateMultiConditionDataProvider::
    ~SteadyStateMultiConditionDataProvider() {
    delete udata;
}

SteadyStateMultiConditionProblem::SteadyStateMultiConditionProblem(
    SteadyStateMultiConditionDataProvider *dp, LoadBalancerMaster *loadBalancer)
    : MultiConditionProblem(dp, loadBalancer) {

    numOptimizationParameters = model->np;

    initialParameters = new double[numOptimizationParameters];
    fillArray(initialParameters, model->np, 0);

    delete[] parametersMin; // TODO: allocated by base class
    parametersMin = new double[numOptimizationParameters];
    fillArray(parametersMin, model->np, -5);

    delete[] parametersMax; // TODO: allocated by base class
    parametersMax = new double[numOptimizationParameters];
    fillArray(parametersMax, model->np, 5);

    optimizationOptions = new OptimizationOptions();
    optimizationOptions->optimizer = OPTIMIZER_IPOPT;
    optimizationOptions->printToStdout = true;
    optimizationOptions->maxOptimizerIterations = 30;
}

void SteadyStateMultiConditionProblem::setSensitivityOptions(
    bool sensiRequired) {
    // sensitivities requested?
    if (sensiRequired) {
        udata->sensi = AMICI_SENSI_ORDER_FIRST;
        udata->sensi_meth = AMICI_SENSI_FSA;
    } else {
        udata->sensi = AMICI_SENSI_ORDER_NONE;
        udata->sensi_meth = AMICI_SENSI_NONE;
    }
}

SteadyStateMultiConditionProblem::~SteadyStateMultiConditionProblem() {}
