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

In order to NOT have to include all of the SoLoud source files in our project we will need
to first build a backend (static library) to include instead.  This will make things much
simpler in our project as we don't need to modify any of the SoLoud source code anyway.

Open a command prompt (as admininstrator) and browse to the \build folder of the SoLoud
installation.  Now run 

    Genie /?

to see a list of the various options you have for backends
etc...

You can of course choose one that suits your needs, however I'm going to go with SDL2 as it
is the most common / tested backend available and it is cross platform and works in the
web browser also.

(If you don't have SDL2 installed you will need to download and install it somewhere first!)
(Note: If you aren't intending on building for Emscripten then you can use MiniAudio which
 doesn't require any additional downloads)

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

(Note: x86 build confirmed working, x64 may not be compatible at this stage)

Configuring Visual Studio for Desktop Use with SoLoud
-----------------------------------------------------

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

Adding the Audio Listener and Audio Sources to your project
-----------------------------------------------------------

...to be added...

Configuring Visual Studio for use with Emscripten and SoLoud
------------------------------------------------------------

In order to be able to compile with Emscripten and SoLoud in our project we will need to make
a few changes.

First of all, any additional include options we set previously will no longer be work.  So we
will need to copy the headers inside the SoLoud \include folder into our project folder.

We also need the soloud_static.lib file which should already be in the project folder (just
mentioning it in case it isn't)

Great, that's the first part done :-)

Now we need to compile a ".o" file which contains the object symbols for the SoLoud header
files which we can use at compile time with emscripten, instead of manually compiling all
of the SoLoud headers every time we compile our program...

MaGetzUb from the discord was gracious enough to provide a batch file which will do exactly
this for us (Many Thanks!).

Run the emcompile.bat file from the root folder in your SoLoud directory and it should
hopefully produce a soloud.o file as a result.  The batch file is located in this repository.

Now copy the soloud.o file into your project directory.

Next step is to create a convenience text document in your project folder called
"em_cmd_line.txt".  We will configure our compiler flags here to make it easier to compile
later on.

Add the following text to the file:

    em++ -std=c++17 -O2 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s USE_LIBPNG=1 -s USE_SDL_MIXER=2
    
Next we need to add the files we wish to compile... assuming your main project file is called "main.cpp" we can use the following
additional text:

    main.cpp soloud.o cAudioListener.cpp cAudioSource.cpp -o pge.html --preload-file ./assets

The entire file should now contain the following:

    em++ -std=c++17 -O2 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s USE_LIBPNG=1 -s USE_SDL_MIXER=2 main.cpp soloud.o cAudioListener.cpp cAudioSource.cpp -o pge.html --preload-file ./assets
    
This will be our compile command to use with emscripten.  Save the text document.

Compiling with Emscripten for Web
---------------------------------


