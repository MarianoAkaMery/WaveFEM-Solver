cmake_minimum_required(VERSION 3.12.0)
project(submission_exam_20260120 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE OR "${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

find_package(MPI REQUIRED)
set(CMAKE_CXX_COMPILER "${MPI_CXX_COMPILER}")

find_package(Boost 1.72.0 REQUIRED
  COMPONENTS filesystem iostreams serialization
  HINTS ${BOOST_DIR} $ENV{BOOST_DIR} $ENV{mkBoostPrefix})
include_directories(${Boost_INCLUDE_DIRS})

find_package(deal.II 9.3.1 REQUIRED
  HINTS ${DEAL_II_DIR} $ENV{DEAL_II_DIR} $ENV{mkDealiiPrefix})
deal_ii_initialize_cached_variables()

add_executable(exercise1_domain_decomposition
  src/exercise1_domain_decomposition.cpp
  src/Exercise1PoissonDD.cpp)
deal_ii_setup_target(exercise1_domain_decomposition)

add_executable(exercise2_unsteady_stokes
  src/exercise2_unsteady_stokes.cpp
  src/Exercise2UnsteadyStokes.cpp)
deal_ii_setup_target(exercise2_unsteady_stokes)
