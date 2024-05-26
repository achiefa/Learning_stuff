#include <iostream>
#include "spdlog/spdlog.h"

// spdlog header files
//#include "spdlog/spdlog.h"

template<class T>
double dfridr(T &func, const double &x, const double &h, double &err)
{
    const int ntab = 10;
    const double con = 1.4, con2 = (con * con);
    const double big = std::numeric_limits<double>::max();
    const double safe = 2.0;

    int i, j;
    double errt, fac, hh, ans;

    auto a = new double[ntab][ntab];

    if ( h == 0.0 ) throw("h must be nonzero in dfridr");

    hh = h;
    a[0][0] = ( func(x + hh) - func(x - hh) ) / ( 2.0 * hh);
    err = big;

    for ( i = 1; i < ntab; i++ ) {
        hh /= con;
        //std::cout << "Iteration step " << i << "   h = " << hh << std::endl;
        a[0][i] = ( func(x + hh) - func(x - hh) ) / ( 2.0 * hh);
        fac = con2;

        for ( j = 1; j <= 1; j++) {
            a[j][i] = ( a[j-1][i] * fac - a[j-1][i-1] ) / ( fac - 1.0 );
            fac = con2 * fac;
            errt = std::max(abs( a[j][i] * a[j-1][i] ) , abs( a[j][i] * a[j-1][i-1] ));

            if (errt <= err) { 
                err = errt;
                ans = a[j][i];
            }
        }

        if (abs(a[i][i] - a[i-1][i-1]) >= safe * err) break;
    }
    
    return ans;
}

template<class T>
double numddif (T &func, const double& x)
{
    double h = 1.e-4 * x;
    std::cout << "Error for numeric differentiation : " << h << std::endl;
    return ( func(x + h) - 2. * func(x) + func(x - h) ) / h / h;
}