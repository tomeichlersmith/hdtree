---
tags:
  - "users"
  - "developers"
title: Coming From ROOT
classes: wide
permalink: metaformat_from_root.html
folder: metaformat
summary: For users coming from CERN's ROOT
---

_Page is Work in Progress_

Since the HDTree meta-format is directly comparable to (and inspired by) ROOT's TTree class,
many users of HDTree are expected to be familiar with the ROOT ecosystem. This page is
focused on providing guidance towards HDF5-related tools that would allow for similar
interaction with HDTrees that ROOT's ecosystem provides for TTrees.

## Graphical Browsing
- `TBrowser` -> `HDFView` and/or JupyterLab extension

## Plotting Branches
- `TTree::Draw` -> `h5py` and `matplotlib.hist`
- `scikit-hep.hist`

## Serialization of Histogram Objects
- `pickle`/`h5py` in python
- `HighFive` in C++

## Merging HDTrees
- Simple, small example using `h5py`
- Reference open issue for writing a C++ program

## awkward and pandas interface
- [Issue #11](https://github.com/tomeichlersmith/hdtree/issues/11) is aiming to define a HDTree Python API modeled after `uproot`'s interface for ROOT TTrees
