#ifndef PARPE_AMICI_SIMULATION_RESULT_WRITER_H
#define PARPE_AMICI_SIMULATION_RESULT_WRITER_H

#include <string>
#include <amici.h>
#include <H5Cpp.h>

namespace parpe {

class SimulationResultWriter {
public:

    SimulationResultWriter() = default;

    SimulationResultWriter(std::string hdf5FileName, std::string rootPath);

    /**
     * @brief Create results datasets. Condition index is first dimension.
     * @param udata
     * @param edata
     * @param numSimulations
     */

    void createDatasets(const amici::Model &model,
                        const amici::ExpData *edata,
                        int numberOfSimulations = 1);

    /**
     * @brief Save results for a single simulation to HDF5 file.
     * @param udata
     * @param edata
     * @param rdata
     * @param simulationIdx If >= 0: write results in the selected
     * position of the result data sets (-> createDatasets)
     */

    void saveSimulationResults(const amici::ExpData *edata,
                               const amici::ReturnData *rdata,
                               int simulationIdx);

    H5::H5File reopenFile();

    bool saveXDot = false;
    bool saveLlh = false;
//    bool saveSllh = false;
    bool saveYSim = false;
    bool saveYMes = false;
//    bool saveP = false;
//    bool saveK = false;

    std::string yMesPath;
    std::string ySimPath;
    std::string xDotPath;
    std::string llhPath;

private:
    void updatePaths();

    std::string rootPath;
    H5::H5File file;
};

} // namespace parpe

#endif
