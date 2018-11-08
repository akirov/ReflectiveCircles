Reflective Circles
==================

This is a simple 2D ray-tracer with an interactive GUI, written in C++, which
tries to find all possible light paths from one point to another through a set
of reflective circles.


Build
-----
To compile under any OS you need Qt SDK installed. Go to "ReflectiveCircles" dir
and launch :
`qmake "CONFIG+=debug"` or `qmake "CONFIG+=release"`
It will create Makefile-s. Then launch :
`make`, `mingw32-make`, or whatever your make command is.
If everything is OK this will produce a binary called circles(.exe) in the
corresponding `bin` sub-folder.


Usage
-----
Just start the binary from `bin` sub-folder. It will open a GUI. There you
can set up a scene with source point A, destination B and several circles,
using the mouse and piking the drawing mode first from the left.
You can set the number of reflections using the "Reflections" spin box.
You can also load a scene from the "File" menu - an example scene file
"input.txt" is provided in the "scenes" dir. You can save the scene in a file
using "Save" from the "File" menu. Start ray tracing with the "Find Path"
button. "Reset" button clears the scene.

Note: The task is solved exactly only in the simplest case (no reflections). In
the other cases it is solved approximately, casting random rays in the scene,
tracing them and remembering these, which come close to the target point. The
target point itself is made "bigger". So each time you press "Find Path" button
you may get different solutions.


ToDo
----
- start tracing in new threads
- add "Stop" button to set mStopRendering flag
- optimize: don't calculate the distance twice - pass it from intersect() to reflect()...
- make MAX_NUM_RAYS configurable
- cast rays only to the figures, take clipping into account
- replace dynamic_casts with something better
- UI control to delete figures?
- QML GUI?
- use the GPU hardware (OpenGL, OpenCL)
- optimize the closest rays to hit point B? we can start from B...


Screenshots
-----------

![screenshot](https://github.com/akirov/ReflectiveCircles/raw/master/screenshot1.jpg)
