#ifndef MC_XRAY_TRANSPORT_SPLINE_H
#define MC_XRAY_TRANSPORT_SPLINE_H

#include <Eigen/Core>
#include <unsupported/Eigen/Splines>


namespace Interpolator {

    /**
     * @brief Abstract base class for interpolators.
     *
     * This class is used to create a common interface for all interpolators.
     */
    class Interpolator {
    public:
        virtual ~Interpolator() = default;
        virtual double operator()(double x) const = 0;
    };

    /**
     * @brief Class which performs 3rd order spline interpolation.
     *
     *
     */
    class Spline : public Interpolator{
    public:
        Spline() = default;
        /**
         * @brief Constructor for the Spline class.
         *
         * @param data A 2 column matrix containing the x and y values to be interpolated.
         */
        explicit Spline(const Eigen::MatrixXd &data);

        /**
         * @brief Performs 3rd order spline interpolation.
         *
         * @param x The x value to be interpolated.
         * @return The interpolated y value.
         */
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

    /**
     * @brief Class which performs 3rd order spline interpolation on a log-log scale.
     */
    class LogLogSpline : public Spline {
    public:
        LogLogSpline() = default;  // Default constructor
        /**
         * @brief Constructor for the LogLogSpline class.
         *
         * @param data A 2 column matrix containing the x and y values to be interpolated.
         */
        explicit LogLogSpline(const Eigen::Matrix<double, Eigen::Dynamic, 2> &data);

        /**
         * @brief Performs 3rd order spline interpolation on a log-log scale.
         *
         * @param x The x value to be interpolated.
         * @return The interpolated y value.
         */
        double operator()(double x) const override;
    };

    /**
     * @brief Class which performs linear interpolation.
     */
    class Linear : public Interpolator {
    public:
        Linear() = default;  // Default constructor
        /**
         * @brief Constructor for the Linear class.
         *
         * @param data A 2 column matrix containing the x and y values to be interpolated.
         */
        explicit Linear(const Eigen::Matrix<double, Eigen::Dynamic, 2> &data);

        /**
         * @brief Performs linear interpolation.
         *
         * @param x The x value to be interpolated.
         * @return The interpolated y value.
         */
        double operator()(double x) const override;
    private:
        Eigen::VectorXd x_vec_;
        Eigen::VectorXd y_vec_;

        bool isXSmallerThanMin(double x) const;
        bool isXBiggerThanMax(double x) const;
        int findIndexOfNextLargestValue(double x) const;
        double calculateInterpolatedY(int idx, double x) const;
    };

    /**
     * @brief Class which performs linear interpolation on a log-log scale.
     */
    class LogLogLinear : public Linear {
    public:
        LogLogLinear() = default;  // Default constructor
        /**
         * @brief Constructor for the LogLogLinear class.
         *
         * @param data A 2 column matrix containing the x and y values to be interpolated.
         */
        explicit LogLogLinear(const Eigen::Matrix<double, Eigen::Dynamic, 2> &data);

        /**
         * @brief Performs linear interpolation on a log-log scale.
         *
         * @param x The x value to be interpolated.
         * @return The interpolated y value.
         */
        double operator()(double x) const override;
    };
}

#endif //MC_XRAY_TRANSPORT_SPLINE_H
