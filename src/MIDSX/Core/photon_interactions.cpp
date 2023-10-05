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
    double k = photon.getEnergy()/ELECTRON_REST_MASS; // unitless

    double rejection_constant = getRejectionConstant(k);

    double x_max = ALPHA*k*sqrt(2);
    double SFMax = material.getData().interpolateIncoherentScatteringFunction(x_max);

    while (true) {
        double random_number_1 = uniform_dist_.sample();
        double x = getX(k, random_number_1);
        double beta = x*x/(ALPHA*ALPHA*k);
        double mu = 1 - pow(x/(ALPHA*k), 2);
        double rejection_prob = getRejectionProb(mu, beta);
        double random_number_2 = uniform_dist_.sample();
        double SF_value = material.getData().interpolateIncoherentScatteringFunction(x);
        if ((SF_value * rejection_prob) >= (random_number_2 * rejection_constant * SFMax)) {
            double theta = acos(mu);
            double phi = 2*PI*uniform_dist_.sample();
            photon.rotate(theta, phi);
            double initial_energy = photon.getEnergy();
            double resulting_energy = getResultingEnergy(mu, k);
            photon.setEnergy(resulting_energy);
            return initial_energy - resulting_energy;
        }
    }
}

ProbabilityDist::DiscreteInversion CoherentScattering::createFormFactorDistribution(Eigen::MatrixXd form_factor_matrix, double x_max) {
    // get CDF = int F(x, Z)^2 dx^2 from 0 to x_max^2
    // square first column of form factor matrix
    form_factor_matrix.col(1) = form_factor_matrix.col(1).array().square();

    Eigen::MatrixXd restricted_form_factor_matrix = PhotonInteractionHelpers::getBlockByRowValue(form_factor_matrix, 0, x_max, 0);
    ProbabilityDist::DiscreteInversion form_factor_dist(restricted_form_factor_matrix);
    return form_factor_dist;
}

double CoherentScattering::sampleThetaFromCoherentScatteringDCS(const ProbabilityDist::DiscreteInversion& form_factor_dist, double x_max) {
    double theta;
    while (true) {
        double sampled_x_squared = form_factor_dist.sample();


        double cos_theta = 1 - 2*sampled_x_squared/(x_max*x_max);
        double random_number = uniform_dist_.sample();
        double g = (1 + cos_theta*cos_theta)/2;
        if (random_number < g) {
            theta = acos(cos_theta);
            break;
        }
    }
    return theta;
}

double IncoherentScattering::getRejectionConstant(double k) {
    if (k < 0.5) {
        return 1.0;
    }
    else {
        return (1 + 2*k)/2;
    }
}

double IncoherentScattering::getX(double k, double random_number) {
    return sqrt(((1 + 2*k)/(1 + 2*k*random_number) - 1)*ALPHA*ALPHA*k);
}

double IncoherentScattering::getRejectionProb(double mu, double beta) {
    return 0.5 * (1 + mu*mu - beta*beta/(1 + beta));
}

double IncoherentScattering::changeTrajectoryAndReturnEnergyForCoherentScattering(Particle& photon, double mu, double k) {
    double theta = acos(mu);
    double phi = 2*PI*uniform_dist_.sample();
    photon.rotate(theta, phi);

    double initial_energy = photon.getEnergy();
    double resulting_energy = getResultingEnergy(mu, k);
    photon.setEnergy(resulting_energy);
    return initial_energy - resulting_energy;
}

double IncoherentScattering::sampleMuFromH(double b, double c_0) {
    // sample h(k, x) = a/(b-x), where x = cos(theta)
    double random_number = uniform_dist_.sample();
    double x = b - (b + 1) * pow(c_0 / 2, random_number);
    return x;
}

double IncoherentScattering::getKNAcceptanceProbability(double a, double b, double mu, double k) {
    // calculate DCS(x) and h(x)
    double f = (1 + mu * mu + (k * k * (1 - mu) * (1 - mu)) / (1 + k * (1 - mu))) / pow(1 + k * (1 - mu), 2);
    double h = a / (b - mu);
    return f/h;
}

double IncoherentScattering::getSFAcceptanceProbability(double mu, double k, Material& material) {
    double x = ALPHA*k*sqrt(1-mu);
    double max_x = ALPHA*k*sqrt(2);
    // S(x, Z)/S(max_x, Z)
    return material.getData().interpolateIncoherentScatteringFunction(x)/material.getData().interpolateIncoherentScatteringFunction(max_x);
}

double IncoherentScattering::getResultingEnergy(double mu, double k) {
    double k_prime = 1/(1 - mu + 1/k);
    return k_prime*ELECTRON_REST_MASS;
}