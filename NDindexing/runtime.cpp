#include <iostream>
#include <vector>
#include <utility>
#include <cassert>
#include <stdexcept>
#include <sstream>

/// @brief Stores n dimensional coordinate system. 
class NDim {
   std::vector<unsigned int> m_dim_vec;
public:
   NDim() = default;
   
   /// @brief Create n dimensional coordinate system from a initializer list, like a std::vector.
   /// @param dim The largest dimension is stored left and the smallest left. 
   NDim(std::initializer_list<unsigned int> dim) : m_dim_vec{dim} {}
   
   /// @brief Create n dimensional coordinate system and initialize each dim with the same value.
   /// @param dim Number of dimensions.
   /// @param initial_value Initial value.
   NDim(unsigned int dim, unsigned int initial_value) : m_dim_vec(dim, initial_value) {}


   /// @brief Return size of dimensions. Does boundary checks.
   /// @param index Dimension.
   /// @return Return size of dimensions.
   unsigned int & at(unsigned int index){
      assert(index > 0);
      assert(index <= m_dim_vec.size());
      return m_dim_vec.at(m_dim_vec.size() - index);
   }

   unsigned int at(unsigned int index) const{
      assert(index > 0);
      assert(index <= m_dim_vec.size());
      return m_dim_vec.at(m_dim_vec.size() - index);
   }

   unsigned int get_dim() const {
      return m_dim_vec.size();
   }

   /// @brief Multiply the number of each dimension.
   /// @return Total number of elements
   unsigned int get_total_elements() const {
      unsigned int total_elements = 1;
      for(unsigned int elements : m_dim_vec){
         total_elements *= elements;
      }
      return total_elements;
   }

};

std::ostream & operator<<(std::ostream & os, NDim const & nDim){
   os << "[";
   for(auto dim = nDim.get_dim(); dim > 0; --dim){
      os << nDim.at(dim);
      if (dim > 1){
         os << " ";
      }
   }
   os << "]";
   return os;
}

void print_mapping(std::vector<std::pair<NDim, unsigned int>> const & mapping){
   for(auto [multi_index, linear_index] : mapping){
      std::cout << multi_index << " -> " << linear_index << std::endl;
   }
}

void print_mapping(std::vector<std::pair<unsigned int, NDim>> const & mapping){
   for(auto [linear_index, multi_index] : mapping){
      std::cout << linear_index << " -> " << multi_index << std::endl;
   }
}

// ################################################################################################
// ### transform multi dimensional index -> linear index
// ################################################################################################

// The formula to calculate the linear index from a n dimensional index is:
// Index = xn ( D{n-1} * ... * D1  ) + x{n-1} ( D{n-2} * ... * D1 ) + ... + x2 * D1 + x1
// where n is the highest dimensions and 1 the smallest
// source: https://stackoverflow.com/questions/29142417/4d-position-from-1d-index

unsigned int get_linear_index_impl(NDim const & nDposition, NDim const & nDdimsize, unsigned int const current_dim){
   if(current_dim > 1){
      // this is xn
      unsigned int partial_index = nDposition.at(current_dim);
      // this part calculate the term: ( D{n-1} * ... * D1  )
      for(unsigned int d = current_dim - 1; // starts with a dimension smaller than the current
          d > 0; // abort, if we are in the sequential part
          --d){
         // xn * ( D{n-1} * ... * D1  )
         partial_index *= nDdimsize.at(d);
      }

      // add the current part with the parts of the smaller dimensions
      return partial_index + get_linear_index_impl(nDposition, nDdimsize, current_dim -1); 
   } else {
      return nDposition.at(1);
   }
}

/// @brief Calculate the linear index from an n dimensional index.
/// @param nDposition Position in the in the n dimensional index.
/// @param nDdimsize Size of each dimension.
/// @return Linear index.
unsigned int get_linear_index(NDim const & nDposition, NDim const & nDdimsize){
   return get_linear_index_impl(nDposition, nDdimsize, nDdimsize.get_dim());
}

/// @brief Helper function to iterate over each possible position in a multi dimensional index and maps the 
///        position to a linear index.
/// @param nDposition Position in multi dimensional index.
/// @param nDdimsize Dimension sizes of the multi dimensional index.
/// @param mappingNDto1D Stores mappings from multi dimensional coordinate to linear coordinate.
/// @param access_counter Counts, how many indices was calculated. Only for verification.
/// @param current_dim The current dimension.
void iterate_over_dim_impl(NDim & nDposition,
                 NDim const & nDdimsize,   
                 std::vector<std::pair<NDim, unsigned int>> & mappingNDto1D, 
                 unsigned int & access_counter,
                 unsigned int const current_dim){
   if(current_dim > 0){
      
      for(unsigned int dim_index = 0; dim_index < nDdimsize.at(current_dim); ++dim_index){
         nDposition.at(current_dim) = dim_index;
         iterate_over_dim_impl(nDposition, nDdimsize, mappingNDto1D, access_counter, current_dim-1);
      }
   } else {
      mappingNDto1D[access_counter] = std::pair<NDim, unsigned int>(nDposition, get_linear_index(nDposition, nDdimsize));
      ++access_counter;
   }
}

void iterate_over_dim_impl(NDim & nDposition,
                 NDim const & nDdimsize,
                 std::vector<std::pair<NDim, unsigned int>> & mappingNDto1D, 
                 unsigned int & access_counter){
   iterate_over_dim_impl(nDposition, nDdimsize, mappingNDto1D, access_counter, nDposition.get_dim());
}

/// @brief Iterate over each possible position in a multi dimensional index and maps the 
///        position to a linear index.
/// @param nDdimsize Dimension sizes of the multi dimensional index.
/// @return Mapping mappings from multi dimensional coordinate to linear coordinate.
std::vector<std::pair<NDim, unsigned int>> to1D(NDim nDdimsize){
   assert(nDdimsize.get_dim() > 0 && "Zero dimensions are not allowed.");

   unsigned int total_elements = 1;

   std::cout << "dimensions: ";
   for(unsigned int dim = nDdimsize.get_dim(); dim > 0; --dim){
      total_elements *= nDdimsize.at(dim);
      std::cout << nDdimsize.at(dim) << ", ";
   }
   std::cout << std::endl;
   
   std::cout << "total elements: " << total_elements << std::endl;

   // use external counter to count how many index calculations happened. In the end, needs to be equal to total_elements 
   unsigned int access_counter = 0;
   NDim nDposition(nDdimsize.get_dim(), 0);
   std::vector<std::pair<NDim, unsigned int>> mappingNDto1D(total_elements);
   
   iterate_over_dim_impl(nDposition, nDdimsize, mappingNDto1D, access_counter);

   if (access_counter != total_elements){ 
      std::stringstream ss;
      ss << "Does not create enough linear index positions\n";
      ss << "access_counter: " << access_counter << "\n";
      ss << "total_elements: " << total_elements << "\n";
      throw std::runtime_error(ss.str());
   }
   return mappingNDto1D;
}

// ################################################################################################
// ### transform linear index -> multi dimensional index
// ################################################################################################

// The formula to calculate the multi dimensional index from a linear index is:
// xn = ( ( Index - Index( x1, ..., x{n-1} ) ) / Product( D1, ..., D{N-1} ) ) % Dn
// With following cases:
// x1 = Index % D1;
// x2 = ( ( Index - Index(x1) ) / D1 ) %  D2;
// xn  = ( ( Index(x{n-1}) - (xn * D{n-1} * ... * D1 ) ) / D1 * ... * DN ) %  DN;
// source: https://stackoverflow.com/questions/29142417/4d-position-from-1d-index


// Resolved for 4 Dim:
// x1 = Index % D1;
// x2 = ( ( Index - x1 ) / D1 ) %  D2;
// x3 = ( ( Index - x2 * D1 - x1 ) / (D1 * D2) ) % D3; 
// x4 = ( ( Index - x3 * D2 * D1 - x2 * D1 - x1 ) / (D1 * D2 * D3) ) % D4;

// The implementation starts with the index of smallest dimensions 
// It uses two performance optimizations
// 1. Take the dividend of the dimension below and extended it by a subtrahend
// 2. Take the divisor of the dimension below and multiplied with the dimension size of the current dim

/// @brief Calculate the index position of each dimension from a linear index.
/// @param linear_index The linear index.
/// @param nDdimsize Size of each dimension.
/// @param nDposition Stores the index of each in index here.
/// @param current_dim The current dimension.
/// @param dividend Pass the dividend of the predecessor dimension for performance optimization. 
/// @param divisor Pass the divisor of the predecessor dimension for performance optimization.
void get_multi_index_impl(unsigned int linear_index, NDim const & nDdimsize, NDim & nDposition, unsigned int const current_dim, unsigned int dividend, unsigned int divisor){
   // special case smallest dimension.
   if (current_dim == 1){
      nDposition.at(1) = (linear_index % nDdimsize.at(1));
      // divisor = 1 is a neutral element
      get_multi_index_impl(linear_index, nDdimsize, nDposition, current_dim + 1, linear_index, 1);
   } else if (current_dim <= nDdimsize.get_dim()){
      
      // ### dividend part
      unsigned int part_dividend = nDposition.at(current_dim-1);
      // the special case for x2 is here handled, because dim is smaller than 1, so no multiplication will happen
      for(unsigned int dim = current_dim -1; dim > 1; --dim){
         part_dividend *= nDdimsize.at(dim);
      }
      dividend -= part_dividend;
      // ### dividend part

      // ### divisor part
      divisor *= nDdimsize.at(current_dim-1);
      // ### divisor part

      nDposition.at(current_dim) = (dividend / divisor) % nDdimsize.at(current_dim);

      get_multi_index_impl(linear_index, nDdimsize, nDposition, current_dim + 1, dividend, divisor);
   }
}

/// @brief Calculate the multi dimensional index from a linear index.
/// @param linear_index The linear index
/// @param nDdimsize Size of each dimension.
/// @return multi dimensional index
NDim get_multi_index(unsigned int linear_index, NDim const & nDdimsize){
   NDim nDposition(nDdimsize.get_dim(), 0);
   get_multi_index_impl(linear_index, nDdimsize, nDposition , 1, 0, 0);
   return nDposition;
}

/// @brief Maps all possible linear index to multi dimensional indices in the index room of nDdimsize.
/// @param nDdimsize Dimension sizes of the multi dimensional index.
/// @return Mapping mappings from linear coordinate to multi dimensional coordinate.
std::vector<std::pair<unsigned int, NDim>> toND(NDim nDdimsize){
   unsigned int total_elements = nDdimsize.get_total_elements();

   std::vector<std::pair<unsigned int, NDim>> mapping(total_elements);

   for(unsigned int i = 0; i < total_elements; ++i){
      mapping[i] = std::pair(i, get_multi_index(i, nDdimsize));
   }
   
   return mapping;
}

int main(int argc, char **argv){
   auto nToLin_1Dto1D = to1D({2});
   print_mapping(nToLin_1Dto1D);

   std::cout << std::endl;
   auto nToLin_2Dto1D = to1D({2, 3});
   print_mapping(nToLin_2Dto1D);

   std::cout << std::endl;
   auto nToLin_3Dto1D = to1D({2, 3, 5});
   print_mapping(nToLin_3Dto1D);

   std::cout << std::endl;
   auto nToLin_4Dto1D = to1D({2, 4, 3, 5});
   print_mapping(nToLin_4Dto1D);

   std::cout << std::endl;
   auto linToN_1Dto4D = toND({2, 4, 3, 5});
   print_mapping(linToN_1Dto4D);

   return 0;
}
