#include "Core/quantity.h"



void VectorValue::addValue(double value) {
    values_.push_back(value);
}

std::vector<double> VectorValue::getVector() const {
    return values_;
}

double VectorValue::getSum() {
    double sum = 0;
    for (auto& value : values_) {
        sum += value;
    }
    sum_ = sum;
    summed_ = true;
    return sum;
}

double VectorValue::getSumSTD() {
    double var = 0;
    if (!var_calculated_) {
        var = getVariance();
    } else {
        var = var_;
    }
    return sqrt(values_.size() * var);
}

double VectorValue::getMean() {
    double sum = 0;
    if (!summed_) {
        sum = getSum();
    } else {
        sum = sum_;
    }
    double mean = sum / values_.size();
    return mean;
}

double VectorValue::getMeanSTD() {
    // sample standard deviation
    double var = 0;
    if (!var_calculated_) {
        var = getVariance();
    } else {
        var = var_;
    }
    return sqrt(var / values_.size());
}

double VectorValue::getVariance() {
    // sample variance
    double sum = 0;
    if (!summed_) {
        sum = getSum();
    } else {
        sum = sum_;
    }
    double mean = sum / values_.size();
    double var = 0;
    for (auto& value : values_) {
        var += (value - mean) * (value - mean);
    }
    var /= (values_.size() - 1);
    var_ = var;
    var_calculated_ = true;
    return var;
}

void CountValue::addCount() {
    count_++;
}

int CountValue::getCount() const {
    return count_;
}

double CountValue::getCountSTD() const {
    return sqrt(count_);
}