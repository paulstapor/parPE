#ifndef OPTIMIZATIONOPTIONS_H
#define OPTIMIZATIONOPTIONS_H

#include <hdf5Misc.h>
#include <map>
#include <string>
#include <functional>
#include <algorithm>
#include <memory>
#include <vector>

namespace parpe {

class Optimizer;

enum class optimizerName {
    OPTIMIZER_IPOPT,
    OPTIMIZER_CERES,
    OPTIMIZER_DLIB,
    OPTIMIZER_TOMS611,
    OPTIMIZER_FSQP,
    OPTIMIZER_MINIBATCH_1 = 10
};



/** Type to describe an optimization (minimization) problem */

class OptimizationOptions {
  public:
    OptimizationOptions() = default;

    /** Optimizer factory method depending on OptimizationOptions::optimizer */
    Optimizer *createOptimizer() const;

    /** Optimizer to use */
    optimizerName optimizer = optimizerName::OPTIMIZER_IPOPT;

    /** Optimizer log file */
    char *logFile = nullptr;

    /** Print progress to stdout */
    bool printToStdout = true;

    /** Maximum number of optimizer iterations*/
    int maxOptimizerIterations = 100;

    static std::unique_ptr<OptimizationOptions> fromHDF5(const char *fileName);
    static std::unique_ptr<OptimizationOptions> fromHDF5(hid_t fileId, const std::string &path = "/optimizationOptions");

    static std::vector<double> getStartingPoint(hid_t fileId, int index);

    /** Number of starts for local optimization (only used for multi-start
     * optimization */
    int numStarts = 1;

    /** Retry optimization in case of infeasibility (only used for multi-start
     * optimization */
    int retryOptimization = false;

    /** use hierarchical optimization if respective configuration is available
      * see hierarchicalOptimization.cpp */
    int hierarchicalOptimization = true;

    int multistartsInParallel = true;

    std::string toString();

    int getIntOption(const std::string &key);
    double getDoubleOption(const std::string &key);
    std::string getStringOption(const std::string& key);

    void setOption(const std::string &key, int value);
    void setOption(const std::string &key, double value);
    void setOption(const std::string& key, std::string value);

    template <typename T>
    void for_each(std::function< void (const std::pair<const std::string, const std::string>, T)> f, T arg) const
    {
        std::for_each(options.cbegin(), options.cend(),
                      std::bind(f, std::placeholders::_1, arg));
    }

private:
    std::map<std::string, std::string> options;
};

Optimizer* optimizerFactory(optimizerName optimizer);

} // namespace parpe

#endif // OPTIMIZATIONOPTIONS_H
