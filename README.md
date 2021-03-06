[![Build Status](https://travis-ci.org/ilixi/ilixi.svg)](https://travis-ci.org/ilixi/ilixi)

[![Coverity Scan Build Status](https://scan.coverity.com/projects/5444/badge.svg)](https://scan.coverity.com/projects/5444)

# What is ilixi?

ilixi is an open-source lightweight GUI framework and it is 
designed with Linux based embedded devices in mind. ilixi 
uses DirectFB at its core in order to render all graphics 
content to a Linux framebuffer device. This eliminates the 
need for X server.

## Build instructions

This section briefly describes the steps for building ilixi only. 
For information on building dependencies please refer to ilixi 
web site (http://www.ilixi.org).

ilixi depends on following packages:
* DirectFB
* libxml2
* libsigc++
* fontconfig
* gettext

Once all dependencies are successfully installed on your system 
you can build and install ilixi as follows:

	./configure
	make && make install

## Documentation

Please refer to ilixi homepage for guides and tutorials.

ilixi uses Doxygen for API documentation. If your system has required software, 
you can generate an up-to-date version as follows:

	./configure
	make doxygen-doc

## License

ilixi is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
