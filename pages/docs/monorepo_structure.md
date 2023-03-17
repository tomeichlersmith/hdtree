---
tags:
  - "developers"
title: Structure of Mono-Repo
permalink: monorepo_structure.html
folder: docs
---

In order to ensure uniformity of the HDTree meta-format,
the various API implementations are kept within this mono-repo.
Each implementation can cater to its language's strengths;
nevertheless, the meta-format itself should unite all of the APIs.

For this reason, the different APIs will also be tested to make sure
files written by one API can be read by others. Each API has its own
subdirectory and it has full control over the organization within that
subdirectory to conform to language conventions. So, in general, the
structure of the mono-repo's root directory is very simple:

- `.github/` : GitHub workflows, templates, and other GitHub-related files
- `test/` : Integration tests to make sure files from one API can be read by another
- `cpp/` : CPP API Implementation
- `Xlang/` : some language X API implementation
- `README.md` : GitHub README
