/*~-------------------------------------------------------------------------~~*
 * Copyright (c) 2016 Los Alamos National Laboratory, LLC
 * All rights reserved
 *~-------------------------------------------------------------------------~~*/
///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief This is where the compile time inputs are defined.
///////////////////////////////////////////////////////////////////////////////

// hydro includes
#include "inputs.h"

#include <flecsale/eos/ideal_gas.h>

// system includes
#include <limits>

namespace apps {
namespace hydro {

//=============================================================================
// type aliases confined to this translation unit
//=============================================================================
using size_t = inputs_t::size_t;
using real_t = inputs_t::real_t;
using vector_t = inputs_t::vector_t;
using string = std::string;
using eos_t = inputs_t::eos_t;
using symmetry_condition_t = symmetry_boundary_condition_t;

//=============================================================================
// These constants are not part of the input class, they are globally scoped
// within this translation unit only.
//=============================================================================

// the value of gamma
constexpr real_t gamma = 1.4;

// the grid dimensions
constexpr size_t num_cells_x = 32;
//constexpr real_t length_x = 1.2;
constexpr real_t length_x = 1.0;

// some length scales
auto dx = length_x / num_cells_x;

// compute a reference volume
auto vol = dx;

// compute a radial size
auto delta_r = std::numeric_limits<real_t>::epsilon() + 
  std::abs( dx/2 );

// we are only using symmetry boundary conditions here
auto symmetry_condition = std::make_shared<symmetry_condition_t>();

//=============================================================================
// Now set the inputs.
//=============================================================================

// the case prefix
string inputs_t::prefix = "sedov_1d";
string inputs_t::postfix = "dat";

// output frequency
size_t inputs_t::output_freq = 20;

// the CFL and final solution time
time_constants_t inputs_t::CFL = 
{ .accoustic = 0.25, .volume = 0.1, .growth = 1.01 };

real_t inputs_t::final_time = 1.0;
real_t inputs_t::initial_time_step = 1.e-5;
size_t inputs_t::max_steps = 20;

// the equation of state
eos_t inputs_t::eos = 
  flecsale::eos::ideal_gas_t<real_t>( 
    /* gamma */ 1.4, /* cv */ 1.0 
  ); 

// this is a lambda function to set the initial conditions
inputs_t::ics_function_t inputs_t::ics = 
  [g=gamma, V=vol]( const vector_t & x, const real_t & )
  {
    constexpr real_t e0 = 0.244816;
    real_t d = 1.0;
    vector_t v = 0;
    real_t p = 1.e-6;
    auto r = abs( x[0] );
    if ( r < delta_r  )
      p = (g - 1) * d * e0 / V;
    return std::make_tuple( d, v, p );
  };

// install each boundary
//
// - both +ve and -ve side boundaries can be installed at once since 
//   they will never overlap
// - if they did overlap, then you need to define them seperately or else
//   its hard to count the number of different conditions on points or edges.
inputs_t::bcs_list_t inputs_t::bcs = {

  // the +/- x-axis boundaries
  std::make_pair( 
    symmetry_condition,
    [lx=length_x]( const vector_t & x, const real_t & t )
    { 
      return ( x[0] == 0.0 || x[0] == lx );
    }
  ),
};

} // namespace
} // namespace
