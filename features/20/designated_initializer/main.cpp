#include <vector>
#include <iostream>

struct Point3D {
   float x = 42.f;
   float y = 42.f;
   float z = 42.f;
};

std::ostream& operator<<(std::ostream& os, Point3D const& p){
   return os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
}

class Mesh {
   std::vector<Point3D> m_points = {};

public:
   void add_point(Point3D && p){
      m_points.push_back(std::move(p));
   }

   void dump(){
      for(auto const & p : m_points){
         std::cout << p << std::endl;
      }   
   }
};



int main(int argc, char **argv){
   Mesh mesh;

   mesh.add_point({.x = 0.f, .y = 0.f, .z = 0.f});
   mesh.add_point({.x = 1.f, .y = 2.f, .z = 1.f});
   mesh.add_point({.x = 3.f});
   mesh.add_point({.x = 5.f, .z = 5.f});
   mesh.add_point({.y = 7.f, .z = 8.f});
   mesh.add_point({.z = 4.f});

   mesh.dump();

   return 0;
}
