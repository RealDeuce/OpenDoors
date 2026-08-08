# Building and linking

OpenDoors supports current Windows, Linux, and macOS toolchains through CMake.
It also retains legacy makefiles and a separate Open Watcom project for
16-bit DOS.

## Current host builds

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --install build --prefix /path/to/prefix
```

Both library variants are enabled by default. They can be selected
independently; at least one must remain enabled:

```sh
cmake -S . -B build \
  -DOPENDOORS_BUILD_SHARED=ON \
  -DOPENDOORS_BUILD_STATIC=OFF
```

On Unix-like systems this produces `libODoors.so` and `libODoors.a` (or the
macOS dynamic-library equivalent). On Windows, the shared build produces
`ODoors62.dll` and `ODoorW.lib`; the static build produces
`ODoors-static.lib`.

The install step installs the selected libraries, public headers, license, and
CMake package files. A downstream CMake project can request either installed
variant:

```cmake
find_package(OpenDoors 6.30 CONFIG REQUIRED COMPONENTS Static)
target_link_libraries(mydoor PRIVATE OpenDoors::Static)
```

Use the `Shared` component and `OpenDoors::Shared` target for dynamic linking.
Requesting a component which was not built causes `find_package()` to fail.
On Windows, `OpenDoors::Static` also supplies the required
[`OD_WIN32_STATIC`](../reference/constants/general.md) definition. Projects
which link `ODoors-static.lib` without the CMake target must define it
themselves before including [`OpenDoor.h`](../reference/api/index.md).

The PowerShell wrapper selects an MSVC architecture and keeps each build tree
separate:

```powershell
.\build-msvc.ps1 -Architecture x64 -Configuration Release
.\build-msvc.ps1 -Architecture x86 -Configuration Debug
```

Use `-Clean` when the selected build tree must be recreated. Pass
`-XpdevDirectory` only when building the optional examples which require
Synchronet's xpdev library.

## DOS

The DOS project is deliberately separate from the host CMake project. With
Open Watcom 2.0 configured in the environment:

```sh
cmake -S dos -B build/dos -G "Watcom WMake" \
  -D CMAKE_SYSTEM_NAME=DOS \
  -D CMAKE_SYSTEM_PROCESSOR=I86 \
  -D CMAKE_BUILD_TYPE=Release
cmake --build build/dos
```

This creates the large-model `ODoorl.lib` library. The normal CI build also
runs its DOS smoke test under DOSBox.

## Older build systems

The GNU, Windows make, and DOS makefiles remain for established projects and
older compilers. New projects should normally use CMake, but using the modern
build does not change the public OpenDoors calling interface.
