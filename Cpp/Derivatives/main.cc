#include <fmt/format.h>
#include <iostream>
#include <stdlib.h>

#include "Ridders.h"


double func (double const &x) {
    return x * x * x * x;
}

double dfunc (double const &x) {
    return 4.0 * x * x * x;
}

double ddfunc (double const &x) {
    return 12.0 * x * x;
}

double dddfuncc (double const &x) {
    return 24. * x;
}

double fxy (double const &x, double const &y) {
    return x * x * y + y * y * x + x + y;
}

double dx_fxy (double const &x, double const &y) {
    return 2.0 * x * y + y * y + 1.0;
}

double dy_fxy (double const &x, double const &y) {
    return 2.0 * x * y + x * x + 1.0;
}

double dxdx_fxy (double const &x, double const &y) {
    return 2.0 * y;
}

double dydy_fxy (double const &x, double const &y) {
    return 2.0 * x;
}

double dydx_fxy (double const &x, double const &y) {
    return 2.0 * ( x + y );
}

int main(int argc, char *argv[])
{
    double x = atof(argv[1]);
    double h = atof(argv[2]);
    double err;

    //const std::function<double(double const&)> num_ddfunc = [&] (double const& x) -> double
    //{
    //  return dfridr(func, x, h, err);
    //};

    //std::cout << h << std::endl;
    //std::cout << ddfunc(x) << std::endl;
    //std::cout << dfridr(num_ddfunc, x, h, err) << std::endl;
    //std::cout << numddif(func, x) << std::endl;
    //std::cout << err << std::endl;
    std::cout << "The exact Hessian is : (" << dxdx_fxy(1.,1.);
    std::cout << " , " << dydy_fxy(1.,1.);
    std::cout << " , " << dydx_fxy(1.,1.) << " )" << std::endl;

    printf("Starting the calculation of the numerical Hessian...\n");
    auto C = new double[2];
    C[0] = 1.;
    C[1] = 1.;
    

    return 0;
}