#ifndef PROBABILITYDIST_H
#define PROBABILITYDIST_H

#include <random>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

// Define a probability distribution class
class ProbabilityDist {
public:
    // A method to randomly sample a value from the distribution
    // This is a pure virtual function, so it must be implemented by any derived classes
    virtual double sample() const = 0;
};

class UniformDist : public ProbabilityDist {
    UniformDist(double min, double max) : min(min), max(max) {}
    double sample() const {
        // Initialize a random number generator
        std::random_device rd;
        std::mt19937 generator(rd());

        // Define a uniform distribution and sample from it
        std::uniform_real_distribution<double> dist(min, max);
        return dist(generator);
    }

private:
    double min;
    double max;
    std::default_random_engine generator;
};

class DiscreteDist : public ProbabilityDist {
    DiscreteDist(std::vector<double> values, std::vector<double> probabilities, std::string samplingAlgorithm = "inversion") 
    : values(std::move(values)), probabilities(std::move(probabilities)), samplingAlgorithm(std::move(samplingAlgorithm)), generator(std::random_device{}()) {}

    double sample() const {
        if (samplingAlgorithm == "inversion") {
            return sampleInversion();
        }

        // add more if statements here to handle other sampling algorithms

        // If no recognized algorithm is specified, return a sentinel value
        throw std::invalid_argument("Unrecognized sampling algorithm: " + samplingAlgorithm);
    }

    double sampleInversion() const {
        // Compute the cumulative distribution function
        std::vector<double> cdf(probabilities.size());
        std::partial_sum(probabilities.begin(), probabilities.end(), cdf.begin());

        // Sample from the distribution
        std::uniform_real_distribution<double> dist(0.0, cdf.back());
        double sample = dist(generator);

        // Use binary search to find the index of the sampled value
        int index = std::lower_bound(cdf.begin(), cdf.end(), sample) - cdf.begin();

        // Return the sampled value
        return values[index];
    }

private:
    std::vector<double> values;
    std::vector<double> probabilities;
    std::string samplingAlgorithm;
    mutable std::mt19937 generator; // mutable is needed because sample() is const and std::mt19937::operator() is non-const.
};

#endif


