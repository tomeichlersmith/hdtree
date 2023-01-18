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


