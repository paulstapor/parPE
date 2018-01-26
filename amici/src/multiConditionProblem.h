#ifndef PROBLEM_H
#define PROBLEM_H

#include "LoadBalancerMaster.h"
#include "MultiConditionDataProvider.h"
#include "multiStartOptimization.h"
#include "optimizationProblem.h"
#include <LoadBalancerWorker.h>
#include <simulationWorkerAmici.h>
#include <cmath> //NAN
#include <amici.h>
#include <multiConditionProblemResultWriter.h>

namespace parpe {

class MultiConditionDataProvider;
class MultiConditionProblemResultWriter;


/**
 * @brief The MultiConditionGradientFunction class represents a cost function based on an AMICI ODE model
 */

class MultiConditionGradientFunction : public GradientFunction {
public:
    MultiConditionGradientFunction() {} // TODO remove?
    MultiConditionGradientFunction(MultiConditionDataProvider *dataProvider, LoadBalancerMaster *loadBalancer, MultiConditionProblemResultWriter *resultWriter = nullptr);


    /**
     * @brief Evaluate cost function at `optimiziationVariables`
     * @param optimiziationVariables Current parameters
     * @param objectiveFunctionValue Out: cost at `optimiziationVariables`
     * @param objectiveFunctionGradient Out: cost gradient at
     * `optimiziationVariables`
     * @return status code, non-zero on failure
     */

    FunctionEvaluationStatus evaluate(
            const double* const parameters,
            double &fval,
            double* gradient) const override;

    int numParameters() const override;

    /**
     * @brief Is called by worker processes to run a simulation for the given
     * condition
     * @param udata UserData for simulation. Model dimensions, sensitivity
     * options and UserData::p is set, others are not.
     * E.g. UserData::k has to be update if applicable.
     * @param dataProvider
     * @param path
     * @param jobId
     * @param resultWriter
     * @return
     */
    // TODO does not belong here
    JobResultAmiciSimulation runAndLogSimulation(amici::UserData *udata, JobIdentifier path,
                                    int jobId) const;


    /**
     * @brief Callback function for loadbalancer
     * @param buffer In/out: message buffer
     * @param msgSize In/out: size (bytes) of bufferobjFunVal
     * @param jobId: In: Identifier of the job (unique up to INT_MAX)
     */
    virtual void messageHandler(std::vector<char> &buffer, int jobId) const;


protected:// for testing
    void updateUserDataCommon(const double *simulationParameters,
                              const double *objectiveFunctionGradient) const;

    /**
     * @brief Run AMICI simulations for conditions with the given indices
     * @param optimizationVariables
     * @param logLikelihood
     * @param objectiveFunctionGradient
     * @param dataIndices
     * @param numDataIndices
     * @return Simulation status, != 0 indicates failure
     */
    virtual int runSimulations(const double *optimizationVariables,
                               double *logLikelihood,
                               double *objectiveFunctionGradient,
                               int *dataIndices, int numDataIndices) const;

    /**
     * @brief Aggregates loglikelihood received from workers.
     * @param data
     * @param logLikelihood
     * @param objectiveFunctionGradient
     * @param dataIndices
     * @param numDataIndices
     * @return *Negative* log likelihood.
     */

    int aggregateLikelihood(JobData &data, double *logLikelihood,
                            double *objectiveFunctionGradient, int dataIdx, double &simulationTimeInS) const;


    /**
     * @brief Aggregates loglikelihood gradient received from workers.
     * @param conditionIdx
     * @param simulationGradient
     * @param objectiveFunctionGradient
     * @param Gradient of the *negative* log likelihood.
     */

    void addSimulationGradientToObjectiveFunctionGradient(
        int conditionIdx, const double *simulationGradient,
        double *objectiveFunctionGradient, int numCommon) const;

    void
    addSimulationGradientToObjectiveFunctionGradientConditionSpecificParameters(
        const double *simulationGradient, double *objectiveFunctionGradient,
        int numCommon, int numConditionSpecificParams,
        int firstIndexOfCurrentConditionsSpecificOptimizationParameters) const;

    void queueSimulation(JobIdentifier path, JobData *d, int *jobDone,
                         pthread_cond_t *jobDoneChangedCondition,
                         pthread_mutex_t *jobDoneChangedMutex,
                         int lenSendBuffer);

    void setSensitivityOptions(bool sensiRequired) const;

private:

    FunctionEvaluationStatus evaluateObjectiveFunction(const double *optimiziationVariables,
                                          double *objectiveFunctionValue,
                                          double *objectiveFunctionGradient,
                                          int *dataIndices, int numDataIndices) const;

    MultiConditionDataProvider *dataProvider;
    LoadBalancerMaster *loadBalancer = nullptr;
    amici::Model *model = nullptr;
    std::unique_ptr<amici::UserData> udata;
    amici::UserData udataOriginal; // for saving sensitivity options which are changed depending on whether gradient is needed
    MultiConditionProblemResultWriter *resultWriter = nullptr;
};




/**
 * @brief The MultiConditionProblem class represents an optimization problem based
 * on an MultiConditionGradientFunction (AMICI ODE model)
 */

class MultiConditionProblem : public OptimizationProblem {

  public:
    MultiConditionProblem() = default;

    MultiConditionProblem(MultiConditionDataProvider *dataProvider);

    MultiConditionProblem(MultiConditionDataProvider *dataProvider,
                          LoadBalancerMaster *loadBalancer);

    ~MultiConditionProblem() = default;


    /**
     * @brief This function is called after each iteration.
     * @return status code, non-zero to abort optimization
     */
//    virtual int intermediateFunction(int alg_mod, int iter_count,
//                                     double obj_value, double inf_pr,
//                                     double inf_du, double mu, double d_norm,
//                                     double regularization_size,
//                                     double alpha_du, double alpha_pr,
//                                     int ls_trials) override;

    /**
     * @brief Called after each cost function evaluation for logging results.
     * @param parameters
     * @param objectiveFunctionValue
     * @param objectiveFunctionGradient
     * @param numFunctionCalls
     * @param timeElapsed
     */
//    virtual void logObjectiveFunctionEvaluation(
//        const double *parameters, double objectiveFunctionValue,
//        const double *objectiveFunctionGradient, int numFunctionCalls,
//        double timeElapsed) override;

    /**
     * @brief Called at the end of an optimization for logging results
     * @param optimalCost
     * @param optimalParameters
     * @param masterTime
     * @param exitStatus
     */
//    virtual void logOptimizerFinished(double optimalCost,
//                                      const double *optimalParameters,
//                                      double masterTime,
//                                      int exitStatus) override;


    void fillParametersMin(double *buffer) const override;
    void fillParametersMax(double *buffer) const override;
    void fillInitialParameters(double *buffer) const override;

    /**
     * @brief earlyStopping
     * @return stop the optimization run
     */
    virtual int earlyStopping();
    MultiConditionDataProvider *getDataProvider();
//    virtual std::unique_ptr<double[]> getInitialParameters(int multiStartIndex) const override;

    JobIdentifier path;

    virtual double getTime() const;

    std::unique_ptr<MultiConditionProblemResultWriter> resultWriter;

    MultiConditionGradientFunction *mcGradFun = nullptr;

    void setInitialParameters(std::vector<double> startingPoint);

    std::unique_ptr<OptimizationReporter> getReporter() const;

  protected:


    /**
     * @brief Keep information from last evaluation to avoid recomputation for
     * same parameters
     * @param optimizationParameters
     * @param objectiveFunctionValue
     * @param objectiveFunctionGradient
     */
    void
    storeCurrentFunctionEvaluation(const double *optimizationParameters,
                                   double objectiveFunctionValue,
                                   const double *objectiveFunctionGradient);
    //TODO
    std::unique_ptr<OptimizationProblem> validationProblem;

    MultiConditionDataProvider *dataProvider = nullptr;

private:
    std::vector<double> startingPoint;

};




/**
 * @brief The MultiConditionProblemGeneratorForMultiStart class generates new
 * MultiConditionProblem instances with proper DataProviders for multi-start
 * optimization
 */

class MultiConditionProblemMultiStartOptimizationProblem
    : public MultiStartOptimizationProblem {
  public:
    int getNumberOfStarts() const { return options.numStarts; }

    bool restartOnFailure() const { return options.retryOptimization; }

    std::unique_ptr<OptimizationProblem> getLocalProblem(int multiStartIndex) const override;

    MultiConditionDataProvider *dp = nullptr;
    OptimizationOptions options;
    MultiConditionProblemResultWriter *resultWriter = nullptr;
    amici::Model *model = nullptr;
    LoadBalancerMaster *loadBalancer = nullptr;
};



void printSimulationResult(JobIdentifier const& path, int jobId, const amici::UserData *udata, const amici::ReturnData *rdata, double timeSeconds);

} // namespace parpe

#endif
