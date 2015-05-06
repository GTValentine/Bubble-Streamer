Bubble Streamer
===============

* Valentine Kunin
* Kai Ninomiya


Building
--------

### Windows

To build the plugin, use Visual Studio 2012 to build `/hplugin.sln`. Release
mode is recommended. `/hplugin.pro` will NOT work due to a bug in 32-bit
Houdini 14 vc11.

The previewer must be built using Qt 5.4 (via Qt Creator). Build the
`/preview.pro` file.

### Linux

To build the plugin and previewer, simply type `make`. Qt 5.4 is required.
Some modification may be required: Update the Houdini version numbers and paths
in `/hplugin/hplugin.pro` to match your installed version.


Loading in Houdini
------------------

Copy or symlink the resulting `.dll` file to your Houdini DSO directory.


Demo Scene
----------

Open the `three-mesh.hipnc` Houdini scene file for a working demo.
