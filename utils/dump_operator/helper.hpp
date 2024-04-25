#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <limits>
#include <sstream>

struct D{
   inline static std::size_t counter = 0;

   std::string name = "";
   std::size_t id = std::numeric_limits<std::size_t>::max();

   std::string build_name(){
    std::stringstream ss;
    ss << "[" << id << "] " << name;
    return ss.str();
   }

   

   D() : name("empty") {
      id = counter++; 
      std::cout << build_name() << ": D()" << std::endl; 
   }

   D(std::string_view const n) : name(n) { 
      id = counter++; 
      std::cout << build_name() << ": D(std::string_view const name)" << std::endl; 
   }

   D(D const & other) : name(other.name) 
   { 
      id = counter++; 
      std::cout << build_name() << ": D(D const &)" << std::endl; 
   }

   D(D&& other) noexcept : name(std::exchange(other.name, "MOVED " + other.name)) 
   { 
      id = counter++; 
      std::cout << build_name() << ": D(D&&)" << std::endl; 
   }

   ~D(){ 
      std::cout << build_name() << ": ~D()" << std::endl; 
   }

   D &operator=(D const & other) 
   { 
      if(&other == this){
         return *this;
      }

      name = other.name;
      std::cout << build_name() << ": operator=(D & const)" << std::endl; 
      return *this;
   }
   
   D &operator=(D && other) noexcept { 
      if(&other == this){
         return *this;
      }
      
      name = std::exchange(other.name, "MOVED " + other.name);
      std::cout << build_name() << ": operator=(D &&)" << std::endl;
      return *this;
   }
};
