---
tags:
  - "developers"
title: Building Docs Locally
permalink: building_docs.html
folder: docs
---

Offering documentation edits while using HDTree is incredibly helpful. 
Documentation (both manually written and automatically generated) 
is kept on the `gh-pages` branch of the GitHub repository. 

The automatically generated documentation from source code is 
usually more detailed and is done differently depending on the
language being written, so that "API Reference" is kept in separate
sites for each API. Manually written documentation is also 
separated by API, but are all written here and processed by jekyll.

## Launching Local Version of Docs
This documentation website is generated using jekyll and the `gh-pages`
branch has a few short scripts that use a container-ized jekyll to 
build the website and host a local server. This means you can get started
on editing the documentation by first [installing docker](https://docs.docker.com/get-docker/)
on your system and then using those scripts. For example,
```
git checkout gh-pages
./watch.sh
```

**Tip**:
Since the `gh-pages` branch has such a different file structure than the branches
with the code on them, I've found it helpful to have separate clones of the
repository: one to be on the `gh-pages` branch for editing the manual documentation
and one to be on a code branch for editing the code and the comments that
get processed into the reference manuals.

## Reference Manuals
The different APIs have different methods of generating reference manuals
from the comments in the code. Besides editing the manual documentation
and having it processed by jekyll, these files are copied onto the `gh-pages`
branch into a subdirectory so that they are not modified by jekyll but still
hosted at the same website.

### C++
The C++ reference is generated using [doxygen](https://www.doxygen.nl/manual/install.html)
and the [doxygen-awesome](https://jothepro.github.io/doxygen-awesome-css/) theme.
The theme is kept in a git submodule, so you will need to make sure the submodules are
downloaded for the local version to be the same theme as the online version.
```
git submodule --update --init
```
After installing `doxygen`, it is expected to be run from the root of this repository.
```
doxygen cpp/docs/doxyfile
```
This produces the HTML doxumentation in the `cpp/docs/html` directory.
If you don't have the jekyll infrastructure up and running, you can view the HTML files
generated by doxygen by opening them in your favorite browser. For example,
```
firefox cpp/docs/html/index.html
```
If you do have the jekyll infrastructure up and running, you can mimic what the GitHub
actions do by copying the generated files into the expected location on the `gh-pages` branch.
For example, if I have two copies of the repository: `hdtree-gh-pages` on the `gh-pages` branch
and `hdtree` on the branch I'm developing on, I can
```
cp -r hdtree/cpp/docs/html hdtree-gh-pages/cpp
```