#include <iostream>
#include <string>
#include <vector>
#include <utility>

struct D{
   std::string name = "";

   D() { 
      std::cout << name << ": D()" << std::endl; 
   }

   D(std::string_view const name) : name(name) { 
      std::cout << name << ": D(std::string_view const name)" << std::endl; 
   }

   D(D const & other) : name(other.name) 
   { 
      std::cout << name << ": D(D const &)" << std::endl; 
   }

   D(D&& other) noexcept : name(std::exchange(other.name, "MOVED " + other.name)) 
   { 
      std::cout << name << ": D(D&&)" << std::endl; 
   }

   ~D(){ 
      std::cout << name << ": ~D()" << std::endl; 
   }

   D &operator=(D const & other) 
   { 
      if(&other == this){
         return *this;
      }

      name = other.name;
      std::cout << name << ": operator=(D & const)" << std::endl; 
      return *this;
   }
   
   D &operator=(D && other) noexcept { 
      if(&other == this){
         return *this;
      }
      
      name = std::exchange(other.name, "MOVED " + other.name);
      std::cout << name << ": operator=(D &&)" << std::endl;
      return *this;
   }
};

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
