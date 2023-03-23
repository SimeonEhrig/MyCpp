#include <iostream>
#include <string>
#include <vector>

struct D{
   D() { std::cout << "D()" << std::endl; }
   D(D const &) { std::cout << "D(D const &)" << std::endl; }
   D(D&&) noexcept { std::cout << "D(D&&)" << std::endl; }
   ~D(){ std::cout << "~D()" << std::endl; }
   D &operator=(D const &) { std::cout << "operator=(D & const)" << std::endl; return *this;}
   D &operator=(D &&) noexcept { std::cout << "operator=(D &&)" << std::endl; return *this;}
};

struct S_Member {
   D d;
};

struct S_Const_Member {
   D const d;
};

int main(int argc, char **argv){
   std::cout << ">>>>>>>> S_Member >>>>>>>>" << std::endl;
   {
      std::vector<S_Member> s_members;
      s_members.emplace_back();
      s_members.emplace_back();
   }
   std::cout << "<<<<<<<< S_Member <<<<<<<<" << std::endl;

   std::cout << std::endl;

   std::cout << ">>>>>>>> S_Const_Member >>>>>>>>" << std::endl;
   {
      std::vector<S_Const_Member> s_const_members;
      s_const_members.emplace_back();
      s_const_members.emplace_back();
   }
   std::cout << "<<<<<<<< S_Const_Member <<<<<<<<" << std::endl;

   return 0;
}
