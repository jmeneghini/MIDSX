#include "Core/source.h"
#include <utility>

Eigen::Vector3d SourceHelpers::angleToUnitDirection(double theta, double phi) {
    Eigen::Vector3d unit_direction;
    unit_direction << sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta);
    return unit_direction;
}

Eigen::MatrixXd SourceHelpers::readCSV(const std::string& file) {
    std::ifstream in(file);
    std::string line;

    std::vector<double> values;
    int rows = 0;
    int cols = 0;

    if (in.is_open()) {
        while (std::getline(in, line)) {
            std::stringstream lineStream(line);
            std::string cell;
            while (std::getline(lineStream, cell, ',')) {
                values.push_back(std::stod(cell));
            }
            ++rows;
        }
        in.close();
    }

    cols = values.size() / rows; // Calculate number of columns
    return Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>(values.data(), rows, cols);
}

MonoenergeticSpectrum::MonoenergeticSpectrum(double energy) : energy_(energy) {}

double MonoenergeticSpectrum::sampleEnergy() {
    return energy_;
}

PolyenergeticSpectrum::PolyenergeticSpectrum(const Eigen::Matrix<double, Eigen::Dynamic, 2> &probabilities_matrix)
: energy_dist_(probabilities_matrix) {}

double PolyenergeticSpectrum::sampleEnergy() {
    double energy = energy_dist_.sample();
    return energy;
}

double PolyenergeticSpectrum::getExpectationValue() const {
    return energy_dist_.getExpectationValue();
}

IsotropicDirectionality::IsotropicDirectionality() : uniform_dist_(0, 2*PI) {}

Eigen::Vector3d IsotropicDirectionality::sampleDirection(const Eigen::Vector3d &photon_initial_position) {
    uniform_dist_.setRange(0, PI);
    double theta = uniform_dist_.sample();
    uniform_dist_.setRange(0, 2*PI);
    double phi = uniform_dist_.sample();
    return SourceHelpers::angleToUnitDirection(theta, phi);
}

BeamDirectionality::BeamDirectionality(Eigen::Vector3d pass_through_point): pass_through_point_(std::move(pass_through_point)) {}

Eigen::Vector3d BeamDirectionality::sampleDirection(const Eigen::Vector3d &photon_initial_position) {
    Eigen::Vector3d beam_direction = pass_through_point_ - photon_initial_position;
    return beam_direction.normalized();
}

RectangularIsotropicDirectionality::RectangularIsotropicDirectionality(Eigen::Vector3d corner, Eigen::Vector3d edge1, Eigen::Vector3d edge2):
                                                                       corner_(std::move(corner)), edge1_(std::move(edge1)), edge2_(std::move(edge2)),
                                                                       uniform_dist_(0, 1) {
    handleOrthogonalEdges();
}

Eigen::Vector3d RectangularIsotropicDirectionality::sampleDirection(const Eigen::Vector3d &photon_initial_position) {
    double alpha_1 = uniform_dist_.sample();
    double alpha_2 = uniform_dist_.sample();
    Eigen::Vector3d direction = (corner_ + alpha_1 * edge1_ + alpha_2 * edge2_ - photon_initial_position).normalized();
    return direction;
}

void RectangularIsotropicDirectionality::handleOrthogonalEdges() {
    if (!areEdgesOrthogonal()) {
        throw std::invalid_argument("Edges are not orthogonal");
    }
}

bool RectangularIsotropicDirectionality::areEdgesOrthogonal() {
    double EPSILON = 1e-9;
    return std::abs(edge1_.dot(edge2_)) < EPSILON;
}

DiscIsotropicDirectionality::DiscIsotropicDirectionality(Eigen::Vector3d center, Eigen::Vector3d normal, double radius) :
        center_(std::move(center)), normal_(std::move(normal)), radius_(radius), uniform_dist_(0, 1) {
    setUAndV();
}

Eigen::Vector3d DiscIsotropicDirectionality::sampleDirection(const Eigen::Vector3d &photon_initial_position) {
    uniform_dist_.setRange(0, 2*PI);
    double theta = uniform_dist_.sample();
    uniform_dist_.setRange(0, radius_);
    double r = uniform_dist_.sample();
    Eigen::Vector3d direction = center_ + r * calculateNormalizedPerimeterVector(theta) - photon_initial_position;
    return direction.normalized();
}

void DiscIsotropicDirectionality::setUAndV() {
    double EPSILON = 1e-9;
    // Try to set u orthogonal to normal and x hat. If parallel, set u orthogonal to normal and y hat
    Eigen::Vector3d x_hat(1, 0, 0);
    Eigen::Vector3d y_hat(0, 1, 0);
    if (std::abs(normal_.dot(x_hat)) < EPSILON) {
        u_ = normal_.cross(y_hat).normalized();
    } else {
        u_ = normal_.cross(x_hat).normalized();
    }
    // calculate v, which is orthogonal to u and normal
    v_ = normal_.cross(u_).normalized();
}

Eigen::Vector3d DiscIsotropicDirectionality::calculateNormalizedPerimeterVector(double theta) {
    return Eigen::Vector3d (u_ * cos(theta) + v_ * sin(theta));
}

SourceGeometry::SourceGeometry(Eigen::Vector3d position) : position_(std::move(position)) {};

PointGeometry::PointGeometry(const Eigen::Vector3d &position) : SourceGeometry(position) {};

Eigen::Vector3d PointGeometry::samplePosition() {
    return position_;
}

PhotonSource::PhotonSource(std::unique_ptr<EnergySpectrum> energy_spectrum,
                           std::unique_ptr<Directionality> directionality,
                           std::unique_ptr<SourceGeometry> source_geometry) :
                            energy_spectrum_(std::move(energy_spectrum)),
                            directionality_(std::move(directionality)),
                            source_geometry_(std::move(source_geometry)) {};

Photon PhotonSource::generatePhoton() {
    Eigen::Vector3d position = source_geometry_->samplePosition();
    Eigen::Vector3d direction = directionality_->sampleDirection(position);
    double energy = energy_spectrum_->sampleEnergy();
    return Photon(position, direction, energy);
}