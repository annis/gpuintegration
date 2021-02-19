#include "demo_utils.cuh"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace quad;


namespace detail{
    class GENZ_4_5D {
    public:
        __device__ __host__ double
        operator()(double x, double y, double z, double w, double v){
            //double alpha = 25.;
            double beta = .5;
            return exp(-1.0*(pow(25,2)*pow(x-beta, 2) + 
                             pow(25,2)*pow(y-beta, 2) +
                             pow(25,2)*pow(z-beta, 2) +
                             pow(25,2)*pow(w-beta, 2) +
                             pow(25,2)*pow(v-beta, 2))
                      );
        }
    };
}

int
main()
{
  double epsrel =  1e-3;
  double const epsrel_min = 1.0240000000000002e-10;
  double true_value = 1.79132603674879e-06;
  detail::GENZ_4_5D integrand;
  PrintHeader();
  constexpr int ndim = 5;
  
  while (cu_time_and_call<detail::GENZ_4_5D, ndim>("Genz4_5D",
                                                integrand,
                                                epsrel,
                                                true_value,
                                                std::cout) == true &&
                                                epsrel > epsrel_min) {
    epsrel /= 5.0;
  }

}
