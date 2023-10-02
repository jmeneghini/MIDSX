#ifndef PROBABILITY_DIST_H
#define PROBABILITY_DIST_H

#include <random>
#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <Eigen/Core>
#include <unsupported/Eigen/CXX11/Tensor>

namespace ProbabilityDistHelpers {
    int findIndexOfNextSmallestValue(double x, const Eigen::VectorXd &vector);
}

namespace ProbabilityDist {

    class Uniform {
    public:
        Uniform() = default;
        Uniform(double min, double max) : dist_(min, max) {}

        double sample() const {
            return dist_(generator_);
        }

        void setRange(double min, double max) {
            dist_ = std::uniform_real_distribution<double>(min, max);
        }
    private:
        mutable std::uniform_real_distribution<double> dist_;
        static thread_local std::mt19937 generator_;
    };


    class DiscreteInversion {
    public:
        explicit DiscreteInversion(const Eigen::Matrix<double, Eigen::Dynamic, 2> &probabilities_matrix);

        double sample() const;

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

    };

    struct IntervalData {
        double x_i;
        double x_i_1;
        double y_i;
        double y_i_1;
        double a_i;
        double b_i;
    };

    class ContinuousInversion {
    public:
        ContinuousInversion() = default;
        explicit ContinuousInversion(std::function<double(double, double)> &PDF, Eigen::VectorXd &energies, double x_min, double x_max);

        double sample(double E) const;

    private:
        std::function<double(double, double)> PDF_;
        std::vector<Eigen::Array<double, Eigen::Dynamic, 2>> CDF_;
        Eigen::VectorXd energies_;
        std::vector<Eigen::Array<double, Eigen::Dynamic, 2>> interp_parameters_;
        double x_min_;
        double x_max_;
        ProbabilityDist::Uniform uniform_dist_;

        void normalizePDF();

        double getXFromY(int energy_index, double y) const;

        void initializeCDFAndInterpolationParameters();

        Eigen::Array<double, Eigen::Dynamic, 2> calculateInterpolationParametersPerEnergy(
                double E, Eigen::Array<double, Eigen::Dynamic, 2> CDF_RITA_PER_ENERGY);

        Eigen::Array<double, Eigen::Dynamic, 2> getMinimizedErrorCDFPerEnergy(double E, double err_thresh);

        Eigen::Array<double, Eigen::Dynamic, 2> generateCDFPerEnergy(double E, Eigen::VectorXd x_grid);

        double getInterpErrorOverInterval(double E, IntervalData id) const;

        double getPDFFromCDFOverInterval(double E, double x, IntervalData id) const;

        double getEta(double x, IntervalData id) const;
    };
}
#endif
