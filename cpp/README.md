# hdtree-cpp
C++ API for the HDTree data organization structure.

hdtree-cpp is a C++17 library with support for 
- serial read/write of an HDTree
- schema evolution of user-defined structures stored in branches of the HDTree

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
**only the goal at the moment**
```cpp
{ // write, we hold the handle
  hdtree::Writer tree("my-file.hdf5","/path/to/tree");
  tree.branch<int>("i_entry");
  for (std::size_t i{0}; i < 5; i++) {
    tree["i_entry"] = i;
    // same as
    // tree.set<int>("i_entry",i);
    tree.fill();
  }
}

{ // write, you hold the handle, similar to TTree
  hdtree::Writer tree("my-file.hdf5","/path/to/tree");
  int mine;
  tree.branch<int>("i_entry", &mine);
  for (std::size_t i{0}; i < 5; i++) {
    mine = i;
    tree.fill();
  }
}

{ // read, we hold the handle
  hdtree::Reader tree("my-file.hdf5","/path/to/tree");
  for (std::size_t i{0}; i < r.entries(); i++) {
    // emphasis on const and &
    //   those are what allow us to skip an unnecessary copy into a local variable
    const auto& read = t.get<int>("i_entry");
    assert(i == read);
  }
}

{ // read, you hold the handle, similar to TTree
  hdtree::Reader tree("my-file.hdf5","/path/to/tree");
  int mine;
  tree.attach("i_entry", &mine);
  for (std::size_t i{0}; i < r.entries(); i++) {
    assert(i == mine);
  }
}
```

## Road Map
- Get operational in current form
- Add Tree interface for holding Branches
- Maybe move buffer into Branches?
- More tests and docs
- exception class for better downstream exception handling
- compare we vs you handle holding
- Look into parallel read

## Table of Contents
- include: the headers for the HDTree C++ API
- src: source files compiled into the hdtree-cpp library
- test: source files for testing hdtree-cpp
- examples: simple, example programs showing hdtree-cpp's various abilities
  - compilation and running of examples are included _always_ included in the build
    so that developers of hdtree-cpp can keep them up-to-date
