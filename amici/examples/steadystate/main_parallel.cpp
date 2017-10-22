#include "hdf5Misc.h"
#include "steadystateProblemParallel.h"
#include <LoadBalancerMaster.h>
#include <LoadBalancerWorker.h>
#include <logging.h>
#include <mpi.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>

/** @file
 *
 * This example demonstrates the use of the loadbalancer / queue for parallel
 * ODE simulation.
 */

void initMPI(int *argc, char ***argv);

int main(int argc, char **argv) {
    int status = 0;

    initMPI(&argc, &argv);
    parpe::initHDF5Mutex();

    int commSize;
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);

    if (commSize == 1) {
        // run in serial mode
        SteadystateProblemParallel problem {NULL};
        status = getLocalOptimum(&problem);

    } else {
        // run in parallel
        SteadystateProblemParallel problem {NULL};

        int mpiRank;
        MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);

        if (mpiRank == 0) {
            parpe::LoadBalancerMaster lbm;
            problem.loadBalancer = &lbm;
            lbm.run();

            status = parpe::getLocalOptimum(&problem);

            lbm.terminate();
            lbm.sendTerminationSignalToAllWorkers();
        } else {
            problem.run();
        }
    }

    parpe::destroyHDF5Mutex();

    MPI_Finalize();

    return status;
}

void initMPI(int *argc, char ***argv) {
    int mpiErr = MPI_Init(argc, argv);
    if (mpiErr != MPI_SUCCESS) {
        parpe::logmessage(parpe::LOGLVL_CRITICAL, "Problem initializing MPI. Exiting.");
        exit(1);
    }

    int mpiRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    if (mpiRank == 0) {
        int commSize;
        MPI_Comm_size(MPI_COMM_WORLD, &commSize);

        parpe::logmessage(parpe::LOGLVL_INFO, "Running with %d MPI processes.", commSize);
    }
}
