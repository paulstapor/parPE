#include <simulationResultWriter.h>
#include <hdf5Misc.h>
#include <udata.h>
#include <rdata.h>
#include <edata.h>
#include <amici_model.h>
#include <cstdio>

namespace parpe {

SimulationResultWriter::SimulationResultWriter(std::string hdf5FileName, std::string rootPath)
    : rootPath(rootPath)
{
    try {
        // crashes on exit H5::Exception::dontPrint();
        file = H5::H5File(hdf5FileName, H5F_ACC_RDWR);
        // TODO reset dontPrint
    } catch (H5::FileIException e) {
        // create if doesn't exist
        file = H5::H5File(hdf5FileName, H5F_ACC_EXCL);
    }

    updatePaths();
}


void SimulationResultWriter::createDatasets(const amici::Model &model, const amici::UserData *udata,
                                            const amici::ExpData *edata,
                                            int numberOfSimulations)
{
    hsize_t numSimulations = static_cast<hsize_t>(numberOfSimulations);
    hsize_t ny = static_cast<hsize_t>(model.nytrue);
    hsize_t nx = static_cast<hsize_t>(model.nxtrue);
    hsize_t nt = static_cast<hsize_t>(udata->nt);

    auto lock = parpe::hdf5MutexGetLock();


    double fillValueDbl = NAN;   /* Fill value for the dataset */
    H5::DSetCreatPropList propList;
    propList.setFillValue(H5::PredType::NATIVE_DOUBLE, &fillValueDbl);

    parpe::hdf5EnsureGroupExists(file.getId(), rootPath.c_str());

    if((saveYMes || saveYSim) && edata->nt > 0 && edata->nytrue > 0) {
        // observables
        constexpr int rank = 3;
        hsize_t dims[rank] = {numSimulations, nt, ny};
        H5::DataSpace dataspace(rank, dims);
        hsize_t chunkDims[rank] = {1, dims[1], dims[2]};
        propList.setChunk(rank, chunkDims);

        if(saveYMes)
            file.createDataSet(yMesPath, H5::PredType::NATIVE_DOUBLE, dataspace, propList);
        if(saveYSim)
            file.createDataSet(ySimPath, H5::PredType::NATIVE_DOUBLE, dataspace, propList);
    }

    if(saveXDot) {
        hsize_t dims[] = {numSimulations, nx};
        H5::DataSpace dataspace(2, dims);
        hsize_t chunkDims[] = {1, dims[1]};
        propList.setChunk(2, chunkDims);
        file.createDataSet(xDotPath, H5::PredType::NATIVE_DOUBLE, dataspace, propList);
    }

    if(saveLlh) {
        hsize_t dims[] = {numSimulations};
        H5::DataSpace dataspace(1, dims);
        hsize_t one = 1;
        propList.setChunk(1, &one);
        file.createDataSet(llhPath, H5::PredType::NATIVE_DOUBLE, dataspace, propList);
    }

    file.flush(H5F_SCOPE_LOCAL);
}


void SimulationResultWriter::saveSimulationResults(const amici::ExpData *edata, const amici::ReturnData *rdata, int simulationIndex)
{
    hsize_t simulationIdx = static_cast<hsize_t>(simulationIndex);
    hsize_t ny = static_cast<hsize_t>(rdata->ny);
    hsize_t nx = static_cast<hsize_t>(rdata->nx);
    hsize_t nt = static_cast<hsize_t>(rdata->nt);

    auto lock = parpe::hdf5MutexGetLock();

    if(saveYMes && edata->nt > 0 && edata->nytrue > 0) {
        auto dataset = file.openDataSet(yMesPath);

        auto filespace = dataset.getSpace();
        hsize_t count[] = {1, nt, ny};
        hsize_t start[] = {simulationIdx, 0, 0};
        filespace.selectHyperslab(H5S_SELECT_SET, count, start);

        auto memspace = dataset.getSpace();
        hsize_t start2[] = {0, 0, 0};
        memspace.selectHyperslab(H5S_SELECT_SET, count, start2);

        dataset.write(edata->my, H5::PredType::NATIVE_DOUBLE, memspace, filespace);
    }

    if(saveYSim && edata->nt > 0 && edata->nytrue > 0) {
        auto dataset = file.openDataSet(ySimPath);

        auto filespace = dataset.getSpace();
        hsize_t count[] = {1, nt, ny};
        hsize_t start[] = {simulationIdx, 0, 0};
        filespace.selectHyperslab(H5S_SELECT_SET, count, start);

        auto memspace = dataset.getSpace();
        hsize_t start2[] = {0, 0, 0};
        memspace.selectHyperslab(H5S_SELECT_SET, count, start2);

        dataset.write(rdata->y, H5::PredType::NATIVE_DOUBLE, memspace, filespace);
    }

    if(saveXDot) {
        auto dataset = file.openDataSet(xDotPath);

        auto filespace = dataset.getSpace();
        hsize_t count[] = {1, nx};
        hsize_t start[] = {simulationIdx, 0};
        filespace.selectHyperslab(H5S_SELECT_SET, count, start);

        auto memspace = dataset.getSpace();
        hsize_t start2[] = {0, 0};
        memspace.selectHyperslab(H5S_SELECT_SET, count, start2);

        dataset.write(rdata->xdot, H5::PredType::NATIVE_DOUBLE, memspace, filespace);
    }

    if(saveLlh) {
        auto dataset = file.openDataSet(llhPath);

        auto filespace = dataset.getSpace();
        hsize_t count[] = {1};
        hsize_t start[] = {simulationIdx};
        filespace.selectHyperslab(H5S_SELECT_SET, count, start);

        auto memspace = dataset.getSpace();
        hsize_t start2[] = {0};
        memspace.selectHyperslab(H5S_SELECT_SET, count, start2);

        dataset.write(rdata->llh, H5::PredType::NATIVE_DOUBLE, memspace, filespace);
    }

    file.flush(H5F_SCOPE_LOCAL);
}

H5::H5File SimulationResultWriter::reopenFile()
{
    return H5::H5File(file.getId());
}

void SimulationResultWriter::updatePaths()
{
    yMesPath = rootPath + "/yMes";
    ySimPath = rootPath + "/ySim";
    xDotPath = rootPath + "/xDot";
    llhPath  = rootPath + "/llh";

}



} // namespace parpe