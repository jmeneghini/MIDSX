#include "Core/photon_interactions.h"

Eigen::MatrixXd PhotonInteractionHelpers::mergeMatrices(std::vector<Eigen::MatrixXd>& matrices) {
    std::vector<double> merged;

    for (const auto& matrix : matrices) {
        std::vector<double> v(matrix.data(), matrix.data() + matrix.size());
        merged.insert(merged.end(), v.begin(), v.end());
    }

    std::sort(merged.begin(), merged.end());

    auto last = std::unique(merged.begin(), merged.end());
    merged.erase(last, merged.end());

    Eigen::MatrixXd result = Eigen::Map<Eigen::MatrixXd>(merged.data(), merged.size(), 1);

    return result;
}

Eigen::MatrixXd PhotonInteractionHelpers::getBlockByRowValue(Eigen::MatrixXd& matrix, double rowStartValue, double rowEndValue, int column) {
    int rowStart = -1, rowEnd = -1;

    for (int i = 0; i < matrix.rows(); ++i) {
        double value = matrix(i, column);

        if (value >= rowStartValue && value <= rowEndValue) {
            if (rowStart == -1 || i < rowStart) rowStart = i;
            if (rowEnd == -1 || i > rowEnd) rowEnd = i;
        }
    }

    if (rowStart == -1 || rowEnd == -1) {
        throw std::invalid_argument("getBlockByRowValue: Value range does not match any matrix elements");
    }

    int numRows = rowEnd - rowStart + 1;
    int numCols = matrix.cols();
    Eigen::MatrixXd block = matrix.block(rowStart, 0, numRows, numCols);
    return block;
}

double PhotoelectricEffect::interact(Particle& photon, Material& material) {
    photon.terminate();
    return photon.getEnergy();
    // A decent approximation for the photoelectric effect is to terminate the photon and return its energy,
    // neglecting lower energy secondary particles
}

double CoherentScattering::interact(Particle& photon, Material& material) {

    double mu = material.getData().sampleCoherentScatteringDCS(photon.getEnergy());

    double theta = acos(mu);
    // sample phi (0, 2pi)
    double phi = 2*PI*uniform_dist_.sample();
    // rotate photon direction
    photon.rotate(theta, phi);
    return 0;
}

double IncoherentScattering::interact(Particle& photon, Material& material) {
    // Kahn's rejection method
    double k = photon.getEnergy() / ELECTRON_REST_MASS; // unitless
    double chi, mu, k_prime;

    while (true) {
        double random_number_1 = uniform_dist_.sample();
        double random_number_2 = uniform_dist_.sample();
        double random_number_3 = uniform_dist_.sample();

        if (isR1Accepted(k, random_number_1)) {
            chi = getChiIfR1Accepted(k, random_number_2);
            if (!isR3AcceptedIfR1Accepted(chi, random_number_3)) continue;
        } else {
            chi = getChiIfR1Rejected(k, random_number_2);
            if (!isR3AcceptedIfR1Rejected(k, chi, random_number_3)) continue;
        }

        // accept/reject with scattering function

        k_prime = getKPrime(k, chi);
        mu = getMu(k, k_prime);
        if (isAcceptedByScatteringFunction(mu, k, material)) {
            return changeTrajectoryAndReturnEnergyForCoherentScattering(photon, mu, k, k_prime);
        }
    }
}

bool IncoherentScattering::isR1Accepted(double k, double random_number_1) {
    double R1_acceptance_prob = (1 + 2*k)/(9 + 2*k);
    return random_number_1 <= R1_acceptance_prob;
}

double IncoherentScattering::getChiIfR1Accepted(double k, double random_number_2) {
    double chi = 1 + 2*k*random_number_2;
    return chi;
}

bool IncoherentScattering::isR3AcceptedIfR1Accepted(double chi, double random_number_3) {
    double R3_acceptance_prob = 4 * (1/chi - 1/(chi*chi));
    return random_number_3 <= R3_acceptance_prob;
}

double IncoherentScattering::getKPrime(double k, double chi) {
    double k_prime = k/chi;
    return k_prime;
}

double IncoherentScattering::getMu(double k, double k_prime) {
    double mu = 1 + 1/k - 1/k_prime;
    return mu;
}

bool IncoherentScattering::isAcceptedByScatteringFunction(double mu, double k, Material& material) {
    double x = ALPHA*k*sqrt(1-mu);
    double max_x = ALPHA*k*sqrt(2);
    // S(x, Z)/S(max_x, Z)
    double accept_prob =  material.getData().interpolateIncoherentScatteringFunction(x)/material.getData().interpolateIncoherentScatteringFunction(max_x);
    double random_number = uniform_dist_.sample();
    return random_number <= accept_prob;
}

double IncoherentScattering::getChiIfR1Rejected(double k, double random_number_2) {
    double chi = (1 + 2*k)/(1 + 2*k*random_number_2);
    return chi;
}

bool IncoherentScattering::isR3AcceptedIfR1Rejected(double k, double chi, double random_number_3) {
    double R3_acceptance_prob = pow(1/k - chi/k + 1, 2) + 1/chi;
    return 2*random_number_3 <= R3_acceptance_prob;
}

double IncoherentScattering::changeTrajectoryAndReturnEnergyForCoherentScattering(Particle& photon, double mu, double k, double k_prime) {
    double theta = acos(mu);
    double phi = 2*PI*uniform_dist_.sample();
    photon.rotate(theta, phi);

    photon.setEnergy(k_prime*ELECTRON_REST_MASS);
    return ELECTRON_REST_MASS * (k - k_prime);
}

