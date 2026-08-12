import json
import subprocess
import sys
import tempfile
import unittest
from contextlib import redirect_stderr
from io import StringIO
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

import run as run_module  # noqa: E402
from run import (analyzer_platform_flags, coverage_disposition,
                 DEFAULT_DOS_TIMEOUT,
                 coverage_waived, default_build_path, expand_configurations,
                 dos_aux_stem, dos_batch, dos_short_stem, dosbox_arguments,
                 effective_coverage_waivers,
                 executable_suffix, native_test_arguments,
                 llvm_branch_waived, llvm_coverage_supported, native_coverage,
                 map_llvm_record_lines,
                 missing_llvm_mcdc_records,
                 original_source_line,
                 run_step, selected_tests, selection_owners,
                 watcom_ast_compatibility_flags,
                 watcom_environment, watcom_target_flags,
                 windows_fixture_arguments)  # noqa: E402


class DosObjectNamingTests(unittest.TestCase):
    def test_default_runtime_timeout_accommodates_the_full_dos_suite(self):
        self.assertEqual(DEFAULT_DOS_TIMEOUT, 900)

    def test_auxiliary_stem_never_matches_the_unit_stem(self):
        self.assertEqual(dos_aux_stem("U1234567"), "U123456A")
        self.assertEqual(dos_aux_stem("UFC1BBAA"), "UFC1BBAB")

    def test_default_build_paths_separate_cross_target_configurations(self):
        paths = {
            default_build_path("unix", "native", None),
            default_build_path("pthread", "native", None),
            default_build_path("windows", "native", "x86"),
            default_build_path("windows", "native", "x64"),
            default_build_path("dos16", "watcom16", None),
            default_build_path("dos32", "watcom32r", None),
            default_build_path("dos32", "watcom32s", None),
        }
        self.assertEqual(len(paths), 7)

    def test_runner_accepts_the_documented_omitted_build_argument(self):
        completed = subprocess.run(
            [sys.executable, str(Path(run_module.__file__)),
             "--platform", "pthread", "--function", "ODThreadSleep"],
            cwd=Path(run_module.__file__).resolve().parents[2],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        self.assertEqual(completed.returncode, 0, completed.stderr)


class TestSelectionTests(unittest.TestCase):
    def test_loads_exact_owners_from_selector_output(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "selection.json"
            path.write_text(json.dumps({
                "sources": {"first.c": ["one"], "second.c": ["two"]}
            }), encoding="utf-8")
            self.assertEqual(selection_owners(path), {
                ("first.c", "one"), ("second.c", "two")
            })

    def test_rejects_malformed_selector_ownership(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "selection.json"
            path.write_text('{"sources":{"first.c":"one"}}',
                            encoding="utf-8")
            with self.assertRaisesRegex(ValueError, "function lists"):
                selection_owners(path)

    def test_exact_owner_selection_does_not_form_a_cross_product(self):
        document = {"tests": [
            {"source": source, "function": function,
             "platforms": ["unix"]}
            for source in ("first.c", "second.c")
            for function in ("one", "two")
        ]}
        selected = list(selected_tests(
            "unix", set(), set(), None, document,
            {("first.c", "one"), ("second.c", "two")}))
        self.assertEqual(
            [(item["source"], item["function"]) for item in selected],
            [("first.c", "one"), ("second.c", "two")])

    def test_start_at_retains_boundary_and_following_platform_tests(self):
        document = {"tests": [
            {"source": "first.c", "function": "one",
             "platforms": ["windows"]},
            {"source": "second.c", "function": "two",
             "platforms": ["unix"]},
            {"source": "third.c", "function": "three",
             "platforms": ["windows"]},
            {"source": "fourth.c", "function": "four",
             "platforms": ["windows"]},
        ]}
        selected = list(selected_tests(
            "windows", set(), set(), ("third.c", "three"), document))
        self.assertEqual(
            [(item["source"], item["function"]) for item in selected],
            [("third.c", "three"), ("fourth.c", "four")])

    def test_source_platform_flags_are_merged_with_case_flags(self):
        document = {
            "source_native_flags": {
                "sample.c": {"windows": ["-Wno-source-warning"]}
            },
            "tests": [{
                "source": "sample.c", "function": "sample",
                "platforms": ["windows"],
                "configurations": [{
                    "name": "default",
                    "native_flags": ["-Wno-case-warning"]
                }]
            }]
        }
        selected = list(selected_tests(
            "windows", set(), set(), None, document))
        self.assertEqual(selected[0]["configuration"]["native_flags"],
                         ["-Wno-source-warning", "-Wno-case-warning"])


class CoverageWaiverTests(unittest.TestCase):
    def test_detects_compound_decisions_missing_from_llvm_mcdc_records(self):
        model = {"decisions": [
            {"id": 1, "line": 10, "conditions": 2},
            {"id": 2, "line": 20, "conditions": 1},
            {"id": 3, "line": 30, "conditions": 3},
        ]}
        generated = [
            '#line 1 "sample.c"',
            *['line'] * 9,
            # LLVM is authoritative about the four macro-expanded conditions.
            'compound decision',
        ]
        records = [[11, 0, 0, [True, True, True, True]]]
        self.assertEqual(missing_llvm_mcdc_records(
            model, records, generated, "sample.c"),
            [{"id": 3, "line": 30, "conditions": 3}])

    def test_maps_generated_lines_through_repeated_line_directives(self):
        generated = [
            '/* support */', '#line 1 "sample.c"', 'first', 'second',
            '#line 100', 'target',
        ]
        self.assertEqual(original_source_line(generated, 6, "sample.c"), 100)
        self.assertIsNone(original_source_line(generated, 1, "sample.c"))

    def test_normalizes_llvm_records_to_original_source_lines(self):
        generated = ['#line 40 "sample.c"', 'decision']
        records = [[2, 3, 2, 20, 1, 1, [True, True]]]
        self.assertEqual(map_llvm_record_lines(
            records, generated, "sample.c"),
            [[40, 3, 2, 20, 1, 1, [True, True]]])

    def test_approved_and_proposed_waivers_have_distinct_dispositions(self):
        entries = [{
            "source": "sample.c", "function": "sample",
            "platform": "unix", "kind": "branch",
            "start_line": 10, "end_line": 10,
        }, {
            "source": "sample.c", "function": "sample",
            "platform": "unix", "kind": "mcdc",
            "start_line": 20, "end_line": 20,
            "proposed": True,
        }]
        self.assertEqual(coverage_disposition(
            entries, "sample.c", "sample", "unix", "branch", 10),
            (True, False))
        self.assertEqual(coverage_disposition(
            entries, "sample.c", "sample", "unix", "mcdc", 20),
            (False, True))

    def test_proposals_are_used_only_when_explicitly_enabled(self):
        approved = [{"source": "approved.c", "function": "approved"}]
        proposals = {"proposals": [{
            "id": "sample-proposal",
            "status": "proposed",
            "source": "sample.c",
            "function": "sample",
            "platforms": ["unix", "dos16"],
            "kinds": ["branch", "mcdc"],
            "start_line": 10,
            "end_line": 11,
        }]}

        strict = effective_coverage_waivers(approved, proposals, False)
        permissive = effective_coverage_waivers(approved, proposals, True)

        self.assertEqual(strict, approved)
        self.assertEqual(len(permissive), 5)
        self.assertFalse(permissive[0].get("proposed", False))
        self.assertTrue(all(item["proposed"] for item in permissive[1:]))
        self.assertEqual(
            {(item["platform"], item["kind"])
             for item in permissive[1:]},
            {("unix", "branch"), ("unix", "mcdc"),
             ("dos16", "branch"), ("dos16", "mcdc")})

    def test_single_condition_mcdc_inherits_branch_waiver(self):
        waivers = [{
            "source": "sample.c",
            "function": "sample",
            "platform": "dos16",
            "kind": "branch",
            "start_line": 10,
            "end_line": 10,
        }]
        self.assertTrue(coverage_waived(
            waivers, "sample.c", "sample", "dos16", "mcdc", 10,
            conditions=1))
        self.assertFalse(coverage_waived(
            waivers, "sample.c", "sample", "dos16", "mcdc", 10,
            conditions=2))
        self.assertFalse(coverage_waived(
            waivers, "sample.c", "sample", "unix", "mcdc", 10,
            conditions=1))

    def test_llvm_condition_branch_accepts_mcdc_waiver(self):
        waivers = [{
            "source": "sample.c",
            "function": "sample",
            "platform": "unix",
            "kind": "mcdc",
            "start_line": 20,
            "end_line": 20,
        }]
        self.assertTrue(llvm_branch_waived(
            waivers, "sample.c", "sample", "unix", 20))
        self.assertFalse(coverage_waived(
            waivers, "sample.c", "sample", "unix", "branch", 20))

    def test_native_coverage_discards_duplicate_observations(self):
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary)
            model = directory / "sample.model.json"
            report = directory / "sample.cov"
            output = directory / "sample.coverage.json"
            model.write_text(json.dumps({
                "source": "sample.c",
                "function": "sample",
                "branches": [],
                "decisions": [{
                    "id": 1,
                    "line": 1,
                    "conditions": 1,
                }],
            }), encoding="utf-8")
            report.write_text(
                "D 1 1 1 0 0\n"
                "D 1 1 1 0 0\n"
                "D 1 1 1 1 1\n"
                "D 1 1 1 1 1\n",
                encoding="ascii")

            native_coverage(report, model, "unix", output)

            result = json.loads(output.read_text(encoding="utf-8"))
            self.assertEqual(len(result["decisions"][0]["vectors"]), 2)
            self.assertEqual(
                result["decisions"][0]["mcc_observed_combinations"], [0, 1])
            self.assertTrue(result["decisions"][0]["mcc_complete"])

    def test_native_coverage_requires_both_assembly_branch_outcomes(self):
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary)
            model = directory / "sample.model.json"
            report = directory / "sample.cov"
            output = directory / "sample.coverage.json"
            model.write_text(json.dumps({
                "source": "sample.asm",
                "function": "sample",
                "branches": [],
                "decisions": [],
                "assembly_branches": [{
                    "id": 1, "line": 7, "instruction": "jne"
                }],
            }), encoding="utf-8")
            report.write_text("A 1 0\n", encoding="ascii")
            with self.assertRaisesRegex(RuntimeError,
                                        "assembly branch 1 line 7"):
                native_coverage(report, model, "dos16", output)

            report.write_text("A 1 0\nA 1 1\n", encoding="ascii")
            native_coverage(report, model, "dos16", output)
            result = json.loads(output.read_text(encoding="utf-8"))
            self.assertEqual(result["assembly_branches"][0]["outcomes"],
                             [0, 1])


class WatcomASTCompatibilityTests(unittest.TestCase):
    def test_dos32_analysis_uses_a_32_bit_c_data_model(self):
        self.assertIn("-m32",
                      run_module.analyzer_platform_flags("dos32", []))

    def test_compiler_driver_selects_dos_for_compilation_and_linking(self):
        self.assertEqual(watcom_target_flags("watcom16"), ["-bcl=dos"])
        self.assertEqual(watcom_target_flags("watcom32r"),
                         ["-bt=dos", "-l=dos4g"])
        self.assertEqual(watcom_target_flags("watcom32s"),
                         ["-bt=dos", "-l=dos4g"])

    def test_uses_watcom_header_va_list_definition(self):
        flags = watcom_ast_compatibility_flags("/opt/watcom")
        self.assertFalse(any("__va_list" in flag for flag in flags))
        self.assertIn("-D__watcall=", flags)
        self.assertIn("-Dinterrupt=", flags)
        self.assertIn("-Dfar=", flags)
        self.assertIn("-fasm-blocks", flags)
        self.assertNotIn("-D_WCCALLBACK=", flags)
        self.assertEqual(flags[-2:], ["-isystem", "/opt/watcom/h"])


class LanguageStandardTests(unittest.TestCase):
    def test_modern_targets_use_the_cmake_c99_contract(self):
        self.assertEqual(run_module.language_standard_flag("unix"),
                         "-std=c99")
        self.assertEqual(run_module.language_standard_flag("pthread"),
                         "-std=c99")
        self.assertEqual(run_module.language_standard_flag("windows"),
                         "-std=c99")
        for platform in ("unix", "pthread", "windows"):
            self.assertNotIn("-pedantic",
                             run_module.native_compile_flags(platform))

    def test_permits_intentionally_partial_aggregate_initializers(self):
        for platform in ("unix", "pthread", "windows"):
            self.assertIn("-Wno-missing-field-initializers",
                          run_module.native_compile_flags(platform))

    def test_ignores_parameters_unused_by_the_isolated_body(self):
        for platform in ("unix", "pthread", "windows"):
            self.assertIn("-Wno-unused-parameter",
                          run_module.native_compile_flags(platform))

    def test_dos_analysis_remains_c89_compatible(self):
        self.assertEqual(run_module.language_standard_flag("dos16"),
                         "-std=c89")
        self.assertEqual(run_module.language_standard_flag("dos32"),
                         "-std=c89")


class WindowsASTCompatibilityTests(unittest.TestCase):
    def test_builds_a_fixture_dll_beside_the_test_executable(self):
        self.assertEqual(windows_fixture_arguments(
            "i686-w64-mingw32-gcc", Path("unit/fixtures/door32/door32.c"),
            Path("build/case/DOOR32.DLL")), [
                "i686-w64-mingw32-gcc", "-std=c99", "-Wall", "-Wextra",
                "-Werror", "-shared", "-Wl,--kill-at",
                "unit/fixtures/door32/door32.c", "-o",
                "build/case/DOOR32.DLL"])

    def test_fixture_receives_the_executable_architecture_flags(self):
        arguments = windows_fixture_arguments(
            "clang", Path("door32.c"), Path("DOOR32.DLL"), ["-m32"])
        self.assertIn("-m32", arguments)

    def test_msvc_style_clang_fixture_omits_the_gnu_export_option(self):
        arguments = windows_fixture_arguments(
            "clang", Path("door32.c"), Path("DOOR32.DLL"))
        self.assertNotIn("-Wl,--kill-at", arguments)

    def test_llvm_execution_is_limited_to_host_compatible_platforms(self):
        self.assertTrue(llvm_coverage_supported("unix"))
        self.assertTrue(llvm_coverage_supported("pthread"))
        self.assertFalse(llvm_coverage_supported("windows"))
        self.assertFalse(llvm_coverage_supported("dos16"))
        self.assertFalse(llvm_coverage_supported("dos32"))

    def test_uses_a_windows_target_for_predefined_platform_macros(self):
        flags = analyzer_platform_flags(
            "windows", ["-D_CRTIMP=", "-DDECLSPEC_IMPORT="])
        self.assertIn("-target", flags)
        target = flags[flags.index("-target") + 1]
        self.assertEqual(target, "x86_64-w64-windows-gnu")

    def test_uses_a_32_bit_windows_target_with_an_i686_compiler(self):
        flags = analyzer_platform_flags(
            "windows", ["-D_CRTIMP="], "i686-w64-mingw32-gcc")
        target = flags[flags.index("-target") + 1]
        self.assertEqual(target, "i686-w64-windows-gnu")

    def test_explicit_architecture_overrides_a_generic_compiler_name(self):
        flags = analyzer_platform_flags(
            "windows", ["-D_CRTIMP="], "clang", "x86")
        target = flags[flags.index("-target") + 1]
        self.assertEqual(target, "i686-w64-windows-gnu")

    def test_native_windows_clang_can_model_the_msvc_abi(self):
        flags = analyzer_platform_flags(
            "windows", ["-D_CRTIMP="], "clang", "x86", "msvc")
        target = flags[flags.index("-target") + 1]
        self.assertEqual(target, "i686-pc-windows-msvc")

    def test_windows_executables_retain_the_mingw_suffix(self):
        self.assertEqual(executable_suffix("windows"), ".exe")
        self.assertEqual(executable_suffix("unix"), "")
        self.assertEqual(executable_suffix("dos16"), ".EXE")

    def test_windows_native_test_can_be_launched_through_wine(self):
        arguments = native_test_arguments(
            Path("build/test.exe"), Path("build/test.cov"),
            Path("/usr/local/bin/wine64"))
        self.assertEqual(arguments, [
            "/usr/local/bin/wine64", "build/test.exe", "build/test.cov"])


class FailureAggregationTests(unittest.TestCase):
    def test_records_a_failure_and_allows_later_steps(self):
        failures = []
        calls = []

        def fail():
            calls.append("failed")
            raise RuntimeError("expected red gate")

        def pass_later():
            calls.append("continued")

        with redirect_stderr(StringIO()):
            self.assertFalse(run_step(failures, "native coverage", fail))
        self.assertTrue(run_step(failures, "LLVM coverage", pass_later))
        self.assertEqual(calls, ["failed", "continued"])
        self.assertEqual(len(failures), 1)
        self.assertIn("native coverage", failures[0])
        self.assertIn("expected red gate", failures[0])


class ConfigurationTests(unittest.TestCase):
    def test_expands_named_preprocessor_configurations(self):
        test = {
            "source": "sample.c",
            "function": "sample",
            "configurations": [
                {"name": "runtime", "defines": ["HAVE_FORMAT=1"]},
                {"name": "fallback", "undefines": ["HAVE_FORMAT"]},
            ],
        }
        expanded = list(expand_configurations(test))
        self.assertEqual([item["configuration"]["name"]
                          for item in expanded], ["runtime", "fallback"])
        self.assertEqual(expanded[0]["source"], "sample.c")
        self.assertNotIn("configuration", test)

    def test_implicit_configuration_preserves_existing_tests(self):
        expanded = list(expand_configurations({"source": "sample.c"}))
        self.assertEqual(expanded[0]["configuration"], {"name": "default"})

    def test_filters_configuration_to_its_applicable_platforms(self):
        test = {
            "source": "sample.c",
            "configurations": [
                {"name": "default"},
                {"name": "diagnostics", "defines": ["DIAGNOSTICS=1"],
                 "platforms": ["windows"]},
            ],
        }
        self.assertEqual(
            [item["configuration"]["name"]
             for item in expand_configurations(test, "unix")],
            ["default"])
        self.assertEqual(
            [item["configuration"]["name"]
             for item in expand_configurations(test, "windows")],
            ["default", "diagnostics"])


class DOSRuntimeTests(unittest.TestCase):
    def test_dosbox_uses_private_configuration_and_exits(self):
        arguments = dosbox_arguments(
            Path("/usr/bin/dosbox"), Path("/tmp/unit-build"),
            Path("/tmp/unit-build/UTRUN.BAT"),
            Path("/tmp/unit-build/dosbox.conf"))
        self.assertEqual(arguments[:6], [
            "/usr/bin/dosbox", "-noconsole", "-exit", "-conf",
            "/tmp/unit-build/dosbox.conf", "-c"])
        self.assertIn('mount c "/tmp/unit-build"', arguments)
        self.assertEqual(arguments[-4:], ["-c", "c:", "-c", "UTRUN.BAT"])

    def test_short_stems_are_deterministic_unique_83_names(self):
        first = dos_short_stem("ODPrntf.c", "od_printf", "default")
        self.assertEqual(first,
                         dos_short_stem("ODPrntf.c", "od_printf", "default"))
        self.assertNotEqual(first,
                            dos_short_stem("ODPrntf.c", "od_printf", "fallback"))
        self.assertLessEqual(len(first), 8)
        self.assertTrue(first.startswith("U"))
        self.assertTrue(first.isalnum())

    def test_batch_records_each_failure_and_continues_to_completion(self):
        text = dos_batch([("U1234567.EXE", "U1234567.COV"),
                          ("U7654321.EXE", "U7654321.COV")],
                         ["FOSSILTS.COM"])
        self.assertTrue(text.startswith("@ECHO OFF\r\nFOSSILTS.COM\r\n"))
        self.assertIn(
            "U1234567.EXE U1234567.COV >U1234567.OUT\r\n", text)
        self.assertIn("IF NOT ERRORLEVEL 1 GOTO OK0\r\n", text)
        self.assertIn("ECHO FAIL>U1234567.BAD\r\n:OK0\r\n", text)
        self.assertIn(
            "U7654321.EXE U7654321.COV >U7654321.OUT\r\n", text)
        self.assertIn("IF NOT ERRORLEVEL 1 GOTO OK1\r\n", text)
        self.assertIn("ECHO FAIL>U7654321.BAD\r\n:OK1\r\n", text)
        self.assertIn("ECHO DONE>UTDONE.OK\r\n", text)
        self.assertTrue(text.endswith("ECHO DONE>UTDONE.OK\r\nEXIT\r\n"))
        self.assertNotIn("GOTO FAILED", text)

    def test_watcom_compiler_does_not_inherit_x_display(self):
        original = {"DISPLAY": ":91", "PATH": "/bin"}
        result = watcom_environment(original, "/opt/watcom")
        self.assertNotIn("DISPLAY", result)
        self.assertEqual(result["INCLUDE"], "/opt/watcom/h")
        self.assertIn("DISPLAY", original)


if __name__ == "__main__":
    unittest.main()
