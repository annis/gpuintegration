#include <oneapi/dpl/execution>
#include <oneapi/dpl/async>
#define CATCH_CONFIG_MAIN
#include "oneAPI/catch2/catch.hpp"

#include <iostream>
#include "oneAPI/pagani/quad/GPUquad/PaganiUtils.dp.hpp"
#include "oneAPI/pagani/quad/util/cudaMemoryUtil.h"
#include "oneAPI/pagani/quad/util/thrust_utils.dp.hpp"
#include "oneAPI/pagani/quad/util/custom_functions.dp.hpp"


template<typename T, size_t size>
T*
make_gpu_arr(std::array<T, size> arr){
	double *d_arr = quad::cuda_malloc_managed<double>(arr.size());
	cuda_memcpy_to_device<double>(d_arr, arr.data(), arr.size());
	return d_arr;
}

TEST_CASE("Half Block")
{
    dpct::device_ext& dev_ct1 = dpct::get_current_device();
    sycl::queue& q_ct1 = dev_ct1.default_queue();
	constexpr size_t size = 512;
	std::array<double, size> arr;
	std::fill(arr.begin(), arr.end(), 3.9);
	
	using MinMax = std::pair<double, double>;
	
	SECTION("Testing min at different positions")
	{
		arr[0] = 1.;
		double* d_arr = make_gpu_arr<double, size>(arr);
		MinMax res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
		
		arr[size-1] = 1.;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
		
		arr[size/2] = 1.;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
		
		arr[size/4] = 1.;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
	}
	
	SECTION("Testing max at different positions")
	{
		arr[0] = 4.1;
		double* d_arr = make_gpu_arr<double, size>(arr);
		MinMax res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[0] = 1.;
		
		arr[size-1] = 4.1;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[size-1] = 1.;
		
		arr[size/2] = 4.1;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[size/2] = 1.;
		
		arr[size/4] = 4.1;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
	}
	
}


TEST_CASE("Full Block")
{
    dpct::device_ext& dev_ct1 = dpct::get_current_device();
    sycl::queue& q_ct1 = dev_ct1.default_queue();
	constexpr size_t size = 1024;
	std::array<double, size> arr;
	std::fill(arr.begin(), arr.end(), 3.9);
	
	using MinMax = std::pair<double, double>;
	
	SECTION("Testing min at different positions")
	{
		arr[0] = 1.;
		double* d_arr = make_gpu_arr<double, size>(arr);
		MinMax res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
		arr[0] = 3.9;
		
		arr[size-1] = 1.;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
		arr[size-1] = 3.9;
		
		arr[size/2] = 1.;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
		arr[size/2] = 3.9;
		
		arr[size/4] = 1.;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
	}
	
	SECTION("Testing max at different positions")
	{
		arr[0] = 4.1;
		double* d_arr = make_gpu_arr<double, size>(arr);
		MinMax res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[0] = 3.9;
		
		arr[size-1] = 4.1;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[size-1] = 3.9;
		
		arr[size/2] = 4.1;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[size/2] = 3.9;
		
		arr[size/4] = 4.1;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[size/4] = 3.9;
	}
	
}

TEST_CASE("Two Full Blocks")
{
    dpct::device_ext& dev_ct1 = dpct::get_current_device();
    sycl::queue& q_ct1 = dev_ct1.default_queue();
	constexpr size_t size = 2048;
	std::array<double, size> arr;
	std::fill(arr.begin(), arr.end(), 3.9);
	
	using MinMax = std::pair<double, double>;
	
	SECTION("Testing min at different positions")
	{
		arr[0] = 1.;
		double* d_arr = make_gpu_arr<double, size>(arr);
		MinMax res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
		arr[0] = 3.9;
		
		arr[size-1] = 1.;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
		arr[size-1] = 3.9;
		
		arr[size/2] = 1.;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
		arr[size/2] = 3.9;
		
		arr[size/4] = 1.;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
	}
	
	SECTION("Testing max at different positions")
	{
		arr[0] = 4.1;
		double* d_arr = make_gpu_arr<double, size>(arr);
		MinMax res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[0] = 3.9;
		
		arr[size-1] = 4.1;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[size-1] = 3.9;
		
		arr[size/2] = 4.1;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[size/2] = 3.9;
		
		arr[size/4] = 4.1;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[size/4] = 3.9;
	}
	
}


TEST_CASE("Misaligned Partial Block")
{
    dpct::device_ext& dev_ct1 = dpct::get_current_device();
    sycl::queue& q_ct1 = dev_ct1.default_queue();
	constexpr size_t size = 1000;
	std::array<double, size> arr;
	std::fill(arr.begin(), arr.end(), 3.9);
	
	using MinMax = std::pair<double, double>;
	
	SECTION("Testing min at different positions")
	{
		arr[0] = 1.;
		double* d_arr = make_gpu_arr<double, size>(arr);
		MinMax res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
		arr[0] = 3.9;
		
		arr[size-1] = 1.;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
		arr[size-1] = 3.9;
		
		arr[size/2] = 1.;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
		arr[size/2] = 3.9;
		
		arr[size/4] = 1.;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
	}
	
	SECTION("Testing max at different positions")
	{
		arr[0] = 4.1;
		double* d_arr = make_gpu_arr<double, size>(arr);
		MinMax res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[0] = 3.9;
		
		arr[size-1] = 4.1;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[size-1] = 3.9;
		
		arr[size/2] = 4.1;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[size/2] = 3.9;
		
		arr[size/4] = 4.1;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
	}
}

TEST_CASE("Misaligned Partial Block with multiple block launches")
{
    dpct::device_ext& dev_ct1 = dpct::get_current_device();
    sycl::queue& q_ct1 = dev_ct1.default_queue();
	constexpr size_t size = 2052;
	std::array<double, size> arr;
	std::fill(arr.begin(), arr.end(), 3.9);
	
	using MinMax = std::pair<double, double>;
	
	SECTION("Testing min at different positions")
	{
		arr[0] = 1.;
		double* d_arr = make_gpu_arr<double, size>(arr);
		MinMax res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
		arr[0] = 3.9;
		
		arr[size-1] = 1.;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
		arr[size-1] = 3.9;
		
		arr[size/2] = 1.;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
		arr[size/2] = 3.9;
		
		arr[size/4] = 1.;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.first == 1.);
		sycl::free(d_arr, q_ct1);
	}
	
	SECTION("Testing max at different positions")
	{
		arr[0] = 4.1;
		double* d_arr = make_gpu_arr<double, size>(arr);
		MinMax res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[0] = 3.9;
		
		arr[size-1] = 4.1;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[size-1] = 3.9;
		
		arr[size/2] = 4.1;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
		arr[size/2] = 3.9;
		
		arr[size/4] = 4.1;
		d_arr = make_gpu_arr<double, size>(arr);
		res = min_max<double>(d_arr, size);
		CHECK(res.second == 4.1);
		sycl::free(d_arr, q_ct1);
	}
}
