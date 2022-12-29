---
tags:
  - "index-include"
title: Meta-Format Description
classes: wide
---

HDTree is a specific metaformat on top of a HDF5 Group.

The tree has a few HDF5 attributes of its own to help interface.
- `__size__` : the number of entries in the tree
- `__version__` : the version of the HDTree meta-format
- `__api__` : the API that was used to write the HDTree
- `__api_version__` : the version of that API

Besides these attributes, all child groups of the tree are the branches.
Each branch can have an arbitrary number of child branches itself.

Each branch has a two attributes
- `__type__` : the name of the type of this branch
- `__version__` : the version of the type

A variable-length container is "flattened" into two sub-branches:
- `__size__` : is a branch storing the successive sizes of the containers
- `data` : is a branch storing the entries in the containers

Similarly, a variable-length mapping is flattened into three sub-branches:
- `__size__` : is a branch storing the successive sizes of the mappings
- `keys` : is a branch storing the keys in the mappings
- `vals` : is a branch storing the values in the mappings

The recursive process of sub-branching continues until atomic[^1] data types are reached.
Which are the only actual HDF5 DataSets. They are stored in chunked and compressed 
one dimensional DataSets.

[^1]: integers, floats, and strings

