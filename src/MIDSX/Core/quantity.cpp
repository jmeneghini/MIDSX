#include "Core/quantity.h"

VectorValue VectorValue::operator+(const VectorValue& other) const {
    // combine two vectors
    VectorValue sum = *this;
    sum.addValues(other.getVector());
    // reset sum and var
    sum.summed_ = false;
    sum.var_calculated_ = false;
    sum.sum_ = 0;
    sum.var_ = 0;
    return sum;
}

void VectorValue::addValue(double value) {
    values_.push_back(value);
}

void VectorValue::addValues(const std::vector<double>& values) {
    values_.insert(values_.end(), values.begin(), values.end());
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

double VectorValue::getCount() {
    return values_.size();
}

double VectorValue::getCountSTD() {
    return sqrt(values_.size());
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

CountValue CountValue::operator+(const CountValue& other) const {
    CountValue sum;
    sum.count_ = count_ + other.count_;
    return sum;
}

void CountValue::addCount() {
    count_++;
}

void CountValue::addCounts(int count) {
    count_ += count;
}

int CountValue::getCount() const {
    return count_;
}

double CountValue::getCountSTD() const {
    return sqrt(count_);
}