# Public integration acceptance tests

These tests are downstream consumers of the installed OpenDoors package. They
include only `OpenDoor.h` and `ODStat.h`; implementation headers and private
test hooks are deliberately unavailable.

`public-contract.json` inventories every public function, global, type,
constant, compatibility alias, personality helper, and `tODControl` field. Run
the checker after changing either public header:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tools/check_public_contract.py
```

`generate_public_contract.py` prints a replacement manifest for review. It
does not modify the tree. Coverage assignments should be reviewed along with
the implementation and reference documentation rather than accepted merely
because they were generated.

To exercise an installed shared package, use:

```sh
cmake -S tests/acceptance -B build/acceptance \
  -DCMAKE_PREFIX_PATH=/path/to/opendoors \
  -DOPENDOORS_VARIANT=Shared \
  -DOPENDOORS_ACCEPTANCE_EXTENDED=ON
cmake --build build/acceptance
ctest --test-dir build/acceptance --output-on-failure
```

The ordinary suite contains deterministic compile, ABI, lifecycle, screen,
configuration, spawn, and drop-file scenarios. The extended setting adds the
external socket peer used for interactive remote input and output. GitHub
Actions runs the ordinary cases on pushes and pull requests, runs the extended
matrix nightly or on request, and requires it for releases.

Acceptance expectations describe the behavior of the current implementation.
Problems exposed while characterizing that behavior are recorded in
[`ISSUES.md`](../../ISSUES.md); the test suite does not introduce an unreviewed behavior change to
make a preferred expectation pass.
