import os
import stat
import subprocess
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]


class UnitXvfbTests(unittest.TestCase):
    def test_explicit_watcom_root_is_exported_to_runner(self):
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary)
            marker = directory / "xvfb-started"
            watcom = directory / "watcom"
            binl = watcom / "binl"
            binl.mkdir(parents=True)
            xvfb = directory / "Xvfb"
            xdpyinfo = directory / "xdpyinfo"
            python = directory / "python3"
            xvfb.write_text(
                "#!/bin/sh\n"
                ": >\"$UNIT_XVFB_TEST_MARKER\"\n"
                "while :; do sleep 1; done\n",
                encoding="ascii")
            xdpyinfo.write_text(
                "#!/bin/sh\n"
                "test -f \"$UNIT_XVFB_TEST_MARKER\"\n",
                encoding="ascii")
            python.write_text(
                "#!/bin/sh\n"
                "test \"$WATCOM\" = \"$UNIT_XVFB_TEST_WATCOM\"\n"
                "test \"${PATH%%:*}\" = \"$WATCOM/binl\"\n",
                encoding="ascii")
            for executable in (xvfb, xdpyinfo, python):
                executable.chmod(executable.stat().st_mode | stat.S_IXUSR)
            environment = os.environ.copy()
            environment.pop("WATCOM", None)
            environment["PATH"] = str(directory) + os.pathsep + os.environ["PATH"]
            environment["UNIT_XVFB_TEST_MARKER"] = str(marker)
            environment["UNIT_XVFB_TEST_WATCOM"] = str(watcom)

            result = subprocess.run(
                [str(ROOT / "tools" / "unit-xvfb"), "--display", ":199",
                 "--watcom", str(watcom), "run", "--help"], cwd=ROOT,
                env=environment, stdout=subprocess.PIPE,
                stderr=subprocess.PIPE, timeout=5)

            self.assertEqual(result.returncode, 0, result.stderr.decode())

    def test_starts_x_server_before_dispatching_runner(self):
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary)
            marker = directory / "xvfb-started"
            xvfb = directory / "Xvfb"
            xdpyinfo = directory / "xdpyinfo"
            python = directory / "python3"
            xvfb.write_text(
                "#!/bin/sh\n"
                ": >\"$UNIT_XVFB_TEST_MARKER\"\n"
                "while :; do sleep 1; done\n",
                encoding="ascii")
            xdpyinfo.write_text(
                "#!/bin/sh\n"
                "test -f \"$UNIT_XVFB_TEST_MARKER\"\n",
                encoding="ascii")
            python.write_text(
                "#!/bin/sh\n"
                "test \"$DISPLAY\" = :197\n"
                "test -f \"$UNIT_XVFB_TEST_MARKER\"\n",
                encoding="ascii")
            xvfb.chmod(xvfb.stat().st_mode | stat.S_IXUSR)
            xdpyinfo.chmod(xdpyinfo.stat().st_mode | stat.S_IXUSR)
            python.chmod(python.stat().st_mode | stat.S_IXUSR)
            environment = os.environ.copy()
            environment["PATH"] = str(directory) + os.pathsep + os.environ["PATH"]
            environment["UNIT_XVFB_TEST_MARKER"] = str(marker)

            result = subprocess.run(
                [str(ROOT / "tools" / "unit-xvfb"), "--display", ":197",
                 "run", "--help"], cwd=ROOT, env=environment,
                stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=5)

            self.assertEqual(result.returncode, 0, result.stderr.decode())

    def test_debug_mode_runs_dosbox_on_private_display(self):
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary)
            marker = directory / "xvfb-started"
            xvfb = directory / "Xvfb"
            xdpyinfo = directory / "xdpyinfo"
            dosbox = directory / "dosbox"
            xvfb.write_text(
                "#!/bin/sh\n"
                ": >\"$UNIT_XVFB_TEST_MARKER\"\n"
                "while :; do sleep 1; done\n",
                encoding="ascii")
            xdpyinfo.write_text(
                "#!/bin/sh\n"
                "test -f \"$UNIT_XVFB_TEST_MARKER\"\n",
                encoding="ascii")
            dosbox.write_text(
                "#!/bin/sh\n"
                "test \"$DISPLAY\" = :198\n"
                "test -f \"$UNIT_XVFB_TEST_MARKER\"\n"
                "test \"$1\" = -conf\n"
                "test \"$2\" = debug.conf\n",
                encoding="ascii")
            for executable in (xvfb, xdpyinfo, dosbox):
                executable.chmod(executable.stat().st_mode | stat.S_IXUSR)
            environment = os.environ.copy()
            environment["PATH"] = str(directory) + os.pathsep + os.environ["PATH"]
            environment["UNIT_XVFB_TEST_MARKER"] = str(marker)

            result = subprocess.run(
                [str(ROOT / "tools" / "unit-xvfb"), "--display", ":198",
                 "debug", str(dosbox), "-conf", "debug.conf"], cwd=ROOT,
                env=environment, stdout=subprocess.PIPE,
                stderr=subprocess.PIPE, timeout=5)

            self.assertEqual(result.returncode, 0, result.stderr.decode())


if __name__ == "__main__":
    unittest.main()
