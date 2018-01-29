#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "optimizationOptions.h"
#include "quadraticTestProblem.h"
#include <cmath>
#include <cstdio>

namespace parpe {

QuadraticTestProblem::QuadraticTestProblem()
    : OptimizationProblem(std::unique_ptr<GradientFunction>(new QuadraticGradientFunction())) {
    auto options = getOptimizationOptions();
    options.maxOptimizerIterations = 12;
    options.optimizer = OPTIMIZER_IPOPT;
    setOptimizationOptions(options);
}

void QuadraticTestProblem::fillParametersMin(double *buffer) const
{
    buffer[0] = -1e5;

}

void QuadraticTestProblem::fillParametersMax(double *buffer) const
{
    buffer[0] = 1e5;
}

std::unique_ptr<OptimizationReporter> QuadraticTestProblem::getReporter() const
{
    return std::unique_ptr<OptimizationReporter>(new OptimizationReporterTest());
}

std::unique_ptr<OptimizationProblem> QuadraticOptimizationMultiStartProblem::getLocalProblem(
        int multiStartIndex) const {
    return std::unique_ptr<OptimizationProblem>(new QuadraticTestProblem());
}

FunctionEvaluationStatus QuadraticGradientFunction::evaluate(
        const double * const parameters,
        double &fval, double *gradient) const
{
    fval = pow(parameters[0] + 1.0, 2) + 42.0;

    if (gradient) {
        mock().actualCall("testObjGrad");
        gradient[0] = 2.0 * parameters[0] + 2.0;
    } else {
        mock().actualCall("testObj");
    }

    return functionEvaluationSuccess;
}

int QuadraticGradientFunction::numParameters() const
{
    mock().actualCall("OptimizationReporterTest::numParameters");

    return 1;
}

bool OptimizationReporterTest::starting(int numParameters, const double * const initialParameters)
{
    mock().actualCall("OptimizationReporterTest::starting");

    return false;
}

bool OptimizationReporterTest::iterationFinished(int numParameters, const double * const parameters, double objectiveFunctionValue, const double * const objectiveFunctionGradient)
{
    mock().actualCall("OptimizationReporterTest::afterCostFunctionCall");

    return false;
}

bool OptimizationReporterTest::beforeCostFunctionCall(int numParameters, const double * const parameters)
{
    mock().actualCall("OptimizationReporterTest::beforeCostFunctionCall");

    return false;
}

bool OptimizationReporterTest::afterCostFunctionCall(int numParameters, const double * const parameters, double objectiveFunctionValue, const double * const objectiveFunctionGradient)
{
    mock().actualCall("OptimizationReporterTest::afterCostFunctionCall");

    if(printDebug) {
        if (objectiveFunctionGradient) {
            printf("g: x: %f f(x): %f f'(x): %f\n", parameters[0], objectiveFunctionValue, objectiveFunctionGradient[0]);
        } else {
            printf("f: x: %f f(x): %f\n", parameters[0], objectiveFunctionValue);
        }
    }


    return false;
}

void OptimizationReporterTest::finished(double optimalCost, const double *optimalParameters, int exitStatus)
{
    mock().actualCall("OptimizationReporterTest::finished").withIntParameter("exitStatus", exitStatus);
}

} // namespace parpe
