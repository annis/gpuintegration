#include <CL/sycl.hpp>
#include <iostream>
#include "oneAPI/pagani/demos/new_time_and_call.dp.hpp"
#include "common/oneAPI/integrands.hpp"

int
main(int argc, char** argv)
{
  int num_repeats = argc > 1 ? std::stoi(argv[1]) : 11;
    constexpr int ndim = 8;
    F_2_8D integrand;
	quad::Volume<double, ndim> vol;
	call_cubature_rules<F_2_8D, ndim>(integrand, vol, num_repeats);
    return 0;
}

