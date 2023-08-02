#ifndef MC_XRAY_TRANSPORT_SPLINE_H
#define MC_XRAY_TRANSPORT_SPLINE_H

#include <Eigen/Core>
#include <unsupported/Eigen/Splines>

// https://stackoverflow.com/a/29825204/14452091

#include <iostream>

namespace Interpolator {

    // Base class
    class Interpolator {
    public:
        virtual ~Interpolator() = default;

        virtual double operator()(double x) const = 0;
    };

    class Spline : public Interpolator{
    public:
        explicit Spline(const Eigen::MatrixXd &data);

        double operator()(double x) const override;

    private:
        double scaledValue(double x) const;

        Eigen::RowVectorXd scaledValues(Eigen::VectorXd const &x_vec) const;

        Eigen::VectorXd x_vec_;
        Eigen::VectorXd y_vec_;

        double x_min_;
        double x_max_;

        Eigen::Spline<double, 1> spline_;
    };

    class LogLogSpline : public Spline {
    public:
        explicit LogLogSpline(const Eigen::Matrix<double, Eigen::Dynamic, 2> &data);

        double operator()(double x) const override;
    };

    class Linear : public Interpolator {
    public:
        explicit Linear(const Eigen::Matrix<double, Eigen::Dynamic, 2> &data);
        double operator()(double x) const override;

    private:
        Eigen::VectorXd x_vec_;
        Eigen::VectorXd y_vec_;

        bool isXSmallerThanMin(double x) const;
        bool isXBiggerThanMax(double x) const;
        int findIndexOfNextLargestValue(double x) const;
        double calculateInterpolatedY(int idx, double x) const;
    };

    class LogLogLinear : public Linear {
    public:
        // convert data to log-log space
        explicit LogLogLinear(const Eigen::Matrix<double, Eigen::Dynamic, 2> &data);

        double operator()(double x) const override;
    };
}

#endif //MC_XRAY_TRANSPORT_SPLINE_H
