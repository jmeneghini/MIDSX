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
    Eigen::MatrixXd readCSV(const std::string& file);
}


/**
 * @brief Virtual class which represents an energy spectrum for a photon source.
 */
class EnergySpectrum {
public:
    virtual double sampleEnergy() = 0;
    virtual ~EnergySpectrum() = default;
};

/**
 * @brief Class which represents a monoenergetic energy spectrum.
 */
class MonoenergeticSpectrum : public EnergySpectrum {
public:
    /**
     * @brief Constructor for the MonoenergeticSpectrum class.
     *
     * @param energy The energy of the monoenergetic spectrum.
     */
    explicit MonoenergeticSpectrum(double energy);

    /**
     * @brief Returns the energy of the monoenergetic spectrum.
     *
     * @return A sample from the monoenergetic spectrum. Will always be the same value.
     */
    double sampleEnergy() override;

private:
    double energy_;
};

/**
 * @brief Class which represents a polyenergetic energy spectrum.
 */
class PolyenergeticSpectrum : public EnergySpectrum {
public:
    /**
     * @brief Constructor for the PolyenergeticSpectrum class.
     *
     * @param probabilities_matrix A matrix where the first column is the energy and the second column is the probability.
     */
    explicit PolyenergeticSpectrum(const Eigen::Matrix<double, Eigen::Dynamic, 2> &probabilities_matrix);

    /**
     * @brief Returns a sample from the polyenergetic spectrum.
     *
     * @return A sample from the polyenergetic spectrum.
     */
    double sampleEnergy() override;

    /**
     * @brief Returns the expectation value of the polyenergetic spectrum.
     *
     * @return The expectation value of the polyenergetic spectrum.
     */
    double getExpectationValue() const;

private:
    Eigen::Matrix<double, Eigen::Dynamic, 2> probabilities_matrix_;
    ProbabilityDist::DiscreteInversion energy_dist_;
};

/**
 * @brief Virtual class which represents a directionality for a photon source.
 */
class Directionality {
public:
    virtual Eigen::Vector3d sampleDirection(const Eigen::Vector3d &photon_initial_position) = 0;
    virtual ~Directionality() = default;
};

/**
 * @brief Class which represents an isotropic directionality.
 *
 * The isotropic directionality is a uniform distribution over the unit sphere.
 */
class IsotropicDirectionality : public Directionality {
public:
    IsotropicDirectionality();
    /**
     * @brief Returns a sample from the isotropic directionality.
     *
     * @return A sample from the isotropic directionality.
     */
    Eigen::Vector3d sampleDirection(const Eigen::Vector3d &photon_initial_position) override;
private:
    ProbabilityDist::Uniform uniform_dist_;
};

/**
 * @brief Class which represents a beam directionality.
 *
 * The beam directionality is infinitely narrow and passes through a point.
 */
class BeamDirectionality : public Directionality {
public:
    /**
     * @brief Constructor for the BeamDirectionality class.
     *
     * @param pass_through_point The point that the beam passes through.
     */
    explicit BeamDirectionality(Eigen::Vector3d pass_through_point);

    /**
     * @brief Returns a sample from the beam directionality.
     *
     * @return A sample from the beam directionality.
     */
    Eigen::Vector3d sampleDirection(const Eigen::Vector3d &photon_initial_position) override;
private:
    Eigen::Vector3d pass_through_point_;
};

/**
 * @brief Class which represents a rectangular isotropic directionality.
 *
 * The rectangular isotropic directionality is a uniform distribution over a rectangle.
 */
class RectangularIsotropicDirectionality : public Directionality {
public:
    /**
     * @brief Constructor for the RectangularIsotropicDirectionality class.
     *
     * @param corner The vector to the corner of the rectangle from the origin.
     * @param edge1 The vector from the corner to the second corner of the rectangle.
     * @param edge2 The vector from the corner to the third corner of the rectangle.
     */
    RectangularIsotropicDirectionality(Eigen::Vector3d corner, Eigen::Vector3d edge1, Eigen::Vector3d edge2);

    /**
     * @brief Returns a sample from the rectangular isotropic directionality.
     *
     * @return A sample from the rectangular isotropic directionality.
     */
    Eigen::Vector3d sampleDirection(const Eigen::Vector3d &photon_initial_position) override;
private:
    Eigen::Vector3d corner_;
    Eigen::Vector3d edge1_;
    Eigen::Vector3d edge2_;
    Eigen::Vector3d normal_;
    ProbabilityDist::Uniform uniform_dist_;

    void handleOrthogonalEdges();

    bool areEdgesOrthogonal();
};

/**
 * @brief Class which represents a disc isotropic directionality.
 *
 * The disc isotropic directionality is a uniform distribution over a disc.
 */
class DiscIsotropicDirectionality : public Directionality {
public:
    /**
     * @brief Constructor for the DiscIsotropicDirectionality class.
     *
     * @param center The center of the disc.
     * @param normal The normal vector of the disc.
     * @param radius The radius of the disc.
     */
    DiscIsotropicDirectionality(Eigen::Vector3d center, Eigen::Vector3d normal, double radius);

    /**
     * @brief Returns a sample from the disc isotropic directionality.
     *
     * @return A sample from the disc isotropic directionality.
     */
    Eigen::Vector3d sampleDirection(const Eigen::Vector3d &photon_initial_position) override;
private:
    Eigen::Vector3d center_;
    Eigen::Vector3d normal_;
    double radius_;
    Eigen::Vector3d u_;
    Eigen::Vector3d v_;
    ProbabilityDist::Uniform uniform_dist_;

    void setUAndV();
    Eigen::Vector3d calculateNormalizedPerimeterVector(double theta);
};

/**
 * @brief Virtual class which represents a geometry for a photon source.
 */
class SourceGeometry {
public:
    virtual Eigen::Vector3d samplePosition() = 0;
    virtual ~SourceGeometry() = default;
protected:
    explicit SourceGeometry(Eigen::Vector3d  position);
    Eigen::Vector3d position_;
};

/**
 * @brief Class which represents a point geometry.
 *
 * The point geometry is a single point.
 */
class PointGeometry : public SourceGeometry {
public:
    /**
     * @brief Constructor for the PointGeometry class.
     *
     * @param position The position of the point.
     */
    explicit PointGeometry(const Eigen::Vector3d& position);

    /**
     * @brief Returns the position of the point.
     *
     * @return The position of the point.
     */
    Eigen::Vector3d samplePosition() override;
};

/**
 * @brief Class which represents a photon source.
 *
 * The photon source is a combination of an energy spectrum, a directionality, and a geometry.
 */
class PhotonSource {
public:
    PhotonSource() = default;
    /**
     * @brief Constructor for the PhotonSource class.
     *
     * @param energy_spectrum The energy spectrum of the photon source.
     * @param directionality The directionality of the photon source.
     * @param source_geometry The geometry of the photon source.
     */
    PhotonSource(std::unique_ptr<EnergySpectrum> energy_spectrum,
                 std::unique_ptr<Directionality> directionality,
                 std::unique_ptr<SourceGeometry> source_geometry);

    /**
     * @brief Returns a photon generated from the photon source.
     * @return
     */
    Photon generatePhoton();
private:
    std::unique_ptr<EnergySpectrum> energy_spectrum_;
    std::unique_ptr<Directionality> directionality_;
    std::unique_ptr<SourceGeometry> source_geometry_;
};



#endif
    

