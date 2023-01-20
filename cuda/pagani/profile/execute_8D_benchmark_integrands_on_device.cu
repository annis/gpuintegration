#include <iostream>
#include "cuda/pagani/demos/new_time_and_call.cuh"
#include <array>
#include <cuda_profiler_api.h>
#include "common/cuda/integrands.cuh"

int main(int argc, char** argv){
  size_t num_invocations = argc > 1 ? std::stoi(argv[1]) : 100000;
  double sum = 0.;
  sum += execute_integrand_at_points<F_1_8D, 8>(num_invocations);
  sum += execute_integrand_at_points<F_2_8D, 8>(num_invocations);
  sum += execute_integrand_at_points<F_3_8D, 8>(num_invocations);
  sum += execute_integrand_at_points<F_4_8D, 8>(num_invocations);
  sum += execute_integrand_at_points<F_5_8D, 8>(num_invocations);
  sum += execute_integrand_at_points<F_6_8D, 8>(num_invocations);
  printf("%.15e\n", sum);
  
  return 0;
}
