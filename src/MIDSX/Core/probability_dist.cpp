#include "Core/probability_dist.h"

// ensures rng is thread safe
thread_local std::mt19937 ProbabilityDist::Uniform::generator_(std::random_device{}());

int ProbabilityDistHelpers::findIndexOfNextSmallestValue(double x, const Eigen::VectorXd &vector) {
    auto lower_bound_it = std::lower_bound(vector.data(), vector.data() + vector.size(), x);
    return lower_bound_it - vector.data();
}

ProbabilityDist::DiscreteInversion::DiscreteInversion(const Eigen::Matrix<double, Eigen::Dynamic, 2> &probabilities_matrix)
        : probabilities_matrix_(probabilities_matrix),
        uniform_dist_(Uniform(0.0, 1.0)) {
    normalize(); // normalize probabilities if necessary
    cdf_matrix_ = generateCDF();
}

double ProbabilityDist::DiscreteInversion::sample() const {
    double sample = uniform_dist_.sample();
    Eigen::VectorXd cdf = cdf_matrix_.col(1);
    int index = ProbabilityDistHelpers::findIndexOfNextSmallestValue(sample, cdf);
    return cdf_matrix_(index, 0);
}

double ProbabilityDist::DiscreteInversion::getExpectationValue() const {
    return probabilities_matrix_.col(0).dot(probabilities_matrix_.col(1));
}

void ProbabilityDist::DiscreteInversion::normalize() {
    double sum = probabilities_matrix_.col(1).sum();
    if (sum != 1.0) {
        probabilities_matrix_.col(1) /= sum;
    }
}

Eigen::Matrix<double, Eigen::Dynamic, 2> ProbabilityDist::DiscreteInversion::generateCDF() const {
    Eigen::VectorXd probabilities = probabilities_matrix_.col(1);
    Eigen::VectorXd cdf = cumsum(probabilities);

    Eigen::Matrix<double, Eigen::Dynamic, 2> cdf_matrix(cdf.rows(), 2);
    cdf_matrix.col(0) = probabilities_matrix_.col(0);
    cdf_matrix.col(1) = cdf;

    return cdf_matrix;
}

Eigen::VectorXd ProbabilityDist::DiscreteInversion::cumsum(const Eigen::VectorXd &vector) {
    Eigen::VectorXd cumsum_vector(vector.rows());
    cumsum_vector(0) = vector(0);
    for (int j = 1; j < vector.rows(); ++j) {
        cumsum_vector(j) = cumsum_vector(j - 1) + vector(j);
    }

    return cumsum_vector;
}

ProbabilityDist::ContinuousInversion::ContinuousInversion(std::function<double(double, double)> &PDF, Eigen::VectorXd& energies, double x_min, double x_max) :
        PDF_(PDF),
        energies_(std::move(energies)),
        x_min_(x_min),
        x_max_(x_max),
        uniform_dist_(Uniform(0.0, 1.0)) {
    initializeCDFAndInterpolationParameters();
}

double ProbabilityDist::ContinuousInversion::sample(double E) const {
    // get CDF and interpolation parameters for E
    int lower_index = ProbabilityDistHelpers::findIndexOfNextSmallestValue(E, energies_);
    int upper_index = lower_index + 1;

    // get x for each E
    double y = uniform_dist_.sample();

    double x_lower = getXFromY(lower_index, y);
    double x_upper = getXFromY(upper_index, y);

    // interpolate x for E
    double x = x_lower + (x_upper - x_lower) / (energies_(upper_index) - energies_(lower_index)) * (E - energies_(lower_index));

    return x;
}

double ProbabilityDist::ContinuousInversion::getXFromY(int energy_index, double y) const {
    // get lower index of y
    Eigen::Array<double, Eigen::Dynamic, 2> CDF_PER_ENERGY = CDF_[energy_index];
    Eigen::Array<double, Eigen::Dynamic, 2> interp_parameters_per_energy = interp_parameters_[energy_index];

    int y_i_index = ProbabilityDistHelpers::findIndexOfNextSmallestValue(y, CDF_PER_ENERGY.col(1));

    if (y_i_index == CDF_PER_ENERGY.rows() - 1) {
        // y is greater than max y
        return x_max_;
    }
    else if (y_i_index == 0) {
        // y is less than min y
        return x_min_;
    }
    else {
        double y_i = CDF_PER_ENERGY(y_i_index, 1);
        double y_i_1 = CDF_PER_ENERGY(y_i_index + 1, 1);
        double x_i = CDF_PER_ENERGY(y_i_index, 0);
        double x_i_1 = CDF_PER_ENERGY(y_i_index + 1, 0);
        double a_i = interp_parameters_per_energy(y_i_index, 0);
        double b_i = interp_parameters_per_energy(y_i_index, 1);

        double nu = y - y_i;
        double delta_i = y_i_1 - y_i;

        double x = x_i + (1 + a_i + b_i) * delta_i * nu / (delta_i * delta_i + a_i * delta_i * nu + b_i * nu * nu) * (x_i_1 - x_i);
        return x;
    }
}

void ProbabilityDist::ContinuousInversion::initializeCDFAndInterpolationParameters() {
    for (int i = 0; i < energies_.size(); ++i) {
        // generate CDF for each energy
        Eigen::Array<double, Eigen::Dynamic, 2> CDF_PER_ENERGY = getMinimizedErrorCDFPerEnergy(energies_(i), 1E-5);
        // normalize CDF
        CDF_PER_ENERGY.col(1) /= CDF_PER_ENERGY(CDF_PER_ENERGY.rows() - 1, 1);

        // calculate interpolation parameters for each energy
        Eigen::Array<double, Eigen::Dynamic, 2> interp_parameters_per_energy = calculateInterpolationParametersPerEnergy(
                energies_(i), CDF_PER_ENERGY);

        CDF_.push_back(CDF_PER_ENERGY);
        interp_parameters_.push_back(interp_parameters_per_energy);
    }
}

Eigen::Array<double, Eigen::Dynamic, 2> ProbabilityDist::ContinuousInversion::getMinimizedErrorCDFPerEnergy(double E, double err_thresh) {
    // get initial CDF with 10 grid points
    Eigen::VectorXd x_grid = Eigen::VectorXd::LinSpaced(10, x_min_, x_max_);

    bool threshold_met = false;

    while (!threshold_met) {
        Eigen::Array<double, Eigen::Dynamic, 2> CDF_RITA_PER_ENERGY = generateCDFPerEnergy(E, x_grid);
        Eigen::Array<double, Eigen::Dynamic, 2> interp_parameters_per_energy_RITA = calculateInterpolationParametersPerEnergy(
                E, CDF_RITA_PER_ENERGY);
        std::vector<double> new_x_grid;

        threshold_met = true;

        for (int i = 0; i < x_grid.rows() - 1; ++i) {
            IntervalData id;
            id.x_i = x_grid(i);
            new_x_grid.push_back(id.x_i);
            id.x_i_1 = x_grid(i + 1);
            id.y_i = CDF_RITA_PER_ENERGY(i, 1);
            id.y_i_1 = CDF_RITA_PER_ENERGY(i + 1, 1);
            id.a_i = interp_parameters_per_energy_RITA(i, 0);
            id.b_i = interp_parameters_per_energy_RITA(i, 1);
            double eps_i = getInterpErrorOverInterval(E, id);
            if (eps_i > err_thresh) {
                // add new grid points at midpoint if error is too large
                double x_new = (id.x_i + id.x_i_1) / 2;
                new_x_grid.push_back(x_new);
                threshold_met = false;
            }
        }
        new_x_grid.push_back(x_grid(x_grid.rows() - 1)); // add last grid point
        x_grid = Eigen::Map<Eigen::VectorXd, Eigen::Unaligned>(new_x_grid.data(), new_x_grid.size()); // move new grid points to x_grid
    }

    Eigen::Array<double, Eigen::Dynamic, 2> CDF_RITA_PER_ENERGY = generateCDFPerEnergy(E, x_grid);
    return CDF_RITA_PER_ENERGY;
}

double ProbabilityDist::ContinuousInversion::getInterpErrorOverInterval(double E, IntervalData id) const {
    // PENELOPE 1.57
    // calculate eps_i = integral from x_i to x_i_1 of abs(p(x) - p_num(x)) dx using trapezoidal rule N=51
    const int N = 51;
    double delta_x = (id.x_i_1 - id.x_i) / N;
    double sum = 0;
    for (int i = 1; i < N; ++i) {
        double x_k = id.x_i + i * delta_x;
        double p = PDF_(x_k, E);
        double p_num = getPDFFromCDFOverInterval(E, x_k, id);
        sum += abs(p - p_num);
    }
    double eps_i = delta_x * (sum + (abs(PDF_(id.x_i_1, E) - getPDFFromCDFOverInterval(E, id.x_i_1, id))
            + abs(PDF_(id.x_i, E) - getPDFFromCDFOverInterval(E, id.x_i, id))) / 2);
    return eps_i;
}

double ProbabilityDist::ContinuousInversion::getPDFFromCDFOverInterval(double E, double x, IntervalData id) const {
    // PENELOPE 1.55
    double eta = getEta(x, id);
    double p_num = pow(1 + id.a_i * eta + id.b_i * eta * eta, 2) / ((1 + id.a_i + id.b_i) * (1 - id.b_i * eta * eta)) * (id.y_i_1 - id.y_i) / (id.x_i_1 - id.x_i);
    return p_num;
}

double ProbabilityDist::ContinuousInversion::getEta(double x, IntervalData id) const {
    // PENELOPE 1.56
    double tau = (x - id.x_i) / (id.x_i_1 - id.x_i);
    if (tau == 0) {
        return 0;
    }
    double eta = (1 + id.a_i + id.b_i - id.a_i * tau) / (2 * id.b_i * tau) * (1 - sqrt(1 - 4 * id.b_i * tau * tau / pow(1 + id.a_i + id.b_i - id.a_i * tau, 2)));
    return eta;
}




Eigen::Array<double, Eigen::Dynamic, 2> ProbabilityDist::ContinuousInversion::calculateInterpolationParametersPerEnergy(
        double E, Eigen::Array<double, Eigen::Dynamic, 2> CDF_RITA_PER_ENERGY) {
    // calculate a_i and b_i for E

    const int CDF_RITA_SIZE = CDF_RITA_PER_ENERGY.rows();
    Eigen::Array<double, Eigen::Dynamic, 2> interp_parameters_per_energy_RITA(CDF_RITA_SIZE - 1, 2);

    for (int i = 0; i < CDF_RITA_SIZE - 1; ++i) {
        double x_i = CDF_RITA_PER_ENERGY(i, 0);
        double x_i_1 = CDF_RITA_PER_ENERGY(i + 1, 0);
        double y_i = CDF_RITA_PER_ENERGY(i, 1);
        double y_i_1 = CDF_RITA_PER_ENERGY(i + 1, 1);
        double b_i = 1 - pow((y_i_1 - y_i) / (x_i_1 - x_i), 2) * (1 / (PDF_(x_i, E) * PDF_(x_i_1, E)));
        double a_i = (y_i_1 - y_i) / (x_i_1 - x_i) * (1 / PDF_(x_i, E)) - b_i - 1;
        interp_parameters_per_energy_RITA(i, 0) = a_i;
        interp_parameters_per_energy_RITA(i, 1) = b_i;
    }

    return interp_parameters_per_energy_RITA;
}

Eigen::Array<double, Eigen::Dynamic, 2> ProbabilityDist::ContinuousInversion::generateCDFPerEnergy(double E, Eigen::VectorXd x_grid) {

    int NUM_GRID_POINTS = x_grid.size();

    const int NUM_POINTS = 51;

    // calculate CDF using extended Simpson's rule with 51 equally spaced points

    Eigen::Array<double, Eigen::Dynamic, 2> CDF_RITA_PER_ENERGY(NUM_GRID_POINTS, 2);

    CDF_RITA_PER_ENERGY(0, 0) = x_min_;
    CDF_RITA_PER_ENERGY(0, 1) = 0;

    for (int j = 0; j < NUM_GRID_POINTS - 1; ++j) {
        double x = x_grid(j);
        double x_1 = x_grid(j + 1);
        double h = (x_1 - x) / (NUM_POINTS - 1);
        double sum = 0;
        for (int k = 0; k < NUM_POINTS; ++k) {
            double x_k = x + k * h;
            double f = PDF_(x_k, E);
            if (k == 0 || k == NUM_POINTS - 1) {
                sum += f;
            } else if (k % 2 == 0) {
                sum += 2 * f;
            } else {
                sum += 4 * f;
            }
        }
        CDF_RITA_PER_ENERGY(j + 1, 0) = x_1;
        CDF_RITA_PER_ENERGY(j + 1, 1) = CDF_RITA_PER_ENERGY(j, 1) + h / 3 * sum;
    }
    return CDF_RITA_PER_ENERGY;
}




