#ifndef PROBABILITY_DIST_H
#define PROBABILITY_DIST_H

#include <random>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

class ProbabilityDist {
public:
    virtual double sample() const = 0;
};

class UniformDist : public ProbabilityDist {
public:
    UniformDist(double min, double max) : min_(min), max_(max), generator_(std::random_device{}()) {}

    double sample() const override {
        std::uniform_real_distribution<double> dist(min_, max_);
        return dist(generator_);
    }

private:
    double min_;
    double max_;
    mutable std::mt19937 generator_;
};

class DiscreteDist : public ProbabilityDist {
public:
    explicit DiscreteDist(const Eigen::Matrix<double, Eigen::Dynamic, 2>& probabilities_matrix, const std::string& sampling_algorithm = "inversion")
            : probabilities_matrix_(probabilities_matrix), sampling_algorithm_(sampling_algorithm), generator_(std::random_device{}()) {
        normalize(); // normalize probabilities if necessary
        cdf_matrix_ = generateCDF();
    }

    double sample() const override {
        if (sampling_algorithm_ == "inversion") {
            return sampleInversion();
        }

        throw std::invalid_argument("Unrecognized sampling algorithm: " + sampling_algorithm_);
    }

    double sampleInversion() const {
        // sample from uniform distribution
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        double sample = dist(generator_);

        // find index of sampled value
        auto lower = std::lower_bound(cdf_matrix_.col(1).data(), cdf_matrix_.col(1).data() + cdf_matrix_.rows(), sample);
        int index = std::distance(cdf_matrix_.col(1).data(), lower);

        return cdf_matrix_(index, 0);
    }

    double getExpectationValue() const {
        double expectation_value = 0;
        for (int i = 0; i < probabilities_matrix_.rows(); ++i) {
            expectation_value += probabilities_matrix_(i, 0) * probabilities_matrix_(i, 1);
        }
        return expectation_value;
    }

private:
    Eigen::Matrix<double, Eigen::Dynamic, 2> probabilities_matrix_;
    Eigen::Matrix<double, Eigen::Dynamic, 2> cdf_matrix_;
    std::string sampling_algorithm_;
    mutable std::mt19937 generator_;

    // Checks if discrete distribution is normalized. If not, normalizes it.
    void normalize() {
        double sum = probabilities_matrix_.col(1).sum();
        if (sum != 1.0) {
            probabilities_matrix_.col(1) /= sum;
        }
    }

    // Performs cumulative sum on an Eigen vector
    static Eigen::VectorXd cumsum(const Eigen::VectorXd& vector) {
        Eigen::VectorXd cumsum_vector(vector.rows());
        cumsum_vector(0) = vector(0);
        for (int i = 1; i < vector.rows(); ++i) {
            cumsum_vector(i) = cumsum_vector(i - 1) + vector(i);
        }

        return cumsum_vector;
    }
    
    // generate cdf for sampling
    Eigen::Matrix<double, Eigen::Dynamic, 2> generateCDF() const {
        Eigen::Matrix<double, Eigen::Dynamic, 2> cdf(probabilities_matrix_.rows(), probabilities_matrix_.cols());
        cdf.col(0) = probabilities_matrix_.col(0);
        cdf.col(1) = cumsum(probabilities_matrix_.col(1));
        return cdf;
    }


};

#endif
