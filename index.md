---
title: "HDTree"
keywords: sample homepage
sidebar: home_sidebar 
layout: home
permalink: index.html
---

ROOT's TTree data structure has been highly successful and useful for HEP; 
nevertheless, alternative file formats now exist which may offer broader 
software tool support and more-stable in-memory interfacing. 

HDTree is one realization of mimicing the TTree-style of data organization
within the popular [HDF5 File Format](https://www.hdfgroup.org/solutions/hdf5/).

## Why
As mentioned above, there are many reasons to use an alternative file format.
Here, two specific reasons are detailed in order to explain some of the 
motivation behind this project.

### Memory Stability
For many developers in HEP, the segmentation faults generated when trying to
implement features using ROOT have become common-place and this poor memory
management hampers a lot of development from proceeding. Specifically, the
thread safety of any library using ROOT for Disk I/O would need to be 
thoroughly tested and studied in order to be trusted. HDF5 does not have this
issue - it is built on a foundation of thread safety in order to ensure 
serialization of all disk operations. This simplifies the devleopment of
multi-threaded programs greatly and allows for libraries (like HDTree APIs)
to _also_ be thread safe.

### Serialization Performance
While ROOT's TTree library has been tuned for HEP's data access pattern,
we can also tune libraries based on HDF5 to the same data access pattern.
This usually involves chunking the data so that fewer disk operations can
be made while simultaneously preventing all of the data from being put
into memory at once (often not feasible given memory limitations).

After performing this tuning, we can observe _an improvement_ of serialization
performance when using HDTree's C++ API compared to ROOT's TTree. The details
of this comparison are left to [another page](cpp_performance.html) for your
investigation.

### Should I use HDTree?
The HDTree data organization structure is focused on _highly structured_ and
_ragged_ data. If your data does not satisfy both of these criteria, then
it probably does not make sense to use HDTree. For example, if your data consists
entirely of arrays of known dimension and length (e.g. images of a known size), then
it is better suited to store the data directly with HDF5 DataSets rather than
use HDTree.

#### Alternatives
Many different languages have more general APIs for working with HDF5 files.
Listed here are ones I've come across during my development of HDTree and 
its APIs.
- [HDF5 Official APIs](https://portal.hdfgroup.org/display/HDF5/HDF5) :
  HDF5 has official APIs for C and Fortran.
- [HighFive](https://github.com/BlueBrain/HighFive) is a modern C++ wrapper
  around the HDF5 C API allowing it to be thread safe and give access to
  parallel I/O. It is header-only, templated, defines `std::string` HDF5
  data types, does exception translation, and is well supported.
- [ess-dmsc/h5cpp](https://github.com/ess-dmsc/h5cpp) is another C++ wrapper
  around HDF5 C API. Relatively newer, still actively developed. It is 
  referenced as a "user provided" C++ API on the HDF5 website.
- [steven-varga/h5cpp](https://github.com/steven-varga/h5cpp) is another C++
  wrapper around HDF5 C API. Older but not as well maintained, last pushed
  in June 2021.
- [h5py](https://www.h5py.org/) is a Python library using the HDF5 and NumPy
  C APIs under-the-hood making it incredibly efficient. It informs HDTree's
  definition of how booleans should be defined in HDF5 data sets. It is also
  explicitly referenced as the Python API within examples posted on HDF5's
  documentation website.
- [hdf5](https://docs.rs/hdf5/latest/hdf5/) is a Rust crate binding the HDF5
  C API into Rust as well as providing higher-level rust-focused functions
  for common tasks.
- [HDF5.jl](https://juliaio.github.io/HDF5.jl/stable/) is a julia package
  which, similar to Rust's package, calls the HDF5 C API under-the-hood
  and offers higher-level julia-focused functions.

## What
The HDTree project consists of the documentation of the HDTree Meta-Format
as well as APIs implementing interaction with this Meta-Format.

<ul>
  {% for page in site.pages %}
    {% if page.tags contains "getting_started" %}
  <li>
    <a href="{{ page.url }}">{{ page.title }}</a>
  </li>
    {% endif %}
  {% endfor %}
</ul>

## Contributing

Anything from fixing typos on this site to developing an API for another language are welcome.
The pages listed here (and in the sidebar) contain information to help you get started.

<ul>
  {% for page in site.pages %}
    {% if page.tags contains "developers" %}
  <li>
    <a href="{{ page.url }}">{{ page.title }}</a>
  </li>
    {% endif %}
  {% endfor %}
</ul>


