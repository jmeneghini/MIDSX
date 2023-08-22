#include "quantity.h"

std::string Quantity::getName() {
    return name_;
}

EnergyDeposition::EnergyDeposition() {
    name_ = "Energy Deposition";
}

IncidentEnergy::IncidentEnergy() {
    name_ = "Incident Energy";
}

PrimaryIncidentEnergy::PrimaryIncidentEnergy() {
    name_ = "Primary Incident Energy";
}

SecondaryIncidentEnergy::SecondaryIncidentEnergy() {
    name_ = "Secondary Incident Energy";
}

NumberOfParticles::NumberOfParticles() {
    name_ = "Number of Particles";
}

NumberOfPrimaryParticles::NumberOfPrimaryParticles() {
    name_ = "Number of Primary Particles";
}

NumberOfSecondaryParticles::NumberOfSecondaryParticles() {
    name_ = "Number of Secondary Particles";
}

EntranceCosines::EntranceCosines() {
    name_ = "Entrance Cosines";
}

PrimaryEntranceCosines::PrimaryEntranceCosines() {
    name_ = "Primary Entrance Cosines";
}

NumberOfInteractions::NumberOfInteractions() {
    name_ = "Number of Interactions";
}

void EnergyDeposition::measure(TempTallyData &temp_tally_data) {
    energy_deposition_.push_back(temp_tally_data.energy_deposited);
}

void IncidentEnergy::measure(TempTallyData &temp_tally_data) {
    incident_energy_.push_back(temp_tally_data.initial_photon.getEnergy());
}

void PrimaryIncidentEnergy::measure(TempTallyData &temp_tally_data) {
    if (temp_tally_data.initial_photon.isPrimary()) {
        primary_incident_energy_.push_back(temp_tally_data.initial_photon.getEnergy());
    }
}

void SecondaryIncidentEnergy::measure(TempTallyData &temp_tally_data) {
    if (!temp_tally_data.initial_photon.isPrimary()) {
        secondary_incident_energy_.push_back(temp_tally_data.initial_photon.getEnergy());
    }
}

void NumberOfParticles::measure(TempTallyData &temp_tally_data) {
    number_of_particles_++;
}

void NumberOfPrimaryParticles::measure(TempTallyData &temp_tally_data) {
    if (temp_tally_data.initial_photon.isPrimary()) {
        number_of_primary_particles_++;
    }
}

void NumberOfSecondaryParticles::measure(TempTallyData &temp_tally_data) {
    if (!temp_tally_data.initial_photon.isPrimary()) {
        number_of_secondary_particles_++;
    }
}

void EntranceCosines::measure(TempTallyData &temp_tally_data) {
    entrance_cosine_.push_back(temp_tally_data.entrance_cosine);
}

void PrimaryEntranceCosines::measure(TempTallyData &temp_tally_data) {
    if (temp_tally_data.initial_photon.isPrimary()) {
        primary_entrance_cosine_.push_back(temp_tally_data.entrance_cosine);
    }
}

void NumberOfInteractions::measure(TempTallyData &temp_tally_data) {
    if (temp_tally_data.isInteract) { number_of_interactions_++; }
}

std::vector<double> EnergyDeposition::getValue() const {
    return energy_deposition_;
}

std::vector<double> IncidentEnergy::getValue() const {
    return incident_energy_;
}

std::vector<double> PrimaryIncidentEnergy::getValue() const {
    return primary_incident_energy_;
}

std::vector<double> SecondaryIncidentEnergy::getValue() const {
    return secondary_incident_energy_;
}

int NumberOfParticles::getValue() const {
    return number_of_particles_;
}

int NumberOfPrimaryParticles::getValue() const {
    return number_of_primary_particles_;
}

int NumberOfSecondaryParticles::getValue() const {
    return number_of_secondary_particles_;
}

std::vector<double> EntranceCosines::getValue() const {
    return entrance_cosine_;
}

std::vector<double> PrimaryEntranceCosines::getValue() const {
    return primary_entrance_cosine_;
}

int NumberOfInteractions::getValue() const {
    return number_of_interactions_;
}





