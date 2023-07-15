#ifndef PROBABILITY_DIST_H
#define PROBABILITY_DIST_H

#include <random>
#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <Eigen/Core>

class ProbabilityDistribution {
public:
    virtual double sample() const = 0;
};

namespace ProbabilityDist {

    class Uniform : public ProbabilityDistribution {
    public:
        Uniform(double min, double max) : generator_(std::random_device{}()), dist_(min, max) {}

        double sample() const override {
            return dist_(generator_);
        }

    private:
        mutable std::uniform_real_distribution<double> dist_;
        mutable std::mt19937 generator_;
    };

    class Discrete : public ProbabilityDistribution {
    public:
        explicit Discrete(const Eigen::Matrix<double, Eigen::Dynamic, 2> &probabilities_matrix,
                              std::string sampling_algorithm = "inversion");

        double sample() const override;

        double sampleInversion() const;

        double getExpectationValue() const;

    private:
        Eigen::Matrix<double, Eigen::Dynamic, 2> probabilities_matrix_;
        Eigen::Matrix<double, Eigen::Dynamic, 2> cdf_matrix_;
        std::string sampling_algorithm_;
        ProbabilityDist::Uniform uniform_dist_;

        // Checks if discrete distribution is normalized. If not, normalizes it.
        void normalize();

        // Performs cumulative sum on an Eigen vector
        static Eigen::VectorXd cumsum(const Eigen::VectorXd &vector);

        // generate cdf for sampling
        Eigen::Matrix<double, Eigen::Dynamic, 2> generateCDF() const;

        int findIndexOfNextSmallestValue(double x) const;
    };
}
#endif
