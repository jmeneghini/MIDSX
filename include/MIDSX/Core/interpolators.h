#ifndef MC_XRAY_TRANSPORT_SPLINE_H
#define MC_XRAY_TRANSPORT_SPLINE_H

#include <Eigen/Core>
#include <unsupported/Eigen/Splines>

namespace Interpolator {

    class Interpolator {
    public:
        virtual ~Interpolator() = default;
        virtual double operator()(double x) const = 0;
    };

    class Spline : public Interpolator{
    public:
        Spline() = default;  // Default constructor
        explicit Spline(const Eigen::MatrixXd &data);

        double operator()(double x) const override;
    private:
        double scaledValue(double x) const;
        Eigen::RowVectorXd scaledValues(Eigen::VectorXd const &x_vec) const;

        Eigen::VectorXd x_vec_;
        Eigen::VectorXd y_vec_;
        double x_min_ = 0.0;
        double x_max_ = 0.0;
        Eigen::Spline<double, 1> spline_;
    };

    class LogLogSpline : public Spline {
    public:
        LogLogSpline() = default;  // Default constructor
        explicit LogLogSpline(const Eigen::Matrix<double, Eigen::Dynamic, 2> &data);

        double operator()(double x) const override;
    };

    class Linear : public Interpolator {
    public:
        Linear() = default;  // Default constructor
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
        LogLogLinear() = default;  // Default constructor
        explicit LogLogLinear(const Eigen::Matrix<double, Eigen::Dynamic, 2> &data);

        double operator()(double x) const override;
    };
}

#endif //MC_XRAY_TRANSPORT_SPLINE_H
