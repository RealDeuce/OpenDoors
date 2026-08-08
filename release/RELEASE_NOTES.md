# OpenDoors 6.3.0

OpenDoors 6.3.0 is displayed traditionally by the toolkit as OpenDoors 6.30.
The first digit identifies the public API generation, the second identifies
the binary ABI generation, and the third identifies a compatible maintenance
release.

This release provides maintained CMake builds and installable package targets,
modern reference documentation, dynamically sized virtual screens, restored
DOS personality support, and tested builds for current and legacy toolchains.
See the repository `CHANGELOG.md` and the online versioning guide for details.

## SDKs

- MSVC x86 and x64 SDKs contain the shared DLL, import library, `/MD` static
  library, and `/MT` static library.
- MinGW SDKs are supplied for MINGW32/i686, MINGW64/x86_64, and
  UCRT64/x86_64. They are separate from and not link-compatible with MSVC
  libraries.
- macOS SDKs are supplied separately for x86_64 targeting macOS 10.13 and
  arm64 targeting macOS 11.0.
- DOS large-model SDKs are supplied for Open Watcom and Turbo C 2.01. The
  Turbo C compiler is not redistributed.
- The Open Watcom 32-bit DOS SDK contains `-3r` and `-3s` flat-model libraries
  plus DOS/32A-bound `-3r` examples. The libraries are also tested with
  DOS/4GW. This product uses DOS/32 Advanced DOS Extender technology.
- Reference documentation and example/personality source are supplied in one
  platform-neutral offline bundle.

Only `OpenDoor.h` and the DOS personality header `ODStat.h` are public.
Installed CMake consumers may request `OpenDoors::Shared`,
`OpenDoors::Static`, or the MSVC-only `OpenDoors::StaticMT` target.
