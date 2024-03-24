# Integrate Algorithm Library into Unreal Engine project
This project builds the algorithm library as a static library that can be integrated into [Unreal Engine](https://www.unrealengine.com) projects. 

The static library has been written and tested on Ubuntu 22.04 with Unreal Engine [v.5.3](https://docs.unrealengine.com/5.3/en-US/).

The code in this repo has been tested with Clang 14.0.0/16.0.6, GCC 11.4.0 and libc++ 15.0.7.

In order to link the static library with Unreal Engine it must be build with [libc++](https://libcxx.llvm.org/) and not [libstdc++](https://gcc.gnu.org/onlinedocs/libstdc++/). This can be accomplished in several ways:
* Compile the library with Clang and set the flag `-stdlib=libc++`.
* Compile the library with GCC and ignore standard C++ include directories. Instead, Unreal Engine [source code](https://docs.unrealengine.com/5.3/en-US/downloading-unreal-engine-source-code/) provides an implementation of [libc++](https://github.com/EpicGames/UnrealEngine/tree/release/Engine/Source/ThirdParty/Unix/LibCxx/include/c%2B%2B/v1) that can be passed to the compiler. 

It is possible to combine the two solutions, for instance the Unreal Engine version of libc++ will also work with a Clang compiler.

This project uses the first approach by default.  The second choice can be enabled by passing the path to Unreal Engine during CMake configuration of the project.

Since Unreal Engine compiles projects into shared libraries, this project also sets the `-fPIC` flag during compilation.

## Compilation using installed Clang compiler and libc++
This is the default way to build this project and it requires a Clang compiler and libc++. It uses whatever compiler is set as clang/clang++ on the command line. If another compiler is needed, then update [CMakeLists.txt](CMakeLists.txt) accordingly.

If compilation fails, it is likely the wrong libc++ (or none) is installed on the system, since libc++ is not standard on Linux. It can be installed with: `sudo apt install libc++-15-dev libc++abi-15-dev`. 

## Compilation using Unreal Engine implementation of libc++
If libc++ is not available on the system then a version is available [here]((https://github.com/EpicGames/UnrealEngine/tree/release/Engine/Source/ThirdParty/Unix/LibCxx/include/c%2B%2B/v1)), which requires downloading the Unreal source code. 

This way of compiling can be enabled by passing the define <code>-DUNREAL_ENGINE_PATH=*path_to_unreal_engine/*</code> to CMake during configuration. 

Unless a compiler is set in the CMake configuration it will use the default one, which on vanilla Ubuntu 22.04 is GCC 11.4.0.
