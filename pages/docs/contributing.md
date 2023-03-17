---
tags:
  - "developers"
title: Contributing to HDTree
permalink: contributing.html
folder: docs
---

All contributing is helpful! 
Anything from correcting a spelling mistake in the documentation,
adding a new example, patching bugs, adding features, or 
as big as starting an API in a new language is highly encouraged. 
Below, I've collected some notes on these
various levels of contribution.

## Documentation Updates
If you are writing more detailed explanation or adding in a new
example, please `git clone` the repository and make sure the updated
documentation can be built into a website by jekyll and has the format
you expect. You can [build and view the documentation locally](building_docs.md)
with the help of a container runner like `docker` to aid in this development.

### New Examples
As far as I'm concerned, the more the merrier! If you are writing an example,
please be detailed about which API and which version of that API you are using
so that future readers can check if anything has changed since the example
was written.

## Patching Bugs or Adding Features
If you find a bug or think of a new feature to add, please open a
[GitHub Issue](https://github.com/tomeichlersmith/hdtree/issues/new)
to start the discussion. This allows all collaborators to see what you plan
to work on as well as potentially offer some insight on how to get going.

## New API
If your favorite language does not have an API represented,
feel free to start writing one! A first API does not have to be
super powered. Even a simple one only focused on reading without
parallelization can be a good start and open the door to other
contributors to expand on it.

Again, similar to patching bugs or adding features, please create
a GitHub issue to start a discussion and outline a plan for what
you want to implement.

As you get closer to a functional API, integration tests will also
be requested. So keep in mind that you may need to be able to run
one of the other APIs to help make sure your API is reading and/or
writing a correct form of the HDTree meta-format.
