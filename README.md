# RPC
RPC framework & examples
- For more information, read `framework/notes.txt`
- Open source under [LICENSE.txt]

## Build & Use

Briefly, we first build rpcc and then generate code from your source code with it. After that, you can use the framework to build your RPC server and client based on your need.

### Build

#### Requirements
To build and use the framework, please make sure [CMake] and a build tool are installed and in `PATH`
#### Configure Build With CMake
1. Switch to project directory.
1. Create build directory with `mkdir build`
1. Switch to build directory with `cd build`
#### Build `rpcc`
rpcc generates code necessary for later builds from your input source code.
1. Create build directory with `mkdir rpcc`
1. Switch to build directory with `pushd rpcc`
1. Configure build files using CMake with `cmake ../../rpcc`
1. Build and move the binary.
	##### Linux (Makefile)
- Build the binary with `make rpcc`
- Move the binary to `framework` directory with `mv rpcc ../../framework`
	##### Windows (MSBuild)
- Build the binary with `msbuild /m /p:Configuration=Release rpcc.vcxproj`
- Move the binary to project directory with `move Release\rpcc.exe ..\..\framework`
<br><br>
5. Switch back to build directory with `popd`
#### Generate Code With `rpcc`
1. Switch to `framework` directory with `pushd ../framework`
1. Specify the file where your functions are as the argument of rpcc. <br>
For example: `./rpcc function.cpp`<br>
Or you can use the interactive terminal/console interface.
1. Switch back to build directory with `popd`
<br><br>
Now the framework is ready for build.
#### Build `rpc-server` & `rpc-client`
1. (<b>Optional</b>) Modify source code paths in `rpcfunc_sources.cmake` in the `framework` directory if necessary.
- `RPC_FUNC_SOURCES` specifies souce code where user functions used by framework are.
- `RPC_CUSTOM_DEF_SOURCES` speccifies source code where custom definitions are.
- `RPC_CLIENT_FUNCCALL_SOURCES` specifies client-side source code where the RPC functions are called.
2. Create build directory with `mkdir framework`
1. Switch to build directory with `pushd framework`
1. Configure build files using CMake with `cmake ../../framework`
1. Build and move the binary.
	##### Linux (Makefile)
- Build the binary with `make`
- Move the binary to wherever you need it with `mv rpc-server rpc-client new_path_of_binaries`
	##### Windows (MSBuild)
- Build the binary with `msbuild /m /p:Configuration=Release rpc.vcxproj`
- Move the binary to project directory with `move Release\rpc-server.exe,Release\rpc-client.exe new_path_of_binaries`

### Use

- To use `rpc-server`, simply execute it to use default port `12345` or specify port in argument.
- To use the example client `rpc-client`, simply execute it.

## Security
Note that our security measures are:
- [x] capable of protecting you from attempts to crack the encrypted traffic.
- [ ] incapable of identifying fake servers that phish you from the start.

[LICENSE.txt]: https://www.apache.org/licenses/LICENSE-2.0.txt
[CMake]: https://cmake.org/download/
