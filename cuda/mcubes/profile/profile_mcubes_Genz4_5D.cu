#include "cuda/mcubes/demos/demo_utils.cuh"
#include "cuda/mcubes/vegasT.cuh"

class GENZ_4_5D {
  public:
    __host__ __device__ double
    operator()(double x, double y, double z, double w, double v)
    {
      double beta = .5;
      return exp(
        -1.0 * (pow(25., 2.) * pow(x - beta, 2.) + pow(25., 2.) * pow(y - beta, 2.) +
                pow(25., 2.) * pow(z - beta, 2.) + pow(25., 2.) * pow(w - beta, 2.) +
                pow(25., 2.) * pow(v - beta, 2.)));
    }
};

int
main(int argc, char** argv)
{
  double epsrel = 1e-3;
  constexpr int ndim = 5;

  double ncall = 1.0e8;
  int titer = 1;
  int itmax = 1;
  int skip = 0;
  VegasParams params(ncall, titer, itmax, skip);
  double true_value = 1.79132603674879e-06;
  
  double lows[] = {0., 0., 0., 0., 0.};
  double highs[] = {1., 1., 1., 1., 1.};
  quad::Volume<double, ndim> volume(lows, highs);
  
  GENZ_4_5D integrand;
  std::array<double, 6> required_ncall = {1.e5, 1.e6, 1.e7, 1.e8, 1.e9, 2.e9};
   
  size_t run = 0;
  
  for(auto num_samples : required_ncall){
    params.ncall = num_samples;
    
	signle_invocation_time_and_call<GENZ_4_5D, ndim>(
        integrand, epsrel, true_value, "f4, 5", params, &volume);
	run++;
	if(run > required_ncall.size())
		break;
  }

  return 0;
}