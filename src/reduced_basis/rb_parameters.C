// rbOOmit: An implementation of the Certified Reduced Basis method.
// Copyright (C) 2009, 2010 David J. Knezevic

// This file is part of rbOOmit.

// rbOOmit is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// rbOOmit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// libmesh includes
#include "libmesh/rb_parameters.h"
#include "libmesh/utility.h"

// C++ includes
#include <sstream>

namespace libMesh
{

RBParameters::RBParameters() :
  _n_steps(1)
{
}

RBParameters::RBParameters(const std::map<std::string, Real> & parameter_map) :
  _n_steps(1)
{
  // Backwards compatible support for constructing an RBParameters
  // object from a map<string, Real>. We store a single entry in each
  // vector in the map.
  for (const auto & [key, val] : parameter_map)
    _parameters[key] = {val};
}

void RBParameters::clear()
{
  _n_steps = 1;
  _parameters.clear();
  _extra_parameters.clear();
}

bool RBParameters::has_value(const std::string & param_name) const
{
  return _parameters.count(param_name);
}

bool RBParameters::has_extra_value(const std::string & param_name) const
{
  return _extra_parameters.count(param_name);
}

Real RBParameters::get_value(const std::string & param_name) const
{
  // get_value() is maintained for backwards compatibility. It simply
  // returns the [0]th entry of the vector if it can, throwing an
  // error otherwise.
  return this->get_step_value(param_name, /*step=*/0);
}

Real RBParameters::get_value(const std::string & param_name, const Real & default_val) const
{
  // get_value() is maintained for backwards compatibility. It simply
  // returns the [0]th entry of the vector if it can, or the default
  // value otherwise.
  return this->get_step_value(param_name, /*step=*/0, default_val);
}

Real RBParameters::get_step_value(const std::string & param_name, std::size_t step) const
{
  const auto & vec = libmesh_map_find(_parameters, param_name);
  libmesh_error_msg_if(step >= vec.size(), "Error getting value for parameter " << param_name);
  return vec[step];
}

Real RBParameters::get_step_value(const std::string & param_name, std::size_t step, const Real & default_val) const
{
  auto it = _parameters.find(param_name);
  return ((it != _parameters.end() && step < it->second.size()) ? it->second[step] : default_val);
}

void RBParameters::set_value(const std::string & param_name, Real value)
{
  // This version of set_value() does not take an index and is provided
  // for backwards compatibility. It creates a vector entry for the specified
  // param_name, overwriting any value(s) that were present.
  _parameters[param_name] = {value};
}

void
RBParameters::set_value_helper(std::map<std::string, std::vector<Real>> & map,
                               const std::string & param_name,
                               std::size_t index,
                               Real value)
{
  // Get reference to vector of values for this parameter, creating it
  // if it does not already exist.
  auto & vec = map[param_name];

  // If vec is already big enough, just set the value
  if (vec.size() > index)
    vec[index] = value;

  // Otherwise push_back() if the vec is just barely not big enough
  else if (vec.size() == index)
    vec.push_back(value);

  // Otherwise, allocate more space (padding with 0s) if vector is not
  // big enough to fit the user's requested index.
  else
    {
      vec.resize(index+1);
      vec[index] = value;
    }
}

void RBParameters::set_value(const std::string & param_name, std::size_t index, Real value)
{
  this->set_value_helper(_parameters, param_name, index, value);
}

void RBParameters::set_extra_value(const std::string & param_name, std::size_t index, Real value)
{
  this->set_value_helper(_extra_parameters, param_name, index, value);
}

void RBParameters::push_back_value(const std::string & param_name, Real value)
{
  // Get reference to vector of values for this parameter, creating it
  // if it does not already exist, and push back the specified value.
  _parameters[param_name].push_back(value);
}

void RBParameters::push_back_extra_value(const std::string & param_name, Real value)
{
  // Get reference to vector of values for this extra parameter, creating it
  // if it does not already exist, and push back the specified value.
  _extra_parameters[param_name].push_back(value);
}

Real RBParameters::get_extra_value(const std::string & param_name) const
{
  // Same as get_value(param_name) but for the map of extra parameters
  const auto & vec = libmesh_map_find(_extra_parameters, param_name);
  libmesh_error_msg_if(vec.size() == 0, "Error getting value for extra parameter " << param_name);
  return vec[0];
}

Real RBParameters::get_extra_value(const std::string & param_name, const Real & default_val) const
{
  // same as get_value(param_name, default_val) but for the map of extra parameters
  auto it = _extra_parameters.find(param_name);
  return ((it != _extra_parameters.end() && it->second.size() != 0) ? it->second[0] : default_val);
}

Real RBParameters::get_extra_step_value(const std::string & param_name, std::size_t step) const
{
  const auto & vec = libmesh_map_find(_extra_parameters, param_name);
  libmesh_error_msg_if(step >= vec.size(), "Error getting value for parameter " << param_name);
  return vec[step];
}

Real RBParameters::get_extra_step_value(const std::string & param_name, std::size_t step, const Real & default_val) const
{
  // same as get_step_value(param_name, index, default_val) but for the map of extra parameters
  auto it = _extra_parameters.find(param_name);
  return ((it != _extra_parameters.end() && step < it->second.size()) ? it->second[step] : default_val);
}

void RBParameters::set_extra_value(const std::string & param_name, Real value)
{
  // Same as set_value(param_name, value) but for the map of extra parameters
  _extra_parameters[param_name] = {value};
}

unsigned int RBParameters::n_parameters() const
{
  return cast_int<unsigned int>(_parameters.size());
}

void RBParameters::set_n_steps(unsigned int n_steps)
{
  _n_steps = n_steps;
}

unsigned int RBParameters::n_steps() const
{
  // Quick return if there are no parameters
  if (_parameters.empty())
    return _n_steps;

  // If _parameters is not empty, we can check the number of steps in the first param
  auto size_first = _parameters.begin()->second.size();

#ifdef DEBUG
  // In debug mode, verify that all parameters have the same number of steps
  for (const auto & pr : _parameters)
    libmesh_assert_msg(pr.second.size() == size_first, "All parameters must have the same number of steps.");
#endif

  // If we made it here in DEBUG mode, then all parameters were
  // verified to have the same number of steps.
  return size_first;
}

void RBParameters::get_parameter_names(std::set<std::string> & param_names) const
{
  param_names.clear();
  for (const auto & pr : _parameters)
    param_names.insert(pr.first);
}

void RBParameters::get_extra_parameter_names(std::set<std::string> & param_names) const
{
  param_names.clear();
  for (const auto & pr : _extra_parameters)
    param_names.insert(pr.first);
}

void RBParameters::erase_parameter(const std::string & param_name)
{
  _parameters.erase(param_name);
}

void RBParameters::erase_extra_parameter(const std::string & param_name)
{
  _extra_parameters.erase(param_name);
}

RBParameters::const_iterator RBParameters::begin() const
{
  return const_iterator(_parameters.begin(), 0);
}

RBParameters::const_iterator RBParameters::end() const
{
  // Note: the index 0 is irrelevant here since _parameters.end() does
  // not refer to a valid vector entry in the map.
  return const_iterator(_parameters.end(), 0);
}

RBParameters::const_iterator RBParameters::extra_begin() const
{
  return const_iterator(_extra_parameters.begin(), 0);
}

RBParameters::const_iterator RBParameters::extra_end() const
{
  // Note: the index 0 is irrelevant here since _parameters.end() does
  // not refer to a valid vector entry in the map.
  return const_iterator(_extra_parameters.end(), 0);
}

bool RBParameters::operator==(const RBParameters & rhs) const
{
  return (this->_parameters == rhs._parameters &&
          this->_extra_parameters == rhs._extra_parameters);
}

bool RBParameters::operator!=(const RBParameters & rhs) const
{
  return !(*this == rhs);
}

RBParameters & RBParameters::operator+= (const RBParameters & rhs)
{
  libmesh_error_msg_if(this->n_steps() != rhs.n_steps(),
                       "Can only append RBParameters objects with matching numbers of steps");

  // Overwrite or add each (key, vec) pair in rhs to *this.
  for (const auto & [key, vec] : rhs._parameters)
    _parameters[key] = vec;
  for (const auto & [key, vec] : rhs._extra_parameters)
    _extra_parameters[key] = vec;

  return *this;
}

std::string RBParameters::get_string(unsigned int precision) const
{
  std::stringstream param_stringstream;
  param_stringstream << std::setprecision(precision) << std::scientific;

  for (const auto & [key, vec] : _parameters)
  {
    param_stringstream << key << ": ";

    // Write comma separated list of values for each param_name
    std::string separator = "";
    for (const auto & val : vec)
    {
      param_stringstream << separator << val;
      separator = ", ";
    }
    param_stringstream << std::endl;
  }

  return param_stringstream.str();
}

void RBParameters::print() const
{
  libMesh::out << get_string();
}

}
