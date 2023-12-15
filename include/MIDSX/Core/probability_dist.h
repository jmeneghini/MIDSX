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

    /**
     * @brief Class which represents a uniform distribution.
     */
    class Uniform {
    public:
        Uniform() = default;
        /**
         * @brief Constructor for the Uniform class.
         *
         * @param min The minimum value of the uniform distribution.
         * @param max The maximum value of the uniform distribution.
         */
        Uniform(double min, double max) : dist_(min, max) {}

        /**
         * @brief Returns a sample from the uniform distribution.
         *
         * @return A sample from the uniform distribution.
         */
        double sample() const {
            return dist_(generator_);
        }

        /**
         * @brief Sets the range of the uniform distribution.
         *
         * @param min The minimum value of the uniform distribution.
         * @param max The maximum value of the uniform distribution.
         */
        void setRange(double min, double max) {
            dist_ = std::uniform_real_distribution<double>(min, max);
        }
    private:
        mutable std::uniform_real_distribution<double> dist_;
        static thread_local std::mt19937 generator_;
    };


    /**
     * @brief Class which uses inversion sampling to sample from a discrete distribution.
     */
    class DiscreteInversion {
    public:
        /**
         * @brief Constructor for the DiscreteInversion class.
         *
         * @param probabilities_matrix A 2 column matrix containing the values and probabilities of the discrete distribution.
         */
        explicit DiscreteInversion(const Eigen::Matrix<double, Eigen::Dynamic, 2> &probabilities_matrix);

        /**
         * @brief Returns a sample from the discrete distribution.
         *
         * @return A sample from the discrete distribution.
         */
        double sample() const;

        /**
         * @brief Returns the expectation value of the discrete distribution.
         *
         * @return The expectation value of the discrete distribution.
         */
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

    /**
     * @brief Class which uses PENELOPE's RITA inversion sampling algorithm to sample from a continuous distribution.
     */
    class ContinuousInversion {
    public:
        ContinuousInversion() = default;

        /**
         * @brief Constructor for the ContinuousInversion class.
         *
         * @param PDF The probability density function to sample from.
         * @param energies The energies at which to generate the CDF.
         * @param x_min The minimum value of the distribution.
         * @param x_max The maximum value of the distribution.
         * @param err_thresh The maximum error allowed for the between the interpolated PDF and the actual PDF.
         */
        explicit ContinuousInversion(std::function<double(double, double)> &PDF, Eigen::VectorXd &energies, double x_min, double x_max, double err_thresh = 1E-4);

        /**
         * @brief Returns a sample from the continuous distribution.
         *
         * @param E The energy at which to sample from the distribution.
         * @return A sample from the continuous distribution.
         */
        double sample(double E) const;

    private:
        std::function<double(double, double)> PDF_;
        std::function<double(double, double)> normalized_PDF_;
        std::vector<Eigen::Array<double, Eigen::Dynamic, 2>> CDF_;
        Eigen::VectorXd energies_;
        std::vector<Eigen::Array<double, Eigen::Dynamic, 2>> interp_parameters_;
        double x_min_;
        double x_max_;
        double err_thresh_;
        ProbabilityDist::Uniform uniform_dist_;

        void normalizePDF(double E);

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
