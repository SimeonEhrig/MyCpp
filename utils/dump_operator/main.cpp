#include "helper.hpp"

#include <iostream>
#include <vector>

struct S_Member {
   D d;
};

struct S_Const_Member {
   D const d;
};

int main(int argc, char **argv){
   {
      D d("move obj");
      D d_move = std::move(d); 
   }
   std::cout << std::endl;

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
