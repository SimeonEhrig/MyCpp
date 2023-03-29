#include <experimental/mdspan>
#include <vector>
#include <iostream>
#include <cassert>

template<typename T>
std::ostream & operator<<(std::ostream & os, std::vector<T> const & vec){
   os << "[";
   for(auto i = 0; i < vec.size(); ++i){
      os << vec[i];
      if (i < vec.size()-1){
         os << " ";
      }
   }
   os << "]";
   return os;
}

int main(int argc, char **argv){
   namespace stdex = std::experimental;

   std::size_t constexpr z_size = 4;
   std::size_t constexpr y_size = 5;
   std::size_t constexpr x_size = 2;

   std::size_t constexpr total_elements = z_size * y_size * x_size;

   std::vector<int> data(total_elements ,0);

   stdex::mdspan mdspan3D{data.data(), stdex::extents{z_size, y_size, x_size}};
   stdex::mdspan mdspan1D{data.data(), stdex::extents{total_elements}};

   #if MDSPAN_USE_BRACKET_OPERATOR
      std::cout << "use multi dimensional access operator[] (supported in C++23)." << std::endl;
   #else
      std::cout << "use operator()." << std::endl;
   #endif

   int access_counter = 0;
   for(auto z = 0; z < mdspan3D.extent(0); ++z){
      for(auto y = 0; y < mdspan3D.extent(1); ++y){
         for(auto x = 0; x < mdspan3D.extent(2); ++x){
            #if MDSPAN_USE_BRACKET_OPERATOR
               mdspan3D[z, y, x] = access_counter++;
            #else
               mdspan3D(z, y, x) = access_counter++;
            #endif
         }
      }
   }

   assert( mdspan1D.extent(0) == total_elements);

   bool are_equal = true;
   for(auto i = 0; i < total_elements; ++i){
      if(mdspan1D[i] != i) {
         are_equal = false;
      }
   }

   if(are_equal){
      std::cout << "mdspan3D maps to the same linear index like mdspan1D" << std::endl;
   } else {
      std::cout << "mdspan3D maps to a different linear index like mdspan1D" << std::endl;
   }

   std::cout << data << std::endl;
   return 0;
}
