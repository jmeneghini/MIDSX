#ifndef MC_XRAY_TRANSPORT_SPLINE_H
#define MC_XRAY_TRANSPORT_SPLINE_H

#include <Eigen/Core>
#include <unsupported/Eigen/Splines>

// https://stackoverflow.com/a/29825204/14452091

#include <iostream>

class SplineInterpolator {
public:
    SplineInterpolator(const Eigen::MatrixXd& data)
            : x_vec_(data.col(0)),
              y_vec_(data.col(1)),
              x_min_(x_vec_.minCoeff()),
              x_max_(x_vec_.maxCoeff()),
              spline_(Eigen::SplineFitting<Eigen::Spline<double, 1>>::Interpolate(
                      y_vec_.transpose(),
                      std::min<int>(x_vec_.rows() - 1, 3),
                      scaledValues(x_vec_)))
    { }

    double operator()(double x) const {
        return spline_(scaledValue(x))(0);
    }

private:
    double scaledValue(double x) const {
        return (x - x_min_) / (x_max_ - x_min_);
    }

    Eigen::RowVectorXd scaledValues(Eigen::VectorXd const &x_vec) const {
        return x_vec.unaryExpr([this](double x) { return scaledValue(x); }).transpose();
    }

    Eigen::VectorXd x_vec_;
    Eigen::VectorXd y_vec_;

    double x_min_;
    double x_max_;

    Eigen::Spline<double, 1> spline_;
};

class LogLogSplineInterpolator : public SplineInterpolator {
public:
    LogLogSplineInterpolator(const Eigen::Matrix<double, Eigen::Dynamic, 2>& data)
            : SplineInterpolator(data)
    { }

    double operator()(double x) const {
        return pow(10, SplineInterpolator::operator()(log10(x)));
    }
};

class LinearInterpolator {
public:
    LinearInterpolator(const Eigen::Matrix<double, Eigen::Dynamic, 2>& data)
            : x_vec_(data.col(0)),
              y_vec_(data.col(1)) // TODO: LogLog Linear Interpolator
    { }

    double operator()(double x) const {
        // Check for out of range x values
        if (x <= x_vec_(0)) return y_vec_(0);
        if (x >= x_vec_(x_vec_.size() - 1)) return y_vec_(y_vec_.size() - 1);

        // Find the interval where the x value is located
        auto upper_bound_it = std::upper_bound(x_vec_.data(), x_vec_.data() + x_vec_.size(), x);
        int idx = upper_bound_it - x_vec_.data();

        // Calculate the interpolation
        double x1 = x_vec_(idx - 1);
        double x2 = x_vec_(idx);
        double y1 = y_vec_(idx - 1);
        double y2 = y_vec_(idx);

        double slope = (y2 - y1) / (x2 - x1);
        return y1 + slope * (x - x1);
};

private:
    Eigen::VectorXd x_vec_;
    Eigen::VectorXd y_vec_;
};

#endif //MC_XRAY_TRANSPORT_SPLINE_H
