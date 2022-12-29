---
title: "HDTree"
keywords: sample homepage
sidebar: home_sidebar 
permalink: index.html
---

introduction

## Why
- memory stability
- serialization performance

## What
project consists of this documentation on what the HDTree Meta-Format is 
using the language of HDF5 files and groups as well as APIs implementing
interaction with this Meta-Format in a few languages.

<ul>
  {% for page in site.pages %}
    {% if page.tags contains "users" %}
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


