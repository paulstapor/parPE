#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "hdf5Misc.h"
#include "optimizationResultWriter.h"

// clang-format off
TEST_GROUP(optimizationResultWriter){
    void setup() {
        parpe::initHDF5Mutex();
    }

    void teardown() {
    }
};
// clang-format on

TEST(optimizationResultWriter, testResultWriter) {
    parpe::OptimizationResultWriter w("deleteme.h5", true);

    w.setRootPath("/bla/");

    w.saveTotalCpuTime(100);

    w.flushResultWriter();

    w.logLocalOptimizerIteration(1, NULL, 0, 0, NULL, 1, 2, 3, 4, 6, 7, 8, 9,
                                 10, 11);

    w.logLocalOptimizerObjectiveFunctionEvaluation(NULL, 0, 1, NULL, 2, 3);

    w.saveLocalOptimizerResults(1, NULL, 0, 12, 0);

    w.flushResultWriter();
}
