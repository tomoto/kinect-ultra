# Introduction #

Someone asked me how to write eyeSlugger.vtx to customize the Eye Slugger model. It may be a little challenging unless you are familiar with OpenGL, but let me try to explain.

A vtx file is a text file that defines a sequence of OpenGL functions to render polygons. You need to learn some OpenGL functions (actually four of them -- glBegin, glEnd, glVertex3f, and glNormal3f) to appropriately program a vtx file because each line of vtx is mapped to the invocation of these functions.

# Syntax #

  * A `Vtx File` starts with a line containing `<number of polygons>` followed by `Polygon Blocks` repeating `<number of polygons>` times.
  * A `Polygon Block` starts with a line containing `<mode number>` `<number of vertices>` followed by `Vertex Lines` repeating `<number of vertices>` times.
  * A `Vertex Line` is a line containing `<x>` `<y>` `<z>` `<nx>` `<ny>` `<nz>`.

Each `Polygon Block` is mapped to the following OpenGL function invocations: `glBegin(<mode number>)`, followed by `glNormal3f(<nx>, <ny>, <nz>)` and `glVertex3f(<x>, <y>, <z>)` repeating for each `Vertex Line`, and then followed by `glEnd()`.

# Example #

This is an extract of actual eyeSlugger.vtx with annotations.

```
64 <- 64 polygons will follow
9 4 <- starts the first polygon by glBegin(GL_POLYGON) with 4 vertices
-0.0000 345.9916 103.3103 0.0000 -0.1126 0.9936 <- creates a vertex by glNormal3f(0, -0.1126, 0.9936) and glVertex3f(0, 345.9916, 103.3103)
-0.0000 266.1489 94.2641 0.0000 -0.1126 0.9936
5.0000 266.1489 94.2641 -0.0000 -0.1126 0.9936
5.0000 336.9717 102.2894 -0.0002 -0.1126 0.9936
9 4 <- starts the next polygon
-0.0000 351.8913 76.1716 -0.0025 0.9787 0.2053
-0.0000 345.9916 103.3103 -0.0030 0.9787 0.2053
5.0000 336.9717 102.2894 0.8777 0.4687 0.0997
5.0000 342.4295 74.6831 0.8863 0.4542 0.0898
...
5 36 <- starts a polygon by glBegin(GL_TRIANGLE_STRIP) with 36 vertices
5.0000 12.8952 -143.2221 1.0000 -0.0000 0.0000
5.0000 15.9940 -133.8254 1.0000 -0.0000 0.0000
5.0000 26.3282 -132.8713 1.0000 -0.0000 0.0000
5.0000 24.2610 -122.4517 1.0000 -0.0000 0.0000
5.0000 50.0915 -122.9198 1.0000 -0.0000 0.0000
5.0000 50.6131 -105.5903 1.0000 0.0000 0.0000
5.0000 94.9993 -112.8516 1.0000 0.0000 0.0000
5.0000 89.1578 -95.8706 1.0000 0.0000 0.0000
5.0000 145.0083 -111.2813 1.0000 0.0000 0.0000
5.0000 128.8825 -91.1560 1.0000 0.0000 0.0000
5.0000 193.7792 -110.5681 1.0000 0.0000 0.0000
5.0000 174.9002 -86.2669 1.0000 0.0000 0.0000
5.0000 239.1787 -104.3003 1.0000 0.0000 0.0000
5.0000 185.1263 -81.9404 1.0000 0.0000 0.0000
5.0000 239.1787 -104.3003 1.0000 0.0000 0.0000
5.0000 226.4242 -62.6681 1.0000 0.0000 0.0000
5.0000 284.2665 -84.5811 1.0000 0.0000 0.0000
5.0000 242.5501 -64.2413 1.0000 0.0000 0.0000
5.0000 284.2665 -84.5811 1.0000 0.0000 0.0000
5.0000 258.2827 -62.2747 1.0000 0.0000 0.0000
5.0000 284.2665 -84.5811 1.0000 0.0000 0.0000
5.0000 268.1155 -52.4419 1.0000 0.0000 0.0000
5.0000 315.7613 -53.2953 1.0000 0.0000 0.0000
5.0000 276.3751 -35.5294 1.0000 0.0000 0.0000
5.0000 334.9413 -14.5941 1.0000 0.0000 0.0000
5.0000 251.5963 -34.7428 1.0000 0.0000 0.0000
5.0000 334.9413 -14.5941 1.0000 0.0000 0.0000
5.0000 268.5088 0.1519 1.0000 0.0000 0.0000
5.0000 334.9413 -14.5941 1.0000 0.0000 0.0000
5.0000 292.5009 0.2299 1.0000 0.0000 0.0000
5.0000 342.9587 22.8989 1.0000 0.0000 0.0000
5.0000 303.9953 47.4532 1.0000 0.0000 0.0000
5.0000 342.4295 74.6831 1.0000 0.0000 0.0000
5.0000 272.4419 44.7820 1.0000 0.0000 0.0000
5.0000 336.9717 102.2894 1.0000 0.0000 0.0000
5.0000 266.1489 94.2641 1.0000 0.0000 0.0000
...
```

# Converting .obj to .vtx #

You can use a Perl script obj2vtx.pl (http://kinect-ultra.googlecode.com/svn/trunk/model/obj2vtx.pl) to create a scaffold of .vtx file. You need to have Perl to run it.

`perl obj2vtx.pl my_model.obj > my_model.vtx`

This script creates a .vtx file by simply mapping each surface in .obj file to an OpenGL polygon rendered by GL\_POLYGON mode. This will work fine without modifications if all your polygons are convex. On the other hand, if you have one or more non-convex polygons (Eye Slugger may have them unfortunattely), you need to fix them manually -- change the mode from GL\_POLYGON (number = 9) to GL\_TRIANGLE\_STRIP (number = 5) or some other mode, and exchange the order of vertices to be rendered appropriately. (In theory, this process could be automated, but I simply did not do it to spare my time.)