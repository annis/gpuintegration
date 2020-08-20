#include <chrono>
#include <cmath>
#include <iostream>
#include <fstream>
#include "function.cuh"
#include "quad/quad.h"
#include "quad/util/cudaUtil.h"
#include <iomanip>
#include "quad/GPUquad/Cuhre.cuh"
#include "quad/util/Volume.cuh"

using namespace quad;

template <typename F>
bool
time_and_call(std::string id, F integrand, double epsrel, double true_value, char const* algname, std::stringstream& outfile,	int _final= 0, int  phase_I_type=0)
{
  using MilliSeconds = std::chrono::duration<double, std::chrono::milliseconds::period>;
  double constexpr epsabs = 1.0e-40;
	
  double lows[] =  {0., 0., 0., 0., 0., 0., 0., 0.};
  double highs[] = {1., 1., 1., 1., 1., 1., 1., 1.};
  
  constexpr int ndim = 8;
  quad::Volume<double, ndim> vol(lows, highs);
  quad::Cuhre<double, ndim> alg(0, nullptr, 0, 0, 1);
	
  //std::string id 			= "BoxIntegral8_15";
  int outfileVerbosity  	= 0;
  int appendMode			= 1;
  
  auto const t0 = std::chrono::high_resolution_clock::now();
  cuhreResult const result = alg.integrate<BoxIntegral8_15>(integrand, epsrel, epsabs, &vol, outfileVerbosity, _final, phase_I_type);
  MilliSeconds dt = std::chrono::high_resolution_clock::now() - t0;
  double const absolute_error = std::abs(result.value - true_value);
  bool good = false;
  
  if(result.status == 0 || result.status == 2){
	  good = true;
  }
  outfile.precision(15);
  FinalDataPrint(outfile, id, true_value, epsrel, epsabs, result.value, result.error,
					result.nregions, result.status, _final, dt.count(), id + ".csv", appendMode);
  outfile.str(""); //clear string stream
 std::cout.precision(17);
  //printf("%.15f +- %.15f epsrel:%e final:%i nregions:%lu flag:%i time:%f\n", result.value, result.error, epsrel, _final, result.nregions, result.status, dt.count());
  std::cout<<id<<",\t"
		   <<true_value<<",\t"
			<<epsrel<<",\t\t\t"
			<<epsabs<<",\t"
			<<result.value<<",\t"
			<<result.error<<",\t"
			<<result.nregions<<",\t"
			<<result.status<<",\t"
			<<_final<<",\t"
			<<dt.count()<<std::endl;
  return good;
}

int main(){
	double epsrel  = 1.0e-3;  // starting error tolerance.	
	int _final 			= 0;
	double true_value 	= 8879.851175413485
;
	std::stringstream outfile;
	BoxIntegral8_15 integrand;
	outfile<<"id, value, epsrel, epsabs, estimate, errorest, regions, converge, final, total_time"<<std::endl; 
	int alternative_phase1 = 0;
	//printf("Testing final = 1 with alternative phase I\n");
	_final = 1;
	while (time_and_call("pdcuhre_f1", integrand, epsrel, true_value, "gpucuhre", outfile, _final) == true && epsrel>=1e-8) {
		epsrel /= 5.0;
	}
	
	_final = 0;
	epsrel = 1.0e-3;
	
	printf("Testing final = 0\n");
	while (time_and_call("pdcuhre_f1", integrand, epsrel, true_value, "gpucuhre", outfile, _final) == true && epsrel >= 2.56e-09) {
      epsrel = epsrel>=1e-6 ? epsrel / 5.0 : epsrel / 2.0;
	}
	
}