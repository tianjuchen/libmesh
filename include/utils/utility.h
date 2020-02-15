// The libMesh Finite Element Library.
// Copyright (C) 2002-2019 Benjamin S. Kirk, John W. Peterson, Roy H. Stogner

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#ifndef LIBMESH_UTILITY_H
#define LIBMESH_UTILITY_H

// Local includes
#include "libmesh/libmesh_common.h" // for Real

// System includes
#include <string>
#include <vector>
#include <algorithm> // for std::lower_bound

namespace libMesh
{

/**
 * Encapsulates the common "get value from map, otherwise error"
 * idiom, which is similar to calling map.at(), but gives a more
 * useful error message with a line number.
 */
#define libmesh_map_find(map, key) libMesh::Utility::map_find((map), (key), __FILE__, __LINE__)

/**
 * Encapsulates the common "get value from vector, otherwise error"
 * idiom, which is similar to calling vec.at(), but gives a more
 * useful error message with a line number.
 */
#define libmesh_vector_at(vec, idx) libMesh::Utility::vector_at((vec), (idx), __FILE__, __LINE__)

// ------------------------------------------------------------
// The Utility namespace is for functions
// which are useful but don't necessarily belong anywhere else.

namespace Utility
{

/**
 * \returns A string containing information about the system you are
 * running on.
 */
std::string system_info();

/**
 * Helper struct for enabling template metaprogramming/SFINAE.
 */
template <typename T>
class is_streamable
{
  template <typename U> // must be template to get SFINAE fall-through...
  static auto test(const U* u) -> decltype(std::cout << *u);

  static auto test(...) -> std::false_type;

public:
  enum { value = !std::is_same<decltype(test((T*)0)), std::false_type>::value };
};

/**
 * This function should not be called directly (although it can be),
 * instead see the libmesh_map_find() macro.
 *
 * Calls find(key), and checks the result against end(). Returns the
 * corresponding value if found, throws an error otherwise. Templated
 * on the type of map, so this will work with both std::map and
 * std::unordered_map.
 */
template<typename Map, typename Key,
         typename std::enable_if<!is_streamable<Key>::value, Key>::type* = nullptr>
inline
typename Map::mapped_type &
map_find(Map & map,
         const Key & key,
         const char * filename,
         int line_number)
{
  auto it = map.find(key);
  if (it == map.end())
    libmesh_error_msg("map_find() error: key not found in file "        \
                      << filename << " on line " << line_number);
  return it->second;
}

/**
 * A version of the function above that works for const objects.
 */
template<typename Map, typename Key,
         typename std::enable_if<!is_streamable<Key>::value, Key>::type* = nullptr>
inline
const typename Map::mapped_type &
map_find(const Map & map,
         const Key & key,
         const char * filename,
         int line_number)
{
  auto it = map.find(key);
  if (it == map.end())
    libmesh_error_msg("map_find() error: key not found in file "        \
                      << filename << " on line " << line_number);
  return it->second;
}

/**
 * A version of the map_find() utility which can only be used if
 * the map key is printable via std::stream.
 */
template<typename Map, typename Key,
         typename std::enable_if<is_streamable<Key>::value, Key>::type* = nullptr>
inline
typename Map::mapped_type &
map_find(Map & map,
         const Key & key,
         const char * filename,
         int line_number)
{
  auto it = map.find(key);
  if (it == map.end())
    libmesh_error_msg("map_find() error: key \"" << key << "\" not found in file " \
                      << filename << " on line " << line_number);
  return it->second;
}

/**
 * A version of the function above that works for const objects.
 */
template<typename Map, typename Key,
         typename std::enable_if<is_streamable<Key>::value, Key>::type* = nullptr>
inline
const typename Map::mapped_type &
map_find(const Map & map,
         const Key & key,
         const char * filename,
         int line_number)
{
  auto it = map.find(key);
  if (it == map.end())
    libmesh_error_msg("map_find() error: key \"" << key << "\" not found in file " \
                      << filename << " on line " << line_number);
  return it->second;
}


/**
 * A replacement for std::vector::at(i) which is meant to be used with
 * a macro, and, unlike at(), gives a proper line number and useful
 * error message when the index is past the end.
 */
template<typename Vector>
inline
typename Vector::reference &
vector_at(Vector & vec,
          typename Vector::size_type i,
          const char * filename,
          int line_number)
{
  if (i >= vec.size())
    libmesh_error_msg("vec_at() error: Index " << i <<
                      " past end of vector in file " << filename <<
                      " on line " << line_number);
  return vec[i];
}

/**
 * Same as above, but for const inputs.
 */
template<typename Vector>
inline
typename Vector::const_reference &
vector_at(const Vector & vec,
          typename Vector::size_type i,
          const char * filename,
          int line_number)
{
  if (i >= vec.size())
    libmesh_error_msg("vec_at() error: Index " << i <<
                      " past end of vector in file " << filename <<
                      " on line " << line_number);
  return vec[i];
}

/**
 * \p Utility::iota is a duplication of the SGI STL extension
 * \p std::iota.  It simply assigns sequentially increasing values
 * to a range. That is, it assigns \p value to \p *first, \p value + 1
 * to \p *(first + 1) and so on. In general, each iterator \p i in the
 * range [first, last) is assigned \p value + (i - \p first).
 */
template <typename ForwardIter, typename T>
void iota (ForwardIter first, ForwardIter last, T value)
{
  // Use std::iota instead!
  libmesh_deprecated();

  while (first != last)
    {
      *first = value++;
      ++first;
    }
}


/**
 * Utility::is_sorted mimics the behavior of the SGI STL extension
 * std::is_sorted.  Checks to see if the range [first,last) is
 * sorted in non-decreasing order, ie. for each "i" in
 * [first,last) *i <= *(i+1).
 */
template<class InputIterator >
bool is_sorted(InputIterator first, InputIterator last)
{
  if (first == last)
    return true;

  // "prev" always points to the entry just to the left of "first"
  //  [-    -    -    -    -    -]
  //   ^    ^
  // prev first
  //
  //  [-    -    -    -    -    -]
  //        ^    ^
  //      prev first
  //
  //  [-    -    -    -    -    -]
  //             ^    ^
  //           prev first
  InputIterator prev( first );
  for (++first; first != last; ++prev, ++first)
    if (*first < *prev)    // Note: this is the same as *prev > *first,
      return false;        // but we only require op< to be defined.

  // If we haven't returned yet, it's sorted!
  return true;


  // A one-liner version using adjacent_find.  This doesn't work for
  // C-style arrays, since their pointers do not have a value_type.
  //
  // Works by checking to see if adjacent entries satisfy *i >
  // *(i+1) and returns the first one which does.  If "last" is
  // returned, no such pair was found, and therefore the range must
  // be in non-decreasing order.
  //
  // return (last ==
  // std::adjacent_find(first, last,
  // std::greater<typename InputIterator::value_type >()));

  // A second one-linear attempt.  This one checks for a **strictly
  // increasing** (no duplicate entries) range.  Also doesn't work
  // with C-style arrays.
  //
  // return (last ==
  // std::adjacent_find(first, last,
  // std::not2(std::less<typename InputIterator::value_type>())));
}


/**
 * The STL provides \p std::binary_search() which returns \p true or
 * \p false depending on whether the searched-for value is found.  In
 * contrast, Utility::binary_find() uses a std::lower_bound() based
 * search on a sorted range to find the required value.
 *
 * \returns An iterator to the searched-for element, or "last" if the
 * element is not found.
 */
template<class ForwardIterator, class T>
ForwardIterator binary_find(ForwardIterator first, ForwardIterator last, const T & value)
{
  ForwardIterator it = std::lower_bound(first, last, value);
  return (it == last || value < *it) ? last : it;
}

/**
 * As above, but takes a custom comparison object.
 */
template<class ForwardIterator, class T, class Compare>
ForwardIterator binary_find(ForwardIterator first, ForwardIterator last, const T & value, Compare comp)
{
  ForwardIterator it = std::lower_bound(first, last, value, comp);
  return (it == last || comp(value,*it)) ? last : it;
}


/**
 * An efficient template instantiation for raising
 * to an arbitrary integer power.
 */
template <int N, typename T>
struct do_pow {
  static inline T apply (const T & x)
  {
    libmesh_assert(N>1);

    if (N%2) // odd exponent
      return x * do_pow<N-1,T>::apply(x);

    const T xNover2 = do_pow<N/2,T>::apply(x);

    return xNover2*xNover2;
  }
};

// An efficient compiler would distill N=6 down to 3
// multiplications, but an inefficient one (or a complicated
// T::operator*) might do worse, so we'll specialize here.
template <typename T>
struct do_pow<6,T> {
  static inline T apply (const T & x)
  {
    const T x2 = x*x,
      x4 = x2*x2;

    return x4*x2;
  }
};

template <typename T>
struct do_pow<1,T> {
  static inline T apply (const T & x) { return x; }
};

template <typename T>
struct do_pow<0,T> {
  static inline T apply (const T &) { return 1; }
};


template <int N, typename T>
inline
T pow(const T & x)
{
  return do_pow<N,T>::apply(x);
}

/**
 * A simple implementation of the factorial.
 */
inline
unsigned int factorial(unsigned int n)
{

  unsigned int factorial_n = 1;

  if (n==0)
    return factorial_n;

  for (unsigned int i=1; i<n; i++)
    factorial_n *= i+1;

  return factorial_n;
}


// Simple function to compute "n choose k", aka the binomial coefficient.
template <typename T>
T binomial(T n, T k)
{
  T ret = 1;

  // Binomial function is "symmetric" in k, C(n, k) = C(n, n-k).
  if (k > n - k)
    k = n - k;

  // Compute n * (n-1) * ... * (n-k+1) / (k * (k-1) * ... * 1)
  for (T i = 0; i < k; ++i)
    {
      ret *= (n - i);
      ret /= (i + 1);
    }

  return ret;
}


/**
 * A convenient method to truly empty a vector using the "swap trick"
 */
template <typename T>
void deallocate (std::vector<T> & vec)
{
  std::vector<T>().swap(vec);
}


// Utility functions useful when dealing with complex numbers.

#ifdef LIBMESH_USE_COMPLEX_NUMBERS

/**
 * \returns For \p r_o_c = 0 the filename for output of the real part
 * of complex data, and for  \p r_o_c = 1 the filename for the imaginary
 * part.
 */
std::string complex_filename (const std::string & basename,
                              unsigned int r_o_c=0);

/**
 * Prepare complex data for writing.
 */
void prepare_complex_data (const std::vector<Complex> & source,
                           std::vector<Real> & real_part,
                           std::vector<Real> & imag_part);

#endif // #ifdef LIBMESH_USE_COMPLEX_NUMBERS


/**
 * Create a directory.
 */
int mkdir(const char* pathname);


/**
 * This Functor simply takes an object and reverses its byte
 * representation.  This is useful for changing endian-ness
 * for file IO.  This class has been tested on x86 architectures
 * with 4-byte words.
 *
 *
 */
class ReverseBytes
{
public:

  /**
   * Constructor.  Takes a bool, determines if we will actually
   * do byte reversing.
   */
  explicit
  ReverseBytes (const bool dr);

  /**
   * Functor.  Takes the data to reverse and performs the
   * byte-ordering reversal.
   */
  template <typename T>
  T operator () (T & data) const;

private:

  /**
   * \returns The value of the reverse flag.
   */
  bool reverse () const { return _do_reverse; }

  /**
   * flag
   */
  const bool _do_reverse;
};



// ReverseBytes inline members
inline
ReverseBytes::ReverseBytes (const bool rb) :
  _do_reverse (rb)
{}


template <typename T>
inline
T ReverseBytes::operator() (T & data) const
{
  // Possibly reverse the byte ordering
  if (this->reverse())
    {
      unsigned char * b = (unsigned char *) &data;

      int i=0;
      int j=(sizeof(T) - 1);

      while (i < j)
        {
          std::swap (b[i], b[j]);
          i++; j--;
        }
    }

  return data;
}


}

} // namespace libMesh

#endif // LIBMESH_UTILITY_H
