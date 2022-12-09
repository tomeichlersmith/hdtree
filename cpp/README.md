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
**at the moment**
```cpp
{ // write
  hdtree::Writer w("my-file.hdf5","/path/to/tree");
  hdtree::Branch<int> i_entry("i_entry");
  for (std::size_t i{0}; i < 5; i++) {
    i_entry.update(i);
    i_entry.save(w);
  }
}

{ // read
  hdtree::Reader r("my-file.hdf5");
  hdtree::Branch<int> i_entry("i_entry");
  for (std::size_t i{0}; i < r.entries(); i++) {
    i_entry.load(r);
    const auto& read = i_entry.get();
    assert(i == read);
  }
}
```

## Road Map
- Get operational in current form
- Add Tree interface for holding Branches
- Maybe move buffer into Branches?
- More tests and docs
- exception class for better downstream exception handling
- Look into parallel read

## Table of Contents
- include: the headers for the HDTree C++ API
- src: source files compiled into the hdtree-cpp library
- test: source files for testing hdtree-cpp
- examples: simple, example programs showing hdtree-cpp's various abilities
  - compilation and running of examples are included _always_ included in the build
    so that developers of hdtree-cpp can keep them up-to-date
