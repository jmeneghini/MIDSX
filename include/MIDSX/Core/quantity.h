#ifndef MCXRAYTRANSPORT_QUANTITY_H
#define MCXRAYTRANSPORT_QUANTITY_H

#include <Eigen/Dense>
#include "photon.h"
#include "tally_data.h"

class VectorValue {
public:
    void addValue(double value);
    void addValues(const std::vector<double>& values);
    std::vector<double> getVector() const;
    double getSum();
    double getSumSTD();
    double getMean();
    double getMeanSTD();
    double getCount();
    double getCountSTD();
    double getVariance();
private:
    std::vector<double> values_ = {};
    double sum_ = 0;
    bool summed_ = false; // prevent multiple calls to getSum()
    double var_ = 0;
    bool var_calculated_ = false;
};

class CountValue {
public:
    void addCount();
    void addCounts(int count);
    int getCount() const;
    double getCountSTD() const;
public:
    int count_= 0;
};

#endif //MCXRAYTRANSPORT_QUANTITY_H
