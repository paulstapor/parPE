#include "localOptimizationIpopt.h"
#include "localOptimizationIpoptTNLP.h"
#include "logging.h"
#include "optimizationOptions.h"
#include "optimizationProblem.h"
#include <alloca.h>
#include <cassert>
#include <cmath>
#include <csignal>
#include <cstdbool>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <pthread.h>

#ifdef INSTALL_SIGNAL_HANDLER
extern volatile sig_atomic_t caughtTerminationSignal;
#endif

static_assert(sizeof(double) == sizeof(Number),
              "Sizeof IpOpt::Number != sizeof double");

/**
 * @brief ipoptMutex Ipopt seems not to be thread safe. Lock this mutex every
 * time that control is passed to ipopt functions.
 */
static pthread_mutex_t ipoptMutex = PTHREAD_MUTEX_INITIALIZER;

using namespace Ipopt;

void setIpOptOptions(SmartPtr<IpoptApplication> app,
                     OptimizationProblem *problem) {
    if (problem->optimizationOptions->printToStdout) {
        app->Options()->SetIntegerValue("print_level", 5);
        app->Options()->SetStringValue("print_user_options", "yes");
    } else {
        app->Options()->SetIntegerValue("print_level", 0);
        app->Options()->SetStringValue("print_user_options", "no");
        app->Options()->SetStringValue("sb",
                                       "yes"); // suppress copyright message
    }

    //    app->Options()->SetStringValue("derivative_test", "first-order");
    //    app->Options()->SetIntegerValue("derivative_test_first_index", 4266);

    app->Options()->SetStringValue("hessian_approximation", "limited-memory");
    app->Options()->SetStringValue("limited_memory_update_type", "bfgs");

    app->Options()->SetIntegerValue(
        "max_iter", problem->optimizationOptions->maxOptimizerIterations);
    app->Options()->SetNumericValue("tol", 1e-9);

    app->Options()->SetIntegerValue("acceptable_iter", 1);
    app->Options()->SetNumericValue("acceptable_tol",
                                    1e20); // set ridiculously high, so only the
                                           // acceptable_* options below matter
    // AddIpoptNumOption(nlp, "acceptable_constr_viol_tol", 1e20);
    // AddIpoptNumOption(nlp, "acceptable_dual_inf_tol", 1e20);
    // AddIpoptNumOption(nlp, "acceptable_compl_inf_tol", 1e20);
    app->Options()->SetNumericValue(
        "acceptable_obj_change_tol",
        problem->optimizationOptions->functionTolerance);

    app->Options()->SetIntegerValue("accept_every_trial_step", problem->optimizationOptions->accept_every_trial_step);
    app->Options()->SetIntegerValue("watchdog_shortened_iter_trigger", problem->optimizationOptions->watchdog_shortened_iter_trigger);


    // TODO check further limited memory options
    // http://www.coin-or.org/Ipopt/documentation/node53.html#opt:hessian_approximation
}

OptimizerIpOpt::OptimizerIpOpt() {}

int OptimizerIpOpt::optimize(OptimizationProblem *problem) {

    ApplicationReturnStatus status;

    pthread_mutex_lock(&ipoptMutex);
    { // ensure all IpOpt objects are destroyed before mutex is unlocked
        SmartPtr<TNLP> mynlp =
            new LocalOptimizationIpoptTNLP(problem, &ipoptMutex);
        SmartPtr<IpoptApplication> app = IpoptApplicationFactory();

        setIpOptOptions(app, problem);

        status = app->Initialize();
        status = app->OptimizeTNLP(mynlp);
    }
    pthread_mutex_unlock(&ipoptMutex);

    return (int)status < Maximum_Iterations_Exceeded;
}
