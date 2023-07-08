# hdtree-cpp
C++ API for the HDTree data organization structure.

hdtree-cpp is a C++17 library with support for 
- serial read/write of an HDTree
- schema evolution of user-defined structures stored in branches of the HDTree

> [Online Reference Manual](docs/html/index.html)

## Installation
### Depedencies
- HDF5
- HighFive
- Boost (for demangling, plans to make optional)

```
cmake -B build -S . \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=<prefix>
cmake --build build --target test
cmake --build build --target install
```

## Usage
Below are just code snippets, please look into the examples for complete
programs that compile and run with HDTree.

### Write 
examples/save.cxx
```cpp
auto tree = hdtree::Tree::save("my-file.hdf5","/path/to/tree");
auto& i_entry = tree.branch<int>("i_entry");
for (std::size_t i{0}; i < 5; i++) {
  *i_entry = i;
  // same as
  //i_entry.update(i);
  tree.save();
}
```

### Read and Write
examples/transform.cxx
```cpp
{ // read and write (separate source/dest)
  auto tree = hdtree::Tree::transform("one.hdf5","/tree1","two.hdf5","/tree2");
  // read this branch
  auto& i_entry = tree.get<int>("i_entry");
  // write this branch
  auto& my_cool_new_var = tree.branch<double>("coolio");
  for (std::size_t i{0}; i < tree.entries(); i++) {
    tree.load();
    
    *my_cool_new_var = i_entry*4.2;    

    tree.save();
  }
}

{ // read and write (same source/dest)
  auto tree = hdtree::Tree::inplace("one.hdf5","/tree1");
  // read this branch
  auto& i_entry = tree.get<int>("i_entry");
  // write this branch
  auto& my_cool_new_var = tree.branch<double>("coolio");
  for (std::size_t i{0}; i < tree.entries(); i++) {
    tree.load();
    
    *my_cool_new_var = i_entry*4.2;    

    tree.save();
  }
}
```

### Read
examples/load.cxx
```cpp
{ // read
  auto tree = hdtree::Tree::load("my-file.hdf5","/path/to/tree");
  // & required
  auto& i_entry = tree.get<int>("i_entry");
  for (std::size_t i{0}; i < r.entries(); i++) {
    tree.load();
    assert(i == *i_entry);
  }
}
```

## Benefits
- resets value in a Branch to a empty state after each `save`
  (i.e. treat the Branch reference as a for-loop-local variable)
- hdtree juggles the memory addresses

## Table of Contents
- include: the headers for the HDTree C++ API
- src: source files compiled into the hdtree-cpp library
- test: source files for testing hdtree-cpp
- examples: simple, example programs showing hdtree-cpp's various abilities
  - compilation and running of examples are _always_ included in the build
    so that developers of hdtree-cpp can keep them up-to-date
