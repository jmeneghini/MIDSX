#ifndef MCXRAYTRANSPORT_QUANTITY_H
#define MCXRAYTRANSPORT_QUANTITY_H

#include <Eigen/Dense>
#include "photon.h"
#include "tally_data.h"

class Quantity {
public:
    virtual void measure(TempTallyData& temp_tally_data) = 0;
    std::string getName();
    virtual ~Quantity() = default;
protected:
    std::string name_;
};

class EnergyDeposition : public Quantity {
public:
    EnergyDeposition();
    void measure(TempTallyData& temp_tally_data) override;
    std::vector<double> getValue() const;
private:
    std::vector<double> energy_deposition_;
};

class IncidentEnergy : public Quantity {
public:
    IncidentEnergy();
    void measure(TempTallyData& temp_tally_data) override;
    std::vector<double> getValue() const;
private:
    std::vector<double> incident_energy_;
};

class PrimaryIncidentEnergy : public Quantity {
public:
    PrimaryIncidentEnergy();
    void measure(TempTallyData& temp_tally_data) override;
    std::vector<double> getValue() const;
private:
    std::vector<double> primary_incident_energy_;
};

class SecondaryIncidentEnergy : public Quantity {
public:
    SecondaryIncidentEnergy();
    void measure(TempTallyData& temp_tally_data) override;
    std::vector<double> getValue() const;
private:
    std::vector<double> secondary_incident_energy_;
};

class NumberOfParticles : public Quantity {
public:
    NumberOfParticles();
    void measure(TempTallyData& temp_tally_data) override;
    int getValue() const;
private:
    int number_of_particles_ = 0;
};

class NumberOfPrimaryParticles : public Quantity {
public:
    NumberOfPrimaryParticles();
    void measure(TempTallyData& temp_tally_data) override;
    int getValue() const;
private:
    int number_of_primary_particles_ = 0;
};

class NumberOfSecondaryParticles : public Quantity {
public:
    NumberOfSecondaryParticles();
    void measure(TempTallyData& temp_tally_data) override;
    int getValue() const;
private:
    int number_of_secondary_particles_ = 0;
};

class EntranceCosines : public Quantity {
public:
    EntranceCosines();
    void measure(TempTallyData& temp_tally_data) override;
    std::vector<double> getValue() const;
private:
    std::vector<double> entrance_cosine_;
};

class PrimaryEntranceCosines : public Quantity {
public:
    PrimaryEntranceCosines();
    void measure(TempTallyData& temp_tally_data) override;
    std::vector<double> getValue() const;
private:
    std::vector<double> primary_entrance_cosine_;
};

class NumberOfInteractions : public Quantity {
public:
    NumberOfInteractions();
    void measure(TempTallyData& temp_tally_data) override;
    int getValue() const;
private:
    int number_of_interactions_ = 0;
};


#endif //MCXRAYTRANSPORT_QUANTITY_H
