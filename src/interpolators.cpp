#include "interpolators.h"

Interpolator::Spline::Spline(const Eigen::MatrixXd &data) :
        x_vec_(data.col(0)),
        y_vec_(data.col(1)),
        x_min_(x_vec_.minCoeff()),
        x_max_(x_vec_.maxCoeff()),
        spline_(Eigen::SplineFitting<Eigen::Spline<double, 1>>::Interpolate(
                y_vec_.transpose(),
                std::min<int>(x_vec_.rows() - 1, 3),
                scaledValues(x_vec_)))
{ }

double Interpolator::Spline::operator()(double x) const {
    return spline_(scaledValue(x))(0);
}

double Interpolator::Spline::scaledValue(double x) const {
    return (x - x_min_) / (x_max_ - x_min_);
}

Eigen::RowVectorXd Interpolator::Spline::scaledValues(Eigen::VectorXd const &x_vec) const {
    return x_vec.unaryExpr([this](double x) { return scaledValue(x); }).transpose();
}

Interpolator::LogLogSpline::LogLogSpline(const Eigen::Matrix<double, Eigen::Dynamic, 2> &data)
        : Spline(data.unaryExpr([](double x) { return log10(x); })) { };

double Interpolator::LogLogSpline::operator()(double x) const {
    return pow(10, Spline::operator()(log10(x)));
}

Interpolator::Linear::Linear(const Eigen::Matrix<double, Eigen::Dynamic, 2> &data)
        : x_vec_(data.col(0)),
          y_vec_(data.col(1)) { };

double Interpolator::Linear::operator()(double x) const {
    // Check for out of range x values
    if (isXSmallerThanMin(x)) {
        return y_vec_(0);
    } else if (isXBiggerThanMax(x)) {
        return y_vec_(y_vec_.size() - 1);
    }
    int idx = findIndexOfNextLargestValue(x);
    return calculateInterpolatedY(idx, x);
}

bool Interpolator::Linear::isXSmallerThanMin(double x) const {
    return x <= x_vec_(0);
}

bool Interpolator::Linear::isXBiggerThanMax(double x) const {
    return x >= x_vec_(x_vec_.size() - 1);
}

int Interpolator::Linear::findIndexOfNextLargestValue(double x) const {
    auto upper_bound_it = std::upper_bound(x_vec_.data(), x_vec_.data() + x_vec_.size(), x);
    return upper_bound_it - x_vec_.data();
}

double Interpolator::Linear::calculateInterpolatedY(int idx, double x) const {
    double x1 = x_vec_(idx - 1);
    double x2 = x_vec_(idx);
    double y1 = y_vec_(idx - 1);
    double y2 = y_vec_(idx);

    double slope = (y2 - y1) / (x2 - x1);
    return y1 + slope * (x - x1);
}

Interpolator::LogLogLinear::LogLogLinear(const Eigen::Matrix<double, Eigen::Dynamic, 2> &data)
        : Linear(data.unaryExpr([](double x) { return log10(x); })) { };

double Interpolator::LogLogLinear::operator()(double x) const {
    return pow(10, Linear::operator()(log10(x)));
}