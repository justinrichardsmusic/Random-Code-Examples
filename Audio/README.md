# Audio Listener / Source PGEX

What is it?
-----------

Adds the ability to play sounds / music using the olc::PixelGameEngine.
The Audio Listener functions as a set of ears to hear sounds in the game
and its position can be adjusted away or closer to Audio Sources so that
panning / volume can be adjusted accordingly.

How does it work?
-----------------

It makes use of the SoLoud cross platform audio library which you can find
out more about here:

https://sol.gfxile.net/soloud/

SoLoud provides several backend implementation options across many platforms
and best of all, it can be configured to compile with emscripten to run in the
web browser as well :-)

Download Instructions
------------------

Unlike my other PGEX options, this one does have some dependencies and will require
some additional setup.

First things first, you'll need to download the source files to SoLoud.  You can find
them here:

https://sol.gfxile.net/soloud/downloads.html

Extract them somewhere on your drive so you can access them when needed (they don't need
to be in your project directory in most situations so keep them separate for now).

Building a Backend
------------------

In order to not have to include all of the SoLoud source files in our project we will need
to first build a backend (static library) to include instead.  This will make things much
simpler in our project as we don't need to modify any of the SoLoud source code anyway.

Open a command prompt (as admininstrator) and browse to the \build folder of the SoLoud
installation.  Now run "Genie /?" to see a list of the various options you have for backends
etc...

You can of course choose one that suits your needs, however I'm going to go with SDL2 as it
is the most common / tested backend available and it is cross platform and works in the
web browser also.

(If you don't have SDL2 installed you will need to download and install it somewhere first!)

Before running the Genie command you will need to first edit the genie.lua file to point to
where you have your installation of SDL2. For example;

    local sdl2_root      = "/sdl2/sdl2-2.0.16"

Save the lua file and return to the command prompt.

Now run Genie again as follows (for Windows using vs2019)

    genie --with-sdl2static-only vs2019

This will generate the necessary project files in the build directory in a new \vs2019
directory.

Now open the "SoloudStatic.vcxproj" in Visual Studio.  Set it to Release Mode and build the
static version (right click the soloudStatic project and select Build).  You should now
see a \lib folder in the main SoLoud directory. This will now contain the soloud_static.lib
file.  This is the file that will be needed as a replacement for the SoLoud source code.

Configuring Visual Studio
-------------------------

I happen to use Visual Studio on Windows, however these instructions can be adapted to suit
other environments also.

Firstly, create a bare bones PGE project.

Next, open the project properties (make sure you have All Configurations and All Platforms
selected before making the next changes).

Under C/C++ in the Additional Include Directories field, add in the location of your
SoLoud \include folder.  This will allow the Audio Listener to access the SoLoud header
information.

Under Linker/Input in the Additional Dependencies field, add in "soloud_static.lib;". This
will link the compiled soloud library at runtime.

However, we still need to place a copy of the soloud_static.lib file in our project directory.
Copy the library file from your SoLoud installation over to your project directory (usually
the same directory as your main.cpp file).

You should now be setup for using SoLoud in your PGE project.

Try to build and run your project, you should not get any errors and it should run at this point
(although it won't do anything yet...)

...more to come...
