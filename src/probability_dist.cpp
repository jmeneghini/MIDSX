#include "probability_dist.h"

ProbabilityDist::Discrete::Discrete(const Eigen::Matrix<double, Eigen::Dynamic, 2> &probabilities_matrix,
                                    std::string sampling_algorithm)
        : probabilities_matrix_(probabilities_matrix), sampling_algorithm_(std::move(sampling_algorithm)),
          generator_(std::random_device{}()) {
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
    // sample from uniform distribution
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double sample = dist(generator_);

    // find index of sampled value
    auto lower = std::lower_bound(cdf_matrix_.col(1).data(), cdf_matrix_.col(1).data() + cdf_matrix_.rows(),
                                  sample);
    int index = std::distance(cdf_matrix_.col(1).data(), lower);

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

Eigen::VectorXd ProbabilityDist::Discrete::cumsum(const Eigen::VectorXd &vector) {
    Eigen::VectorXd cumsum_vector(vector.rows());
    cumsum_vector(0) = vector(0);
    for (int i = 1; i < vector.rows(); ++i) {
        cumsum_vector(i) = cumsum_vector(i - 1) + vector(i);
    }

    return cumsum_vector;
}

Eigen::Matrix<double, Eigen::Dynamic, 2> ProbabilityDist::Discrete::generateCDF() const {
    Eigen::VectorXd probabilities = probabilities_matrix_.col(1);
    Eigen::VectorXd cdf = cumsum(probabilities);

    Eigen::Matrix<double, Eigen::Dynamic, 2> cdf_matrix(cdf.rows(), 2);
    cdf_matrix.col(0) = probabilities_matrix_.col(0);
    cdf_matrix.col(1) = cdf;

    return cdf_matrix;
}
