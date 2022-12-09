# hdtree-cpp
C++ API for the HDTree data organization structure.

hdtree-cpp is a header-only C++17 library with support for 
- serial read/write of an HDTree
- schema evolution of user-defined structures stored in branches of the HDTree

## Installation
Since hdtree-cpp is header-only, you can _technically_ make sure your program
includes the `include` directory here; however, it is preferred to build, test,
and install hdtree-cpp within your environment so that you check hdtree-cpp is
compatible with your environment and you generate the CMake integration for
hdtree-cpp.

```
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=<prefix>
cmake --build build --target test
cmake --build build --target install
```

## Usage
**Goal at the moment**
```cpp
{ // write
  hdtree::Writer w("my-file.hdf5");
  hdtree::Tree t = w.open("/path/to/tree");
  for (std::size_t i_entry{0}; i_entry < 5; i_entry++) {
    t.set("i_entry",i_entry);
    t.fill();
  }
}

{ // read
  hdtree::Reader r("my-file.hdf5");
  hdtree::Tree t = r.get("/path/to/tree");
  for (auto entry : t) {
    const auto& i = entry.get<std::size_t>("i_entry");
  }
}
```

## Table of Contents
- include: the headers for the HDTree C++ API
- test: source files for testing hdtree-cpp
- examples: simple, example programs showing hdtree-cpp's various abilities
  - compilation and running of examples are included _always_ included in the build
    so that developers of hdtree-cpp can keep them up-to-date
