# Building and linking

OpenDoors supports current Windows, Linux, and macOS toolchains through CMake.
It also retains legacy makefiles and a separate Open Watcom project for
16-bit DOS.

## Current host builds

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

On Unix-like systems this produces `libODoors.so` and `libODoors.a` (or the
macOS dynamic-library equivalent). On Windows, the shared build produces
`ODoors62.dll` and `ODoorW.lib`; the static build produces
`ODoors-static.lib`.

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
