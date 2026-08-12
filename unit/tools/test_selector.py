#!/usr/bin/env python3

import sys
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parent))
from selector import (case_source, containing_functions, includes,
                      fixture_owners, select, test_owner, unix_variants,
                      watcom_variants)  # noqa: E402


class SelectorTests(unittest.TestCase):
    SOURCE = """\
#include "one.h"
static int first(int value)
{
   return value + 1;
}

static int second(void)
{
#if FEATURE
   return 2;
#else
   return 3;
#endif
}
"""

    def test_body_hunk_selects_one_function(self):
        self.assertEqual(containing_functions(self.SOURCE, [(4, 4)]),
                         {"first"})

    def test_signature_and_file_scope_hunks_fall_back(self):
        self.assertIsNone(containing_functions(self.SOURCE, [(2, 2)]))
        self.assertIsNone(containing_functions(self.SOURCE, [(1, 1)]))

    def test_preprocessor_hunk_inside_function_falls_back(self):
        self.assertIsNone(containing_functions(self.SOURCE, [(9, 9)]))

    def test_local_include_is_resolved_relative_to_owner(self):
        self.assertEqual(includes('#include "detail/item.h"\n', "src/a.c"),
                         {"src/detail/item.h"})

    def test_case_owner_supports_c_and_assembler_sources(self):
        self.assertEqual(test_owner("unit/cases/ODAuto/od_autodetect.c"),
                         ("ODAuto.c", "od_autodetect"))
        self.assertEqual(test_owner("unit/cases/ODSwap/__xspawn.c"),
                         ("ODSwap.asm", "__xspawn"))

    def test_shared_case_support_selects_its_complete_source(self):
        self.assertEqual(case_source("unit/cases/ODLog/errno_mock.h"),
                         "ODLog.c")
        self.assertIsNone(case_source("unit/cases/unknown/helper.h"))

    def test_fixture_change_selects_each_registered_consumer(self):
        tests = [
            {"source": "ODCom.c", "function": "ODComOpen",
             "configurations": [{
                 "name": "door32",
                 "windows_fixture": "unit/fixtures/door32/door32.c"}]},
            {"source": "ODCom.c", "function": "ODComGetByte",
             "configurations": [{
                 "name": "fossil",
                 "dos_tsr_fixture": "tests/fossil_tsr.asm"}]},
            {"source": "ODCom.c", "function": "unrelated"},
        ]
        self.assertEqual(
            fixture_owners("unit/fixtures/door32/door32.c", tests),
            {("ODCom.c", "ODComOpen")})
        self.assertEqual(
            fixture_owners("tests/fossil_tsr.asm", tests),
            {("ODCom.c", "ODComGetByte")})

    def test_unix_matrix_contains_only_selected_variants(self):
        self.assertEqual(unix_variants({"pthread": ["ODPlat.c"]}),
                         {"platform": ["pthread"]})
        self.assertEqual(
            unix_variants({"unix": ["ODLog.c"], "pthread": ["ODPlat.c"]}),
            {"platform": ["unix", "pthread"]})

    def test_watcom_matrix_contains_only_selected_memory_models(self):
        self.assertEqual(watcom_variants({"dos16": ["ODSwap.asm"]}), {
            "include": [{"target": "dos16-large", "platform": "dos16",
                         "toolchain": "watcom16"}]
        })
        self.assertEqual(watcom_variants({"dos32": ["OD32DPMI.c"]}), {
            "include": [
                {"target": "dos32-register", "platform": "dos32",
                 "toolchain": "watcom32r"},
                {"target": "dos32-stack", "platform": "dos32",
                 "toolchain": "watcom32s"},
            ]
        })

    def test_selects_only_the_function_owning_a_safe_source_hunk(self):
        inventory = {"sources": [{
            "path": "sample.c", "platforms": ["unix", "pthread", "windows"],
            "functions": [{"name": "one"}, {"name": "two"}]
        }]}
        tests = [{"source": "sample.c", "function": "one",
                  "platforms": ["unix", "windows"]},
                 {"source": "sample.c", "function": "two",
                  "platforms": ["pthread"]}]
        with patch("selector.build_inventory", return_value=inventory), \
                patch("selector.registered_tests", return_value=tests), \
                patch("selector.changed_files", return_value=["sample.c"]), \
                patch("selector.source_functions", return_value={"two"}):
            result = select("base", "head", False)
        self.assertEqual(result["sources"], {"sample.c": ["two"]})
        self.assertEqual(result["platforms"], {"pthread": ["sample.c"]})

    def test_header_change_selects_each_transitive_source_owner(self):
        inventory = {"sources": [{
            "path": "first.c", "platforms": ["unix"],
            "functions": [{"name": "one"}]
        }, {
            "path": "second.c", "platforms": ["windows"],
            "functions": [{"name": "two"}]
        }]}
        closures = {"first.c": {"shared.h"}, "second.c": set()}
        tests = [{"source": "first.c", "function": "one",
                  "platforms": ["unix"]},
                 {"source": "second.c", "function": "two",
                  "platforms": ["windows"]}]
        with patch("selector.build_inventory", return_value=inventory), \
                patch("selector.registered_tests", return_value=tests), \
                patch("selector.changed_files", return_value=["shared.h"]), \
                patch("selector.include_closure",
                      side_effect=lambda source, revision: closures[source]):
            result = select("base", "head", False)
        self.assertEqual(result["sources"], {"first.c": ["one"]})

    def test_selector_maps_a_changed_fixture_to_exact_owners(self):
        inventory = {"sources": [{
            "path": "sample.c", "platforms": ["windows", "dos16"],
            "functions": [{"name": "door"}, {"name": "fossil"}]
        }]}
        tests = [
            {"source": "sample.c", "function": "door",
             "platforms": ["windows"], "configurations": [{
                 "name": "fixture", "windows_fixture": "fixture.c"}]},
            {"source": "sample.c", "function": "fossil",
             "platforms": ["dos16"], "configurations": [{
                 "name": "fixture", "dos_tsr_fixture": "fossil.asm"}]},
        ]
        with patch("selector.build_inventory", return_value=inventory), \
                patch("selector.registered_tests", return_value=tests), \
                patch("selector.changed_files", return_value=["fixture.c"]):
            result = select("base", "head", False)
        self.assertEqual(result["sources"], {"sample.c": ["door"]})
        self.assertEqual(result["platforms"], {"windows": ["sample.c"]})

    def test_build_manifest_change_selects_the_complete_inventory(self):
        inventory = {"sources": [{
            "path": "sample.c", "platforms": ["unix"],
            "functions": [{"name": "one"}, {"name": "two"}]
        }]}
        tests = [
            {"source": "sample.c", "function": name,
             "platforms": ["unix"]}
            for name in ("one", "two")
        ]
        with patch("selector.build_inventory", return_value=inventory), \
                patch("selector.registered_tests", return_value=tests), \
                patch("selector.changed_files", return_value=["CMakeLists.txt"]):
            result = select("base", "head", False)
        self.assertEqual(result["mode"], "unit-infrastructure")
        self.assertEqual(result["sources"], {"sample.c": ["one", "two"]})


if __name__ == "__main__":
    unittest.main()
