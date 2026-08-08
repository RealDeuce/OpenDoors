# Building and linking

OpenDoors supports current Windows, Linux, and macOS toolchains through CMake.
It also retains legacy makefiles and a separate Open Watcom project for
16-bit and 32-bit DOS.

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
`ODoors63.dll` and `ODoorW.lib`; the static build produces
`ODoors-static.lib`.

The install step installs the selected libraries, [`OpenDoor.h`](../reference/api/index.md),
the DOS-only [`ODStat.h`](../reference/personality/index.md), the license, and
CMake package files. Other headers in the source tree are private. A downstream
CMake project can request either installed
variant:

```cmake
find_package(OpenDoors 6.3 CONFIG REQUIRED COMPONENTS Static)
target_link_libraries(mydoor PRIVATE OpenDoors::Static)
```

Use the `Shared` component and `OpenDoors::Shared` target for dynamic linking.
Requesting a component which was not built causes `find_package()` to fail.
On Windows, `OpenDoors::Static` also supplies the required
[`OD_WIN32_STATIC`](../reference/constants/general.md) definition. Projects
which link `ODoors-static.lib` without the CMake target must define it
themselves before including [`OpenDoor.h`](../reference/api/index.md).

With MSVC, `OPENDOORS_BUILD_MSVC_STATIC_MT=ON` adds
`OpenDoors::StaticMT` and `ODoors-static-mt.lib`, built with Microsoft's
static C runtime. The option requires `OPENDOORS_BUILD_STATIC=ON` and is not
available with other compilers. MinGW uses `OpenDoors::Shared` and
`OpenDoors::Static`, but its `.dll.a` and `.a` libraries are distinct from
MSVC's `.lib` files.

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

### 32-bit flat-model DOS

The same DOS-specific project selects a separate implementation when
[`CMAKE_SYSTEM_PROCESSOR`](https://cmake.org/cmake/help/latest/variable/CMAKE_SYSTEM_PROCESSOR.html)
is `I386`:

```sh
CC=wcl386 cmake -S dos -B build/dos32 -G "Unix Makefiles" \
  -D CMAKE_SYSTEM_NAME=DOS \
  -D CMAKE_SYSTEM_PROCESSOR=I386 \
  -D CMAKE_BUILD_TYPE=Release
cmake --build build/dos32
```

The build creates two libraries. `ODOOR32R.lib` is for applications compiled
with Open Watcom's `-3r` register convention; `ODOOR32S.lib` is for the `-3s`
stack convention. Every object in an application must use the convention
matching the selected library. [`OpenDoor.h`](../reference/api/index.md)
selects the matching API callback convention from the compiler mode and scopes
its enumeration and structure layout so unrelated compiler defaults cannot
change the DOS32 ABI.

The libraries target ordinary 32-bit flat-model LE executables. CI verifies
them under both DOS/4GW and DOS/32A. Release example executables have DOS/32A
bound into them and do not require a separate extender file. This product uses
DOS/32 Advanced DOS Extender technology.

The 32-bit DOS communication implementation supports
[`COM_FOSSIL`](../reference/constants/session.md#communication-methods).
Block calls use DPMI conventional memory as a transfer buffer and fall back to
byte-at-a-time FOSSIL calls when that buffer is unavailable.
[`COM_INTERNAL`](../reference/constants/session.md#communication-methods), the
direct-UART implementation, is not supported on this target.

## Older build systems

The GNU, Windows make, and DOS makefiles remain for established projects and
older compilers. New projects should normally use CMake, but using the modern
build does not change the public OpenDoors calling interface.
