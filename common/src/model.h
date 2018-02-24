#ifndef PARPE_COMMON_MODEL_H
#define PARPE_COMMON_MODEL_H

#include <functions.h>

#include <vector>
#include <memory>

namespace parpe {

/**
 * The Model class represent a model with any outputs = f(parameters, features)
 *
 * TODO output is currently 1-dimensional; should change
 */
template <typename X>
class Model {
public:
    virtual ~Model() = default;

    virtual void evaluate(double const* parameters,
                          std::vector<X> const& features,
                          std::vector<double>& outputs) const;

    virtual void evaluate(double const* parameters,
                          std::vector<X> const& features,
                          std::vector<double>& outputs, // here only one output per model!
                          std::vector<std::vector<double>>& outputGradients) const = 0;

};


/**
 * @brief The LinearModel class represents a linear model y = Ax + b with feature matrix A and parameters [x, b].
 */

class LinearModel : public Model<std::vector<double>>
{
public:
    LinearModel() = default;

    // From Model:
    using Model::evaluate;

    /**
     * @brief Evaluate linear model with the given parameter on the given dataset
     * @param parameters
     * @param features
     * @param outputs
     * @param outputGradients
     */
    void evaluate(double const* parameters,
                  std::vector<std::vector<double>> const& features,
                  std::vector<double>& outputs, // here only one output per model!
                  std::vector<std::vector<double>>& outputGradients) const override;

};

/**
 * @brief The LinearModelMSE class is a wrapper around LinearModel implementing the
 * mean squared error loss function.
 */
class LinearModelMSE : public SummedGradientFunction<int>
{
public:
    LinearModelMSE(int numParameters)
        :numParameters_(numParameters) {}

    // SummedGradientFunction
    FunctionEvaluationStatus evaluate(
            const double* const parameters,
            int dataset,
            double &fval,
            double* gradient) const override {
        std::vector<int> dsets {dataset};
        return evaluate(parameters, dsets , fval, gradient);
    }

    FunctionEvaluationStatus evaluate(
            const double* const parameters,
            std::vector<int> datasets,
            double &fval,
            double* gradient) const override;

    int numParameters() const {return numParameters_;}


    int numParameters_ = 0;
    std::vector<std::vector<double>> datasets;
    std::vector<double> labels;
    LinearModel lm;
};

}
#endif