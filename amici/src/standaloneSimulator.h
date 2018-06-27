#ifndef STANDALONESIMULATOR_H
#define STANDALONESIMULATOR_H

#include "multiConditionProblem.h"

#include <amici/amici.h>

namespace parpe {

/**
 * @brief The StandaloneSimulator class is for running simulations for a given dataset
 * and given parameters after optimization in parallel or in sequential mode and saving the simulation results.
 *
 * Command line interface should support:
 * ./simulate --at-optimum : use parameters from last iteration of all multi-start optimization runs
 *            --parameter-matrix : using arbitrary parameters from some matrix in hdf5 file
 *            --along-trajectory : use parameters along the optimization trajectory of all multi-start optimization runs
 */
class StandaloneSimulator
{
public:
    StandaloneSimulator(MultiConditionDataProvider *dp);

    /**
     * @brief Run simulations for the given parameter and write results to an HDF5 file at the given location
     * @param resultFile Name of HDF5 output file. Will be created or appended.
     * @param resultPath HDF5 file root group name
     * @param optimizationParameters Parameters for simulation (results from hierarchical or standard optimization
     * @param loadBalancer LoadBalander instance for distributed memory parallel, or nullptr for shared memory parallel or sequential
     * @param inputFile File with simulation options and data used for optimization
     * @return Number of errors encountered
     */
    int run(const std::string &resultFile,
            const std::string &resultPath,
            std::vector<double> const& optimizationParameters,
            LoadBalancerMaster *loadBalancer,
            const H5::H5File &inputFile);

    void messageHandler(std::vector<char> &buffer, int jobId);

private:

    SimulationRunnerSimple::AmiciResultPackageSimple runSimulation(JobIdentifier path, amici::Solver &solver, amici::Model &model);

    MultiConditionDataProvider *dataProvider = nullptr;

    const int maxSimulationsPerPackage = 8;
};


// enum class SimulatorOpType {finalParameters};

std::pair<int, double> getFunctionEvaluationWithMinimalCost(std::string const& datasetPath, H5::H5File const& file);

/**
 * @brief Read the final parameter set from parPE result file for the given local optimization index
 * @param startIndex
 * @param file
 * @return The final parameter vector
 */
std::vector<double> getFinalParameters(const std::string &startIndex, const H5::H5File &file);

std::vector<std::vector<double>> getParameterTrajectory(const std::string &startIndex, H5::H5File const& file);

int getNumStarts(const H5::H5File &file, const std::string &rootPath = "/");

int runFinalParameters(parpe::StandaloneSimulator &sim,
                       const std::string &inFileName,
                       const std::string &resultFileName,
                       const std::string &resultPath,
        parpe::LoadBalancerMaster *loadBalancer);

int runAlongTrajectory(parpe::StandaloneSimulator &sim,
                       std::string const& inFileName,
                       std::string const& resultFileName,
                       std::string const& resultPath,
                       parpe::LoadBalancerMaster *loadBalancer);


int runSimulator(parpe::StandaloneSimulator &sim,
                 std::string const& simulationMode,
                 std::string const& inFileName,
                 std::string const& dataFilePath,
                 std::string const& resultFileName,
                 std::string const& resultPath,
                 parpe::LoadBalancerMaster *loadBalancer);

int runSimulator(MultiConditionDataProvider &dp,
                 std::string const& simulationMode,
                 std::string const& inFileName,
                 std::string const& dataFilePath,
                 std::string const& resultFileName,
                 std::string const& resultPath);


} // namespace parpe

#endif // STANDALONESIMULATOR_H
