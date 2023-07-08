# Getting Started with HDTree C++

## Installing the HDTree C++ API
CMake and a C++17 compatible C++ compiler are required.
Both of which are readily available via your system software repjositories.
- On Ubuntu derivatives: `sudo apt update && sudo apt install cmake gcc g++`

The HDF5 libray exists in many Unix repositories so look there for installing
it, as always, you can fall back to 
[building the latest release from source](https://www.hdfgroup.org/downloads/hdf5/).
- On Ubuntu derivatives: `sudo apt update && sudo apt install libhdf5-dev`
- On MacOS: `brew install hdf5`

The HighFive C++ Wrapper is used by HDTree so it is also required.
- Download a [HighFive release](https://github.com/BlueBrain/HighFive/releases)
```
wget https://github.com/BlueBrain/HighFive/archive/refs/tags/v2.7.0.tar.gz
```
- Unpack the source
```
tar xzf v2.7.0.tar.gz
cd HighFive-v2.7.0
```
- Configure the build. Use the `CMAKE_INSTALL_PREFIX` if you wish to
  install HighFive somewhere besides `/usr/local`.
```
cmake -DHIGHFIVE_EXAMPLES=OFF -DHIGHFIVE_UNIT_TESTS=OFF -B build -S .
```
- Install the interface. May require administrative (`sudo`) privileges
  if installing to `/usr/local`.
```
make install
```

Building HDTree is similar to HighFive but a separate compilation
step will be helpful since, unlike HighFive, HDTree is not a
header-only C++ library.
- Download [a release](https://github.com/tomeichlersmith/hdtree/releases).
```
wget https://github.com/tomeichlersmith/hdtree/archive/refs/tags/cpp/v0.4.5.tar.gz
```
- Unpack the source
```
tar xzf v0.4.5.tar.gz
cd hdtree-v0.4.5/cpp
```
- Configure the build (again, use `CMAKE_INSTALL_PREFIX` if you wish to change
  the install location).
```
cmake -B build -S .
```
- Build the library.
```
cd build
make
```
- Install
```
make install
```


## First Steps
There are four ways to access a HDTree with the C++ API.
They are mainly separated by different stages of processing the data.
We start with `save` since you will first need to write a HDF5 file
with an HDTree in it in order to be able to go further.

The code below is copied in from the examples directory
within the C++ API source. This means the code "snippets" are pretty long,
but I've tried to include explanatory comments within them.
The examples are compiled alongside the HDTree C++ API so you
can try it out immediately after building it.

### write-only (`save`)
First, we are just going to write some example data to a file.
This shows an example of a write-only process. After compilation,
run by providing a name for the file and the tree.
```
hdtree-eg-save my-first-hdtree.h5 the-tree
```

```cpp
{{#include ../examples/save.cxx }}
```

### read and write (`transform` or `inplace`)
Another common task is to perform calculations on some input data
and save those calculations into the tree as well. This does not answer
the question of what should be done with the original data. Should we
(a) copy the original data and write it to a new file with the new data 
or (b) write the new data into the input file alongside the original data.
In the HDTree C++ API, option (a) is achieved with `transform` and option
(b) is done with `inplace`. Both can be run from the same executable and
the choice is made depending of if you give a new file and tree name or 
not.
```
# this will use hdtree::Tree::transform
hdtree-eg-transform my-first-hdtree.h5 the-tree my-second-hdtree.h5 the-second-tree
# this will use hdtree::Tree::inplace
hdtree-eg-transform my-first-hdtree.h5 the-tree
```

```cpp
{{#include ../examples/transform.cxx }}
```

### read-only (`load`)
Finally, the last common task is reading in the data from the tree and using
it to do some other task (e.g. making a plot or fitting the data with some
model). In this API, that is called `load`ing and the example program included
prints a simple histogram of the averages of the original data generated earlier.

**Fun Fact**: This is an example of the central limit theorem!

```
# this will error-out if you didn't run step two!
hdtree-eg-load my-first-hdtree.h5 the-tree
# the below is example output, it may change since the random data may change!
0.X | Num Entries
< 0 |
0.0 |
0.1 |*
0.2 |
0.3 |***
0.4 |********************************************
0.5 |*************************************************
0.6 |**
0.7 |
0.8 |*
0.9 |
> 1 |
```

```cpp
{{#include ../examples/load.cxx }}
```

## User-Defined Data Structures
User-defined objects can also be serialized within HDTree. Simplified
schema evolution (a la ROOT's `ClassDef` macro) is also available; however,
this example merely shows the required boiler-plate.

HDTree's C++ API has chosen to avoid automatically deducing the on-disk naming
from the in-memory class member names. This introduces more boilerplate, but,
in my opinion, is helpful for essentially documenting how on-disk data was
generated.


```cpp
{{#include ../examples/user_class.cxx }}
```


## More Intense Use Case
The C++ HDTree API is mainly implemented through its
various `Branch` classes. The `Tree` class is mainly there
to be a helpful interface for handling a set of `Branch`es.
I point this out because if you are interested in building
a larger data processing framework around the C++ HDTree API,
I would suggest focusing on writing your own version of `Tree`
to accomodate your needs rather than attempting to use the `Tree`
that is apart of this repository.

