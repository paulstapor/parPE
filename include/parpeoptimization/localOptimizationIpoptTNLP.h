#ifndef LOCALOPTIMIZATIONIPOPTTNLP_H
#define LOCALOPTIMIZATIONIPOPTTNLP_H

// This should fix `error "don't have header file for stddef"' with some IpOpt
// versions
#define HAVE_CSTDDEF
#include <IpIpoptApplication.hpp>
#undef HAVE_CSTDDEF

#include <cmath>
#include <memory>
#include <mutex>

#include <parpecommon/misc.h>

namespace parpe {

using namespace Ipopt;


/** Mutex for managing access to IpOpt routines which are not thread-safe */
typedef std::recursive_mutex mutexIpOptType;

/**
 * @brief ipoptMutex Ipopt seems not to be thread safe. Lock this mutex every
 * time that control is passed to ipopt functions.
 */
static mutexIpOptType mutexIpOpt {};


InverseUniqueLock<mutexIpOptType> ipOptReleaseLock();

std::unique_lock<mutexIpOptType> ipOptGetLock();


class OptimizationProblem;
class OptimizationReporter;

class LocalOptimizationIpoptTNLP : public Ipopt::TNLP {
  public:

    LocalOptimizationIpoptTNLP(OptimizationProblem &problem, OptimizationReporter &reporter);

    virtual ~LocalOptimizationIpoptTNLP() override = default;

    virtual bool get_nlp_info(Index &n, Index &m, Index &nnz_jac_g,
                              Index &nnz_h_lag,
                              IndexStyleEnum &index_style) override;

    virtual bool get_bounds_info(Index n, Number *x_l, Number *x_u, Index m,
                                 Number *g_l, Number *g_u) override;

    virtual bool get_starting_point(Index n, bool init_x, Number *x,
                                    bool init_z, Number *z_L, Number *z_U,
                                    Index m, bool init_lambda,
                                    Number *lambda) override;

    virtual bool eval_f(Index n, const Number *x, bool new_x,
                        Number &obj_value) override;

    /**
     * @brief See Ipopt::TNLP::eval_grad_f.
     *
     * Note: Failure in eval_f (i.e. returning non-finite value or false) will make IpOpt try a new
     * step, unless this happens at the starting point. However, if eval_f succeeds, but eval_grad_f
     * fails, then IpOpt will terminate.
     *
     * @param n
     * @param x
     * @param new_x
     * @param grad_f
     * @return
     */
    virtual bool eval_grad_f(Index n, const Number *x, bool new_x,
                             Number *grad_f) override;

    virtual bool eval_g(Index n, const Number *x, bool new_x, Index m,
                        Number *g) override;

    virtual bool eval_jac_g(Index n, const Number *x, bool new_x, Index m,
                            Index nele_jac, Index *iRow, Index *jCol,
                            Number *values) override;

    virtual bool intermediate_callback(
        AlgorithmMode mode, Index iter, Number obj_value, Number inf_pr,
        Number inf_du, Number mu, Number d_norm, Number regularization_size,
        Number alpha_du, Number alpha_pr, Index ls_trials,
        const IpoptData *ip_data, IpoptCalculatedQuantities *ip_cq) override;

    virtual void finalize_solution(SolverReturn status, Index n,
                                   const Number *x, const Number *z_L,
                                   const Number *z_U, Index m, const Number *g,
                                   const Number *lambda, Number obj_value,
                                   const IpoptData *ip_data,
                                   IpoptCalculatedQuantities *ip_cq) override;
private:
    OptimizationProblem &problem;
    OptimizationReporter &reporter;

    // need to store initial parameters, because IpOpt asks twice
    std::vector<double> initialParameters;

};

void setIpOptOption(const std::pair<const std::string, const std::string> &pair, SmartPtr<OptionsList>* o);

} // namespace parpe

#endif // LOCALOPTIMIZATIONIPOPTTNLP_H
