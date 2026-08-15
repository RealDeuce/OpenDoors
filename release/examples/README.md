# OpenDoors examples

These examples consume an installed OpenDoors 6.3 SDK. Set
`CMAKE_PREFIX_PATH` to the extracted SDK and choose one of its exported
targets:

```sh
cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=/path/to/opendoors \
  -DOPENDOORS_EXAMPLE_VARIANT=Shared
cmake --build build
```

`Shared` and `Static` are available in every current desktop SDK.
`StaticMT` is available in MSVC SDKs and uses Microsoft's static C runtime.

The `ex_vote` example also requires Synchronet's xpdev library.
Pass `-DOPENDOORS_XPDEV_DIR=/path/to/xpdev`, or place xpdev beside this
examples directory. No other locations are searched automatically.

The `personalities` directory contains the standard DOS personality modules.
They use only `OpenDoor.h` and `ODStat.h`, and are intended as examples for
authors replacing or extending the DOS local status display. Compile them in
the same large memory model as the supplied DOS library.
