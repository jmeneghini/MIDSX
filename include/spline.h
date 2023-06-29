#ifndef MC_XRAY_TRANSPORT_SPLINE_H
#define MC_XRAY_TRANSPORT_SPLINE_H

#include <Eigen/Core>
#include <unsupported/Eigen/Splines>

// https://stackoverflow.com/a/29825204/14452091

#include <iostream>

class SplineFunction {
public:
    SplineFunction(Eigen::VectorXd const &x_vec,
                   Eigen::VectorXd const &y_vec)
            : x_min_(x_vec.minCoeff()),
              x_max_(x_vec.maxCoeff()),
              spline_(Eigen::SplineFitting<Eigen::Spline<double, 1>>::Interpolate(
                      y_vec.transpose(),
                      std::min<int>(x_vec.rows() - 1, 3),
                      scaledValues(x_vec)))
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

    double x_min_;
    double x_max_;

    Eigen::Spline<double, 1> spline_;
};

#endif //MC_XRAY_TRANSPORT_SPLINE_H
