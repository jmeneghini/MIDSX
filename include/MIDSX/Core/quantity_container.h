#ifndef MCXRAYTRANSPORT_QUANTITY_CONTAINER_H
#define MCXRAYTRANSPORT_QUANTITY_CONTAINER_H

#include <vector>
#include "quantity.h"
#include "interaction_data.h"
#include "material_helpers.h"
#include <memory>
#include <unordered_map>

class QuantityContainer {
public:
    void addQuantity(std::unique_ptr<Quantity> quantity);
    void measureAll(TempTallyData& temp_tally_data);
    void processMeasurements();
    void merge(const std::vector<std::unique_ptr<QuantityContainer>>& containers);

    TallyData getTallyData() const;
    double getPrimaryFluence() const;
    double getPrimaryAirKerma(InteractionData &interaction_data, int MaterialID, const Eigen::VectorXd& energy_bin) const;
    void setArea(double area);

    // For iterating over quantities
    auto begin() {return quantities_.begin();}
    auto end() {return quantities_.end();}
private:
    std::unordered_map<std::string, std::unique_ptr<Quantity>> quantities_;
    TallyData tally_data_;
};

namespace QuantityContainerFactory {
    std::unique_ptr<QuantityContainer> PrimaryFluence();
    std::unique_ptr<QuantityContainer> AllQuantities();
}


#endif //MCXRAYTRANSPORT_QUANTITY_CONTAINER_H