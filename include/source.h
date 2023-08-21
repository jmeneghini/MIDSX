#ifndef SOURCE_H
#define SOURCE_H

#include <Eigen/Dense>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "photon.h"
#include "probability_dist.h"
#include "constants.h"

namespace SourceHelpers {
    Eigen::Vector3d angleToUnitDirection(double theta, double phi);
    Eigen::MatrixXd readCSV(std::string file);
}


class EnergySpectrum {
public:
    virtual double sampleEnergy() = 0;
    virtual ~EnergySpectrum() = default;
};

class MonoenergeticSpectrum : public EnergySpectrum {
public:
    explicit MonoenergeticSpectrum(double energy);
    double sampleEnergy() override;

private:
    double energy_;
};

class PolyenergeticSpectrum : public EnergySpectrum {
public:
    explicit PolyenergeticSpectrum(const Eigen::Matrix<double, Eigen::Dynamic, 2> &probabilities_matrix);
    double sampleEnergy() override;
    double getExpectationValue() const;

private:
    Eigen::Matrix<double, Eigen::Dynamic, 2> probabilities_matrix_;
    ProbabilityDist::Discrete energy_dist_;
};

class Directionality {
public:
    virtual Eigen::Vector3d sampleDirection(const Eigen::Vector3d &photon_initial_position) = 0;
    virtual ~Directionality() = default;
};

class IsotropicDirectionality : public Directionality {
public:
    IsotropicDirectionality();
    Eigen::Vector3d sampleDirection(const Eigen::Vector3d &photon_initial_position) override;

private:
    ProbabilityDist::Uniform uniform_dist_;
};

class BeamDirectionality : public Directionality {
public:
    explicit BeamDirectionality(const Eigen::Vector3d &pass_through_point);
    Eigen::Vector3d sampleDirection(const Eigen::Vector3d &photon_initial_position) override;

private:
    Eigen::Vector3d pass_through_point_;
};

class RectangularIsotropicDirectionality : public Directionality {
public:
    RectangularIsotropicDirectionality(const Eigen::Vector3d& min_position_of_rectangle,
                                       const Eigen::Vector3d& max_position_of_rectangle);
    Eigen::Vector3d sampleDirection(const Eigen::Vector3d &photon_initial_position) override;

private:
    Eigen::Vector3d min_position_;
    Eigen::Vector3d max_position_;
    Eigen::Vector3d normal_;
    ProbabilityDist::Uniform uniform_dist_;

    void setNormal();

    double getZFromPlaneEquation(double x, double y);
};

class SourceGeometry {
public:
    virtual Eigen::Vector3d samplePosition() = 0;
    virtual ~SourceGeometry() = default;

protected:
    explicit SourceGeometry(const Eigen::Vector3d& position);
    Eigen::Vector3d position_;
};

class PointGeometry : public SourceGeometry {
public:
    explicit PointGeometry(const Eigen::Vector3d& position);
    Eigen::Vector3d samplePosition() override;
};

class PhotonSource {
public:
    PhotonSource() = default;
    PhotonSource(std::unique_ptr<EnergySpectrum> energy_spectrum,
                 std::unique_ptr<Directionality> directionality,
                 std::unique_ptr<SourceGeometry> source_geometry);

    Photon generatePhoton();

private:
    std::unique_ptr<EnergySpectrum> energy_spectrum_;
    std::unique_ptr<Directionality> directionality_;
    std::unique_ptr<SourceGeometry> source_geometry_;
};



#endif
    

