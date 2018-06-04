Tankette Utility : imagecooker
==============================

(C) Sean Brennan, 2018
------------------------------

### Description

This program is used to output many files from an input file, a PNG or TIFF.
The output files are either PNGs or 16-bit unsigned int data files.

The files are collected by a java utility into an sqlite3 database for use
by the tankette terrain server.

Example input files are NASA's Mars DEM and Viking color TIFF images, with
resolutions of 46080 x 23040 and 92160 x 46080, and sizes of 2 gigs and
12 gigs respectively.

On a 7 gig machine the viking load will fail.  On a 16 gig machine it barely
runs.  It does not do anything clever like chop images a scanline at a time,
but rather by loading the whole buffer into memory and then doing pixel
compares.  This is because the underlying resolution of the image is ignored,
the output is specified in resolution per tile and number of tiles.

No blurring or averaging is done, simply floor and nearest pixel.

For tiles numbering w across and h high, images are created in:
+ ./textures/i_y/tex_y_x.png
and mesh data in
+ ./meshes/d_y/data_y_x.dat


### Usage

The following directories/links to directories should be made ahead of time.
1. ./textures
2. ./meshes

They should be writable and empty to start with.  Then decide the following:
1. The number if tiles across
2. The number if tiles down
3. The resolution of the textures, in x and y.
4. The resolution of the meshes, in x, and y.

Then create the two tile groups with the following commands:
+ _textures_ ./imagecooker -f input.png -x xres -y yres -w width -h height -t
+ _meshes_ ./imagecooker -f input.tiff -x xres -y yres -w width -h height -m

You can check the resulting texture images with an image viewer like eog.

To assemble the resulting tiles into an sqlite database and populate the
manifest file, stay tuned for a new java repository.
