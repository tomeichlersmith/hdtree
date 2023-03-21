---
tags:
  - "users"
  - "getting_started"
title: C++ HDTree API
classes: wide
permalink: cpp_landing_page.html
folder: cpp
summary: Getting Started with the C++ HDTree API
---

## First Steps
There are four ways to access a HDTree with the C++ API.
They are mainly separated by different stages of processing the data.
We start with `save` since you will first need to write a HDF5 file
with an HDTree in it in order to be able to go further.

The code snippets below are copied in from the examples directory
within the C++ API source.

### write-only (`save`)
{% highlight cpp %}
{% include cpp/save.cxx %}
{% endhighlight %}

### read-only (`load`)

### read from one file and write to a different one (`transform`)

### read from and write to the same file (`inplace`)

## More Intense Use Case
The C++ HDTree API is mainling implemented through its
various `Branch` classes. The `Tree` class is mainly there
to be a helpful interface for handling a set of `Branch`es.
I point this out because if you are interested in building
a larger data processing framework around the C++ HDTree API,
I would suggest focusing on writing your own version of `Tree`
to accomodate your needs rather than attempting to use the `Tree`
that is apart of this repository.

