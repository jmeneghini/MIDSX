#include "source.h"

Eigen::Vector3d SourceHelpers::angleToUnitDirection(double theta, double phi) {
    Eigen::Vector3d unit_direction;
    unit_direction << sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta);
    return unit_direction;
}

Eigen::MatrixXd SourceHelpers::readCSV(std::string file) {
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



BeamDirectionality::BeamDirectionality(const Eigen::Vector3d &pass_through_point): pass_through_point_(pass_through_point) {}

Eigen::Vector3d BeamDirectionality::sampleDirection(const Eigen::Vector3d &photon_initial_position) {
    Eigen::Vector3d beam_direction = pass_through_point_ - photon_initial_position;
    return beam_direction.normalized();
}



RectangularIsotropicDirectionality::RectangularIsotropicDirectionality(const Eigen::Vector3d& min_position_of_rectangle,
                                                                       const Eigen::Vector3d& max_position_of_rectangle):
                                                                       min_position_(min_position_of_rectangle),
                                                                       max_position_(max_position_of_rectangle),
                                                                       uniform_dist_(0, 2*PI) {
    assert(min_position_.x() < max_position_.x());
    assert(min_position_.y() < max_position_.y());
    setNormal();
}

Eigen::Vector3d RectangularIsotropicDirectionality::sampleDirection(const Eigen::Vector3d &photon_initial_position) {
    uniform_dist_.setRange(min_position_.x(), max_position_.x());
    double x = uniform_dist_.sample();
    uniform_dist_.setRange(min_position_.y(), max_position_.y());
    double y = uniform_dist_.sample();
    double z = getZFromPlaneEquation(x, y);
    Eigen::Vector3d direction = Eigen::Vector3d(x, y, z) - photon_initial_position;
    return direction.normalized();
};

void RectangularIsotropicDirectionality::setNormal() {
    Eigen::Vector3d min_corner_to_max_corner = max_position_ - min_position_;
    Eigen::Vector3d min_corner_to_above_corner = Eigen::Vector3d(max_position_.x(), min_position_.y(), min_position_.z()) - min_position_;
    normal_ = min_corner_to_max_corner.cross(min_corner_to_above_corner);
}

double
RectangularIsotropicDirectionality::getZFromPlaneEquation(double x, double y) {
    return min_position_.z() - (normal_.x() * (x - min_position_.x()) + normal_.y() * (y - min_position_.y())) / normal_.z();
}

SourceGeometry::SourceGeometry(const Eigen::Vector3d &position) : position_(position) {};

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





