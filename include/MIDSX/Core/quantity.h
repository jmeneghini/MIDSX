#ifndef MCXRAYTRANSPORT_QUANTITY_H
#define MCXRAYTRANSPORT_QUANTITY_H

#include <vector>
#include <cmath>

/**
 * @brief Class which represents a vector quantity. Used by the Tally classes to store simulation data.
 */
class VectorValue {
public:
    // overloading the + operator
    VectorValue operator+(VectorValue& other) const;

    // pretty self-explanatory
    void addValue(double value);
    void addValues(const std::vector<double>& values);
    std::vector<double>& getVector();
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

/**
 * @brief Class which represents a count quantity. Used by the Tally classes to store simulation data.
 */
class CountValue {
public:
    // overloading the + operator
    CountValue operator+(const CountValue& other) const;

    // pretty self-explanatory
    void addCount();
    void addCounts(int count);
    int getCount() const;
    double getCountSTD() const;
public:
    int count_= 0;
};

#endif //MCXRAYTRANSPORT_QUANTITY_H
