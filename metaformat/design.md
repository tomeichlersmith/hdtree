# Design Principles

This page outlines the qualitative goals of the HDTree meta-format and
thus also any API interacting with it. These are not concrete requirements
of any API; however, they are helpful to keep in mind when diving into
deep development associated with HDTree.

## Accessible Along Both Axes
In the "DataFrame" vocabulary (popularized by the [R language](https://www.r-project.org/) 
and the Python package [pandas](https://pandas.pydata.org/)),
each table of data has two axes: along the rows and along the columns. HDTree is focused
on making sure the data is accessible along both of these axes because both access patterns
are useful in different situations.

In HDTree vocabulary, each "branch" is a "column" and each "event" is a "row". While HDTree
allows for entries within one "cell" (an intersection between a single row and column) to
be an arbitrarily complex data type, this organization is still at the foundation of its
development.

## In-File Version Control
Not only will HDTree store the version of the meta-format in the objects it writes to HDF5
files, it will also store the version of the API and the version of any user-defined data
structures. This gives allows users to recieve the benefits of a flexible schema without
losing track of how their schema has evolved.

## Concrete Acknowledgement of Data Organization
When defining user data types for serialization, HDTree APIs require acknowledgement
of where the data is going in the HDTree in-file structure. This may not be literally
required in some dynamic language APIs; however, it is important for the user to 
be aware of how their data will be organized and how it will end up on disk.

## Not Limited to Specific Language
HDTree _is_ the meta-format. While originating in C++, the format of data-on-disk should
not be limited to a specific language. This prevents evolution of the meta-format or
its APIs. This informs development of the meta-format itself by requiring any new features
implemented in one language API to have plausible equivlants in other languages.
