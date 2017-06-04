#ifndef LOCAL_OPTIMIZATION_H
#define LOCAL_OPTIMIZATION_H

#include "optimizationProblem.h"

/**
 * @brief getLocalOptimum Get local optimum using Ipopt Optimizer
 * @param dataPath
 * @return Returns 0 on success.
 */
int getLocalOptimumIpopt(OptimizationProblem *problem);

#endif