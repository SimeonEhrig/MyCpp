# About

Playing around with the static and dynamic ranks of `std::experimental::extents`.

# Output

```
std::numeric_limits<std::size_t>::max() == stdex::dynamic_extent : true

fullyDynamic3D.rank(): 3
fullyDynamic3D.rank_dynamic(): 3
fullyDynamic3D calculated static ranks: 0
 fullyDynamic3D.extent(0): 4
 fullyDynamic3D.static_extent(0): 18446744073709551615
 fullyDynamic3D.static_extent(0) == stdex::dynamic_extent : true
   fullyDynamic3D.extent(1): 5
   fullyDynamic3D.static_extent(1): 18446744073709551615
   fullyDynamic3D.static_extent(1) == stdex::dynamic_extent : true
     fullyDynamic3D.extent(2): 2
     fullyDynamic3D.static_extent(2): 18446744073709551615
     fullyDynamic3D.static_extent(2) == stdex::dynamic_extent : true


fullyStatic3D.rank(): 3
fullyStatic3D.rank_dynamic(): 0
fullyStatic3D calculated static ranks: 3
 fullyStatic3D.extent(0): 4
 fullyStatic3D.static_extent(0): 4
 fullyStatic3D.static_extent(0) == stdex::dynamic_extent : false
   fullyStatic3D.extent(1): 5
   fullyStatic3D.static_extent(1): 5
   fullyStatic3D.static_extent(1) == stdex::dynamic_extent : false
     fullyStatic3D.extent(2): 2
     fullyStatic3D.static_extent(2): 2
     fullyStatic3D.static_extent(2) == stdex::dynamic_extent : false

mixedStaticDynamic3D_1.rank(): 3
mixedStaticDynamic3D_1.rank_dynamic(): 1
mixedStaticDynamic3D_1 calculated static ranks: 2
 mixedStaticDynamic3D_1.extent(0): 4
 mixedStaticDynamic3D_1.static_extent(0): 4
 mixedStaticDynamic3D_1.static_extent(0) == stdex::dynamic_extent : false
   mixedStaticDynamic3D_1.extent(1): 5
   mixedStaticDynamic3D_1.static_extent(1): 18446744073709551615
   mixedStaticDynamic3D_1.static_extent(1) == stdex::dynamic_extent : true
     mixedStaticDynamic3D_1.extent(2): 2
     mixedStaticDynamic3D_1.static_extent(2): 2
     mixedStaticDynamic3D_1.static_extent(2) == stdex::dynamic_extent : false

mixedStaticDynamic3D_2.rank(): 3
mixedStaticDynamic3D_2.rank_dynamic(): 2
mixedStaticDynamic3D_2 calculated static ranks: 1
 mixedStaticDynamic3D_2.extent(0): 4
 mixedStaticDynamic3D_2.static_extent(0): 18446744073709551615
 mixedStaticDynamic3D_2.static_extent(0) == stdex::dynamic_extent : true
   mixedStaticDynamic3D_2.extent(1): 5
   mixedStaticDynamic3D_2.static_extent(1): 5
   mixedStaticDynamic3D_2.static_extent(1) == stdex::dynamic_extent : false
     mixedStaticDynamic3D_2.extent(2): 2
     mixedStaticDynamic3D_2.static_extent(2): 18446744073709551615
     mixedStaticDynamic3D_2.static_extent(2) == stdex::dynamic_extent : true

mixedStaticDynamic3D_3.rank(): 3
mixedStaticDynamic3D_3.rank_dynamic(): 1
mixedStaticDynamic3D_3 calculated static ranks: 2
 mixedStaticDynamic3D_3.extent(0): 4
 mixedStaticDynamic3D_3.static_extent(0): 4
 mixedStaticDynamic3D_3.static_extent(0) == stdex::dynamic_extent : false
   mixedStaticDynamic3D_3.extent(1): 5
   mixedStaticDynamic3D_3.static_extent(1): 5
   mixedStaticDynamic3D_3.static_extent(1) == stdex::dynamic_extent : false
     mixedStaticDynamic3D_3.extent(2): 2
     mixedStaticDynamic3D_3.static_extent(2): 18446744073709551615
     mixedStaticDynamic3D_3.static_extent(2) == stdex::dynamic_extent : true
```
