#include "probability_dist.h"

// ensures rng is thread safe
thread_local std::mt19937 ProbabilityDist::Uniform::generator_(std::random_device{}());

ProbabilityDist::Discrete::Discrete(const Eigen::Matrix<double, Eigen::Dynamic, 2> &probabilities_matrix,
                                    std::string sampling_algorithm)
        : probabilities_matrix_(probabilities_matrix), sampling_algorithm_(std::move(sampling_algorithm)),
        uniform_dist_(Uniform(0.0, 1.0)) {
    normalize(); // normalize probabilities if necessary
    cdf_matrix_ = generateCDF();
}

double ProbabilityDist::Discrete::sample() const {
    if (sampling_algorithm_ == "inversion") {
        return sampleInversion();
    }
    throw std::invalid_argument("Unrecognized sampling algorithm: " + sampling_algorithm_);
}

double ProbabilityDist::Discrete::sampleInversion() const {
    double sample = uniform_dist_.sample();
    int index = findIndexOfNextSmallestValue(sample);
    return cdf_matrix_(index, 0);
}

double ProbabilityDist::Discrete::getExpectationValue() const {
    return probabilities_matrix_.col(0).dot(probabilities_matrix_.col(1));
}

void ProbabilityDist::Discrete::normalize() {
    double sum = probabilities_matrix_.col(1).sum();
    if (sum != 1.0) {
        probabilities_matrix_.col(1) /= sum;
    }
}

Eigen::Matrix<double, Eigen::Dynamic, 2> ProbabilityDist::Discrete::generateCDF() const {
    Eigen::VectorXd probabilities = probabilities_matrix_.col(1);
    Eigen::VectorXd cdf = cumsum(probabilities);

    Eigen::Matrix<double, Eigen::Dynamic, 2> cdf_matrix(cdf.rows(), 2);
    cdf_matrix.col(0) = probabilities_matrix_.col(0);
    cdf_matrix.col(1) = cdf;

    return cdf_matrix;
}

int ProbabilityDist::Discrete::findIndexOfNextSmallestValue(double x) const {
    auto lower_bound_it = std::lower_bound(cdf_matrix_.col(1).data(), cdf_matrix_.col(1).data() + cdf_matrix_.rows(),
                                           x);
    return lower_bound_it - cdf_matrix_.col(1).data();
}

Eigen::VectorXd ProbabilityDist::Discrete::cumsum(const Eigen::VectorXd &vector) {
    Eigen::VectorXd cumsum_vector(vector.rows());
    cumsum_vector(0) = vector(0);
    for (int i = 1; i < vector.rows(); ++i) {
        cumsum_vector(i) = cumsum_vector(i - 1) + vector(i);
    }

    return cumsum_vector;
}