#include <iostream>
#include <ostream>
#include <vector>

template<typename TData>
class MyVector : public std::vector<TData> {
public:
   MyVector(std::initializer_list<TData> list) : std::vector<TData>(list) {}
   MyVector(MyVector const & other) : std::vector<TData>(other) {}

   MyVector<TData> merge(MyVector<TData> const & other) const {
      MyVector<TData> merged(*this);
      merged.reserve(merged.size() + other.size());
      std::copy(other.begin(), other.end(), std::back_inserter(merged));
      return merged;
   }
};

template<typename TData>
MyVector<TData> operator<<(MyVector<TData> const & v1, MyVector<TData> const & v2){
   return v1.merge(v2);
}

template<typename T>
std::ostream & operator<<(std::ostream & os, MyVector<T> const & vec){
   os << "[";
   for(auto i = 0; i < vec.size(); ++i){
      os << vec[i];
      if (i != vec.size() - 1){
         os << ", ";
      }
   }
   os << "]";
   return os;
}

int main(int argc, char **argv){
   MyVector<int> v1{1, 2, 3};
   MyVector<int> v2{4, 5, 6};
   MyVector<int> v3{7, 8, 9};

   std::cout << "v1.merge(v2.merge(v3)): " << v1.merge(v2.merge(v3)) << std::endl;
   std::cout << "v1 << v2 << v3: " << v1<< v2 << v3 << std::endl;
   std::cout << "(v1 << v2 << v3): " << (v1<< v2 << v3) << std::endl;
   auto v123 = v1<< v2 << v3;
   std::cout << "v123: " << v123 << std::endl;

   std::cout << std::endl;

   std::cout << "v3.merge(v1.merge(v2)): " << v3.merge(v1.merge(v2)) << std::endl;
   std::cout << "v3 << v1 << v2: " << v3 << v1 << v2 << std::endl;
   std::cout << "(v3 << v1 << v2): " << (v3 << v1 << v2) << std::endl;
   auto v312 = v1<< v2 << v3;
   std::cout << "v312: " << v312 << std::endl;

   return 0;
}
