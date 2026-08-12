import sys
import json
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from clang_model import Callable, Variable  # noqa: E402
from generate import (default_mock, early_mock_declarations, embedded_case,
                      generate, insert_late_defines, mock_definitions,
                      replace_c_includes,
                      variable_definition, variable_definitions)  # noqa: E402
from inventory import ROOT  # noqa: E402


class IncludedImplementationTests(unittest.TestCase):
    def test_emits_an_unmodified_boolean_tree_for_llvm_coverage(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "sample.c"
            case = root / "case.c"
            portable = root / "portable.c"
            llvm = root / "llvm.c"
            source.write_text(
                "static int target(int left, int right)\n"
                "{\n   static int calls;\n   ++calls;\n"
                "   if(left && right) return calls;\n   return 0;\n}\n",
                encoding="latin-1")
            case.write_text(
                "static void test_target(void) { UT_ASSERT(utt_target(1, 1)); }\n"
                "static const UTTestCase ut_cases[] = {{\"target\", test_target}};\n",
                encoding="latin-1")
            generate(source, "target", case, portable, "clang", ["-std=c89"],
                     uninstrumented_output=llvm)
            self.assertIn("ut_cov_condition", portable.read_text("latin-1"))
            self.assertNotIn("ut_cov_condition", llvm.read_text("latin-1"))
            self.assertIn("ut_state_register", llvm.read_text("latin-1"))
            self.assertEqual(
                json.loads(portable.with_suffix(".model.json").read_text()),
                json.loads(llvm.with_suffix(".model.json").read_text()))

    def test_replaces_quoted_c_implementation_with_matching_header(self):
        source = """\
#ifdef OLD
#include "trio/trio.c"
#else
#include "third_party/trio/trio.c"
#endif
"""
        self.assertEqual(replace_c_includes(source), """\
#ifdef OLD
#include "trio/trio.h"
#else
#include "third_party/trio/trio.h"
#endif
""")

    def test_leaves_headers_and_system_includes_unchanged(self):
        source = '#include "local.h"\n#include <runtime.c>\n'
        self.assertEqual(replace_c_includes(source), source)

    def test_embeds_case_local_headers_without_absolute_includes(self):
        case = ROOT / "unit" / "cases" / "ODGetIn" / "ODShiftSeq.c"
        result = embedded_case(case)
        self.assertNotIn('#include "common.h"', result)
        self.assertNotIn(str(case.resolve()), result)
        self.assertIn("ut_set_sequence", result)


class VariableDefinitionTests(unittest.TestCase):
    def test_preserves_function_pointer_calling_convention(self):
        variable = Variable("id", "hook", "BOOL (*)(INT)", True, "ODCALL")
        self.assertEqual(variable_definition(variable),
                         "BOOL (ODCALL *hook)(INT);")

    def test_omits_runtime_provided_external_objects(self):
        variables = [Variable("id1", "environ", "char **", False, ""),
                     Variable("id2", "od_control", "tODControl", False, "")]
        self.assertEqual(variable_definitions(variables, {"environ"}),
                         ["tODControl od_control;"])


class LateDefineTests(unittest.TestCase):
    def test_reasserts_configuration_at_the_target_signature(self):
        source = "#define outp real_outp\n\nstatic void target(void)\n{\n}\n"
        self.assertEqual(insert_late_defines(
            source, 3, ["outp=UTOutp", "FEATURE"], ["DISABLED"]),
            "#define outp real_outp\n\n"
            "#undef DISABLED\n"
            "#undef outp\n#define outp UTOutp\n"
            "#undef FEATURE\n#define FEATURE 1\n"
            "#line 3\n"
            "static void target(void)\n{\n}\n")

    def test_inserts_turbo_mock_prototypes_after_headers(self):
        source = "#include <conio.h>\n\nstatic void target(void)\n{\n}\n"
        result = insert_late_defines(
            source, 3, [], declarations=["int utm_inp(unsigned);"])
        self.assertIn("int utm_inp(unsigned);\n#line 3\nstatic", result)


class EarlyMockDeclarationTests(unittest.TestCase):
    def test_declares_errno_accessors_before_runtime_headers_use_them(self):
        self.assertEqual(early_mock_declarations({"_errno", "fopen"}),
                         ["int *utm__errno(void);"])

    def test_declares_vsnprintf_before_mingw_headers_replace_its_macro(self):
        self.assertEqual(early_mock_declarations({"vsnprintf"}), [
            "#include <stdarg.h>",
            "int utm_vsnprintf(char *, size_t, const char *, va_list);"
        ])

    def test_reasserts_dependency_aliases_after_runtime_headers(self):
        self.assertEqual(mock_definitions("target", {"__error": object()}),
                         ["target=utt_target", "__error=utm___error"])


class DefaultMockTests(unittest.TestCase):
    def test_zeroes_results_without_a_mockable_libc_dependency(self):
        function = Callable("id", "dependency", "RECT", [], False)
        result = default_mock(function, 1)
        self.assertNotIn("memset", result)
        self.assertIn("ut_result_bytes", result)


if __name__ == "__main__":
    unittest.main()
