#ifndef KOKKOSCUHRE_PHASES_CUH
#define KOKKOSCUHRE_PHASES_CUH

#include "Sample.cuh"
#include "quad.h"

template <int NDIM>
__device__ void
ActualCompute(ViewVectorDouble generators,
              double* g,
              const Structures<double>& constMem,
              size_t feval_index,
              size_t total_feval,
              const member_type team_member)
{

  for (int dim = 0; dim < NDIM; ++dim) {
    g[dim] = 0;
  }

  int threadIdx = team_member.team_rank();
  int blockIdx = team_member.league_rank();

  int posCnt = constMem._gpuGenPermVarStart(feval_index + 1) -
               constMem._gpuGenPermVarStart(feval_index);
  int gIndex = constMem._gpuGenPermGIndex(feval_index);

  for (int posIter = 0; posIter < posCnt; ++posIter) {
    int pos = constMem._gpuGenPos((constMem._gpuGenPermVarStart(feval_index)) +
                                  posIter);
    int absPos = abs(pos);

    if (pos == absPos) {
      g[absPos - 1] = constMem._gpuG(gIndex * NDIM + posIter);
    } else {
      g[absPos - 1] = -constMem._gpuG(gIndex * NDIM + posIter);
    }
  }

  for (int dim = 0; dim < NDIM; dim++) {
    generators(total_feval * dim + feval_index) = g[dim];
  }
}

template <int NDIM>
void
ComputeGenerators(ViewVectorDouble generators,
                  size_t FEVAL,
                  const Structures<double> constMem)
{
  uint32_t nBlocks = 1;
  uint32_t nThreads = BLOCK_SIZE;

  Kokkos::TeamPolicy<Kokkos::LaunchBounds<256, 4>> team_policy1(nBlocks,
                                                                nThreads);
  auto team_policy = Kokkos::Experimental::require(
    team_policy1, Kokkos::Experimental::WorkItemProperty::HintLightWeight);

  Kokkos::parallel_for(
    "Phase1", team_policy1, KOKKOS_LAMBDA(const member_type team_member) {
      int threadIdx = team_member.team_rank();
      int blockIdx = team_member.league_rank();

      size_t perm = 0;
      double g[NDIM];

      for (size_t dim = 0; dim < NDIM; ++dim) {
        g[dim] = 0;
      }

      size_t feval_index = perm * BLOCK_SIZE + threadIdx;
      if (feval_index < FEVAL) {
        ActualCompute<NDIM>(
          generators, g, constMem, feval_index, FEVAL, team_member);
      }

      team_member.team_barrier();

      for (perm = 1; perm < FEVAL / BLOCK_SIZE; ++perm) {
        int feval_index = perm * BLOCK_SIZE + threadIdx;
        ActualCompute<NDIM>(
          generators, g, constMem, feval_index, FEVAL, team_member);
      }

      team_member.team_barrier();

      feval_index = perm * BLOCK_SIZE + threadIdx;
      if (feval_index < FEVAL) {
        int feval_index = perm * BLOCK_SIZE + threadIdx;
        ActualCompute<NDIM>(
          generators, g, constMem, feval_index, FEVAL, team_member);
      }

      team_member.team_barrier();
    });
}

template <typename IntegT, int NDIM>
__device__ void
INIT_REGION_POOL(
  IntegT d_integrand,
  double* dRegions,
  double* dRegionsLength,
  size_t numRegions,
  // const Structures<double>& constMem,
  const double* _gpuG,
  const double* _GPUScale,
  const double* _GPUNorm,
  const int* _gpuGenPermGIndex,
  const double* _cRuleWt,
  int FEVAL,
  int NSETS,
  double* lows,
  double* highs,
  //int iteration,
  int depth,
  const double* generators,
  Region<NDIM>* sRegionPool,
  const member_type team_member)
{

  typedef Kokkos::View<Region<NDIM>*,
                       Kokkos::DefaultExecutionSpace::scratch_memory_space,
                       Kokkos::MemoryTraits<Kokkos::Unmanaged>>
    ScratchViewRegion;

  ScratchViewDouble vol(team_member.team_scratch(0), 1);
  ScratchViewDouble Jacobian(team_member.team_scratch(0), 1);
  ScratchViewDouble ranges(team_member.team_scratch(0), NDIM);
  ScratchViewInt maxDim(team_member.team_scratch(0), 1);
  ScratchViewGlobalBounds sBound(team_member.team_scratch(0), NDIM);

  // int threadIdx = team_member.team_rank();

  if (team_member.team_rank() == 0) {

    int blockIdx = team_member.league_rank();
    Jacobian(0) = 1;
    double maxRange = 0;
    for (int dim = 0; dim < NDIM; ++dim) {
      double lower = dRegions[dim * numRegions + blockIdx];
      sRegionPool[0].bounds[dim].lower = lower;
      sRegionPool[0].bounds[dim].upper =
        lower + dRegionsLength[dim * numRegions + blockIdx];

      sBound(dim).unScaledLower = lows[dim];
      sBound(dim).unScaledUpper = highs[dim];
      ranges(dim) = sBound(dim).unScaledUpper - sBound(dim).unScaledLower;
      //sRegionPool(0).div = depth;

      double range = sRegionPool[0].bounds[dim].upper - lower;
      Jacobian(0) = Jacobian(0) * ranges(dim);

      if (range > maxRange) {
        maxDim(0) = dim;
        maxRange = range;
      }
    }
    vol(0) = ldexp(1., -depth);
  }

  int sIndex = 0;
  team_member.team_barrier();
  Sample<IntegT, NDIM>(d_integrand,
                       sIndex,
                       // constMem,
                       _gpuG,
                       _GPUScale,
                       _GPUNorm,
                       _gpuGenPermGIndex,
                       _cRuleWt,
                       FEVAL,
                       NSETS,
                       sRegionPool,
                       vol.data(),
                       maxDim(0),
                       ranges.data(),
                       Jacobian.data(),
                       generators,
                       // sdata,
                       sBound.data(),
                       team_member);
  team_member.team_barrier();
}

template <typename IntegT, int NDIM>
void
INTEGRATE_GPU_PHASE1(IntegT d_integrand,
                     double* dRegions,
                     double* dRegionsLength,
                     size_t numRegions,
                     double* dRegionsIntegral,
                     double* dRegionsError,
                     double* activeRegions,
                     int* subDividingDimension,
                     //double epsrel,
                     //double epsabs,
                     // Structures<double> constMem,
                     const double* _gpuG,
                     const double* _GPUScale,
                     const double* _GPUNorm,
                     const int* _gpuGenPermGIndex,
                     const double* _cRuleWt,
                     int FEVAL,
                     int NSETS,
                     double* lows,
                     double* highs,
                    // int iteration,
                     int depth,
                     const double* generators)
{

  uint32_t nBlocks = numRegions;
  uint32_t nThreads = BLOCK_SIZE;
  typedef Kokkos::View<Region<NDIM>*,
                       Kokkos::DefaultExecutionSpace::scratch_memory_space,
                       Kokkos::MemoryTraits<Kokkos::Unmanaged>>
    ScratchViewRegion;
  using mainKernelPolicy = Kokkos::TeamPolicy<Kokkos::LaunchBounds<64, 16>>;
  int shMemBytes = ScratchViewInt::shmem_size(1) +       // for maxDim
                   ScratchViewDouble::shmem_size(1) +    // for vol
                   ScratchViewDouble::shmem_size(1) +    // for Jacobian
                   ScratchViewDouble::shmem_size(NDIM) + // for ranges
                   ScratchViewRegion::shmem_size(
                     1) + // how come shmem_size doesn't return size_t? the
                          // tutorial exercise was returning an int too
                   ScratchViewGlobalBounds::shmem_size(NDIM) + // for sBound
                   ScratchViewDouble::shmem_size(BLOCK_SIZE);  // for sdata

  Kokkos::parallel_for(
    "Phase1",
    mainKernelPolicy(nBlocks, nThreads)
      .set_scratch_size(0, Kokkos::PerTeam(shMemBytes)),
    KOKKOS_LAMBDA(const member_type team_member) {
      // Kokkos::parallel_for( "Phase1", team_policy(nBlocks,
      // nThreads).set_scratch_size(0, Kokkos::PerTeam(shMemBytes)),
      // KOKKOS_LAMBDA (const member_type team_member) {

      ScratchViewRegion sRegionPool(team_member.team_scratch(0), 1);

      INIT_REGION_POOL<IntegT, NDIM>(d_integrand,
                                     dRegions,
                                     dRegionsLength,
                                     numRegions,
                                     /*constMem*/ _gpuG,
                                     _GPUScale,
                                     _GPUNorm,
                                     _gpuGenPermGIndex,
                                     _cRuleWt,
                                     FEVAL,
                                     NSETS,
                                     lows,
                                     highs,
                                    // iteration,
                                     depth,
                                     generators,
                                     sRegionPool.data(),
                                     team_member);
      team_member.team_barrier();

      if (team_member.team_rank() == 0) {
        activeRegions[team_member.league_rank()] = 1.;
        subDividingDimension[team_member.league_rank()] =
          sRegionPool(0).result.bisectdim;
        dRegionsIntegral[team_member.league_rank()] = sRegionPool(0).result.avg;
        dRegionsError[team_member.league_rank()] = sRegionPool(0).result.err;
      }
    });
}

#endif
