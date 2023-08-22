#include "quantity_container.h"

void QuantityContainer::addQuantity(std::unique_ptr<Quantity> quantity) {
    quantities_.emplace(quantity->getName(), std::move(quantity));
}

void QuantityContainer::measureAll(TempTallyData &temp_tally_data) {
    for (auto &quantity : quantities_) {
        quantity.second->measure(temp_tally_data);
    }
}

void QuantityContainer::processMeasurements() {
    for (auto &quantity : quantities_) {
        if (quantity.first == "Energy Deposition") {
            tally_data_.energy_deposition = InteractionDataHelpers::convertNVectorsToEigenMatrix<double>
                    ({(dynamic_cast<EnergyDeposition*>(quantity.second.get()))->getValue()});
        } else if (quantity.first == "Incident Energy") {
            tally_data_.incident_energy = InteractionDataHelpers::convertNVectorsToEigenMatrix<double>
                    ({(dynamic_cast<IncidentEnergy*>(quantity.second.get()))->getValue()});
        } else if (quantity.first == "Primary Incident Energy") {
            tally_data_.primary_incident_energy = InteractionDataHelpers::convertNVectorsToEigenMatrix<double>
                    ({(dynamic_cast<PrimaryIncidentEnergy*>(quantity.second.get()))->getValue()});
        } else if (quantity.first == "Secondary Incident Energy") {
            tally_data_.secondary_incident_energy = InteractionDataHelpers::convertNVectorsToEigenMatrix<double>
                    ({(dynamic_cast<SecondaryIncidentEnergy*>(quantity.second.get()))->getValue()});
        } else if (quantity.first == "Number of Particles") {
            tally_data_.number_of_particles = (dynamic_cast<NumberOfParticles*>(quantity.second.get()))->getValue();
        } else if (quantity.first == "Number of Primary Particles") {
            tally_data_.number_of_primary_particles = (dynamic_cast<NumberOfPrimaryParticles*>(quantity.second.get()))->getValue();
        } else if (quantity.first == "Number of Secondary Particles") {
            tally_data_.number_of_secondary_particles = (dynamic_cast<NumberOfSecondaryParticles*>(quantity.second.get()))->getValue();
        } else if (quantity.first == "Entrance Cosines") {
            tally_data_.entrance_cosines = InteractionDataHelpers::convertNVectorsToEigenMatrix<double>
                    ({(dynamic_cast<EntranceCosines*>(quantity.second.get()))->getValue()});
        } else if (quantity.first == "Primary Entrance Cosines") {
            tally_data_.primary_entrance_cosines = InteractionDataHelpers::convertNVectorsToEigenMatrix<double>
                    ({(dynamic_cast<PrimaryEntranceCosines*>(quantity.second.get()))->getValue()});
        } else if (quantity.first == "Number of Interactions") {
            tally_data_.number_of_interactions = (dynamic_cast<NumberOfInteractions*>(quantity.second.get()))->getValue();
        }
    }
    // not proud of this, but it works... the physicist's way. Cant seem to find a better way to do this without restructuring quantities
}

// combine the data from multiple containers into one, preventing duplicates
void QuantityContainer::merge(const std::vector<std::unique_ptr<QuantityContainer>>& containers) {
    for (auto &container : containers) {
        for (auto &quantity : container->quantities_) {
            if (quantities_.find(quantity.first) == quantities_.end()) {
                quantities_.emplace(quantity.first, std::move(quantity.second));
            }
        }
    }
}

TallyData QuantityContainer::getTallyData() const {
    return tally_data_;
}

double QuantityContainer::getPrimaryFluence() const {
    // sum of all cosines of entrance angles
    double cosines_sum = 0;
    for (auto &cosine : tally_data_.entrance_cosines) {
        cosines_sum += cosine;
    }
    return 1/(tally_data_.area * cosines_sum);
}

//double QuantityContainer::getPrimaryAirKerma(InteractionData &interaction_data, int MaterialID, const Eigen::VectorXd& energy_bin) const {
//    auto material_data = interaction_data.getMaterial(MaterialID).getData();
//    double energy_absorption = 0;
//
//    for (int i = 0; i < energy_bin.size() - 1; i++) {
//        double energy = energy_bin(i);
//        double energy_bin_width = computeBinWidth(energy_bin, i);
//
//        double cosines_sum = computeCosinesSumForEnergyBin(energy, energy_bin_width);
//
//        energy_absorption += material_data->interpolateMassEnergyAbsorptionCoefficient(energy) * energy * cosines_sum;
//
//    }
//
//    return energy_absorption / tally_data_.area;
//} // this is much cleaner but is yielding statistically smaller results... wtf?

double QuantityContainer::getPrimaryAirKerma(InteractionData &interaction_data, int MaterialID, const Eigen::VectorXd& energy_bin) const {
    auto material_data = interaction_data.getMaterial(MaterialID).getData();
    double energy_absorption = 0;

    for (int i = 0; i < energy_bin.size() - 1; i++) {
        double energy = energy_bin(i);
        double energy_bin_width = energy_bin(i+1) - energy;
        // get sum of entrance cosines for this energy bin
        double cosines_sum = 0;
        int k = 0;
        for (int j = 0; j < tally_data_.primary_entrance_cosines.size(); j++) {
            if (tally_data_.primary_incident_energy(j) <= energy + energy_bin_width && tally_data_.primary_incident_energy(j) > energy - energy_bin_width) {
                cosines_sum += 1/tally_data_.primary_entrance_cosines(j);
                k++;
            }
        }
        if (k == 0) { // no particles in this energy bin. Avoids divide by zero
            energy_absorption += 0;
        }
        else {
            energy_absorption += material_data->interpolateMassEnergyAbsorptionCoefficient(energy) * energy * cosines_sum;
        }
    }
    return energy_absorption / tally_data_.area;
}

void QuantityContainer::setArea(double area) {
    tally_data_.area = area;
}

double QuantityContainer::computeBinWidth(const Eigen::VectorXd& energy_bin, int index) {
    return energy_bin(index+1) - energy_bin(index);
}

double QuantityContainer::computeCosinesSumForEnergyBin(double energy, double energy_bin_width) const {
    double cosines_sum = 0;
    int count = 0;

    for (int j = 0; j < tally_data_.primary_entrance_cosines.size(); j++) {
        if (isWithinEnergyBin(tally_data_.primary_incident_energy(j), energy, energy_bin_width)) {
            cosines_sum += 1/tally_data_.primary_entrance_cosines(j);
            count++;
        }
    }

    return (count == 0) ? 0 : cosines_sum;
}

bool QuantityContainer::isWithinEnergyBin(double incident_energy, double energy, double energy_bin_width) {
    return (incident_energy <= energy + energy_bin_width) && (incident_energy > energy);
}

std::unique_ptr<QuantityContainer> QuantityContainerFactory::PrimaryFluence() {
auto container = std::make_unique<QuantityContainer>();
    container->addQuantity(std::make_unique<EntranceCosines>());
    container->addQuantity(std::make_unique<IncidentEnergy>());
    container->addQuantity(std::make_unique<NumberOfPrimaryParticles>());
    return container;
}

std::unique_ptr<QuantityContainer> QuantityContainerFactory::AllQuantities() {
    auto container = std::make_unique<QuantityContainer>();
    container->addQuantity(std::make_unique<EntranceCosines>());
    container->addQuantity(std::make_unique<PrimaryEntranceCosines>());
    container->addQuantity(std::make_unique<IncidentEnergy>());
    container->addQuantity(std::make_unique<PrimaryIncidentEnergy>());
    container->addQuantity(std::make_unique<SecondaryIncidentEnergy>());
    container->addQuantity(std::make_unique<NumberOfPrimaryParticles>());
    container->addQuantity(std::make_unique<NumberOfSecondaryParticles>());
    container->addQuantity(std::make_unique<NumberOfParticles>());
    container->addQuantity(std::make_unique<NumberOfInteractions>());
    container->addQuantity(std::make_unique<EnergyDeposition>());
    return container;
}

