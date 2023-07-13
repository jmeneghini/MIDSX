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

    double k= photon.getEnergy() / (PLANCK_CONSTANT * SPEED_OF_LIGHT * 10E8); // in inverse angstroms

    double x_min = 0;
    double x_max = k;

    // restrict form factor matrix to energies between E_min and E_max
    Eigen::MatrixXd restricted_form_factor_matrix = PhotonInteractionHelpers::getBlockByRowValue(form_factor_matrix, x_min, x_max*x_max, 0);
    ProbabilityDist::Discrete form_factor_dist(restricted_form_factor_matrix);

    double theta;
    while (true) {
        double sampled_x = form_factor_dist.sample();


        double cos_theta = 1 - 2*sampled_x*pow(1/k, 2);
        double random_number = uniform_dist_.sample();
        double g = (1 + cos_theta*cos_theta)/2;
        if (random_number < g) {
            theta = acos(cos_theta);
            break;
        }
    }

    // sample phi (0, 2pi)
    double phi = 2*PI*uniform_dist_.sample();


    // rotate photon direction
    photon.rotate(theta, phi);

    return 0;
}

double IncoherentScattering::interact(Particle& photon, const InteractionData& interaction_data, int element) {
    double E = photon.getEnergy();
    double kappa = E / (ELECTRON_MASS);
    double a_1 = log(1 + 2 * kappa);
    double a_2 = (2 * kappa) * (1 + kappa) / pow((1 + 2 * kappa), 2);

    double pi_1 = a_1 / (a_1 + a_2);
    // pi_2 = 1 - pi_1

    double tau_min = 1 / (1 + 2 * kappa);
    double tau_max = 1;
    double tau;

    double k = photon.getEnergy() / (PLANCK_CONSTANT * SPEED_OF_LIGHT * 10E8); // in inverse angstroms

    // sample phi
    double phi = 2 * PI * uniform_dist_.sample();

    while (true) {
        // sample cos(theta) (PENELOPE-2018 p. 71)

        // special case of inversion sampling where x values do not matter,
        // only y values (binary search isn't necessary,
        // implement more efficient DiscreteDist.sample())
        double random_number_1 = uniform_dist_.sample();
        double random_number_2 = uniform_dist_.sample();

        if (random_number_1 < pi_1) {
            // i = 1
            tau = pow(tau_min, random_number_2);
        } else {
            // i = 2
            tau = sqrt((tau * tau) + random_number_2 * (1 - (tau * tau)));
        }

        double cos_theta = 1 - (1 - tau) / (kappa * tau);
        double E_prime = E / (1 + kappa * (1 - cos_theta));

        // calculate x for scattering function

        double x = k * sqrt(0.5 * (1 - cos_theta));

        // compute scattering function at q_c
        double scattering_function = (*interaction_data.interaction_data_map.at(
                element).incoherent_scattering_function_interpolator)(x);

        double scattering_function_at_pi = (*interaction_data.interaction_data_map.at(
                element).incoherent_scattering_function_interpolator)(k);

        double random_number_3 = uniform_dist_.sample();

        double T = (1 - ((1 - tau) * ((2 * kappa + 1) * tau - 1)) / (kappa * kappa * tau * (1 + tau * tau))) *
                   (scattering_function / scattering_function_at_pi);

        if (random_number_3 < T) {
            // accept
            photon.setEnergy(E_prime);
            photon.rotate(acos(cos_theta), phi);
            return E - E_prime;
        }
    }
}
