---
# Feel free to add content and custom Front Matter to this file.
# To modify the layout, see https://jekyllrb.com/docs/themes/#overriding-theme-defaults

layout: home
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
    {% if page.tags contains "index-include" %}
  <li>
    <a href="{{ page.url }}">{{ page.title }}</a>
  </li>
    {% endif %}
  {% endfor %}
</ul>
