#include "photon_interactions.h"

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

double PhotoelectricEffect::interact(Particle& photon, const InteractionData& interaction_data, int element) {
    photon.terminate();
    return photon.getEnergy();
}

double CoherentScattering::interact(Particle& photon, const InteractionData& interaction_data, int element) {
    // get coherent scattering form factor
    Eigen::MatrixXd form_factor_matrix = interaction_data.interaction_data_map.at(element).coherent_form_factor_matrix;

    double k = photon.getEnergy()/ELECTRON_REST_MASS; // unitless
    double a = ELECTRON_REST_MASS/(sqrt(2)*PLANCK_CONSTANT*SPEED_OF_LIGHT*1E8); // in inverse angstroms

    double x_min = 0;
    double x_max = a*k*sqrt(2);

    ProbabilityDist::Discrete form_factor_dist = createFormFactorDistribution(form_factor_matrix, x_min, x_max);
    double theta = sampleThetaFromCoherentScatteringDCS(form_factor_dist, x_max);
    // sample phi (0, 2pi)
    double phi = 2*PI*uniform_dist_.sample();
    // rotate photon direction
    photon.rotate(theta, phi);
    return 0;
}

double IncoherentScattering::interact(Particle& photon, const InteractionData& interaction_data, int element) {
    double k = photon.getEnergy()/ELECTRON_REST_MASS; // unitless

    double c_0 = 2*(2*k*k + 2*k + 1)/pow(2*k + 1, 3);
    double b = (1 + c_0/2)/(1-c_0/2);
    double a = 2*(b-1);

    while (true) {
        double x = sampleXFromH(b, c_0);
        double accept_prob = getAcceptanceProbability(a, b, x, k);

        double random_number_2 = uniform_dist_.sample();
        if (random_number_2 < accept_prob) return changeTrajectoryAndReturnEnergyForCoherentScattering(photon, x, k);
    }
}

ProbabilityDist::Discrete CoherentScattering::createFormFactorDistribution(Eigen::MatrixXd form_factor_matrix, double x_min, double x_max) {
    Eigen::MatrixXd restricted_form_factor_matrix = PhotonInteractionHelpers::getBlockByRowValue(form_factor_matrix, x_min, x_max*x_max, 0);
    ProbabilityDist::Discrete form_factor_dist(restricted_form_factor_matrix);
    return form_factor_dist;
}

double CoherentScattering::sampleThetaFromCoherentScatteringDCS(const ProbabilityDist::Discrete& form_factor_dist, double x_max) {
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

double IncoherentScattering::changeTrajectoryAndReturnEnergyForCoherentScattering(Particle& photon, double x, double k) {
    double theta = acos(x);
    double phi = 2*PI*uniform_dist_.sample();
    photon.rotate(theta, phi);

    double resulting_energy = getResultingEnergy(x, k);
    photon.setEnergy(resulting_energy);
    return photon.getEnergy() - resulting_energy;
}

double IncoherentScattering::sampleXFromH(double b, double c_0) {
    // sample h(k, x) = a/(b-x), where x = cos(theta)
    double random_number = uniform_dist_.sample();
    double x = b - (b + 1) * pow(c_0 / 2, random_number);
}

double IncoherentScattering::getAcceptanceProbability(double a, double b, double x, double k) {
    // calculate DCS(x) and h(x)
    double f = (1 + x * x + (k * k * (1 - x) * (1 - x)) / (1 + k * (1 - x))) / pow(1 + k * (1 - x), 2);
    double h = a / (b - x);
    return f/h;
}

double IncoherentScattering::getResultingEnergy(double x, double k) {
    double k_prime = 1/(1 - x + 1/k);
    return k_prime*ELECTRON_REST_MASS;
}




