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





