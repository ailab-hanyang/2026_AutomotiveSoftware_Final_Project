#!/usr/bin/env python3
"""Save the evaluation result of one scenario test run (TC-S) to results/<TC-ID>/run_<n>.txt.

usage (from the repository root, while the simulator is still running after the run ends):
    source install/setup.bash
    python3 tools/save_evaluation.py TC-S-001

Every TC-S is run 3 times; run it once after each run. The files are numbered run_1.txt, run_2.txt, ...
The file also records the time and the git commit, so that a run can be repeated on the same code.
"""
import datetime
import re
import subprocess
import sys
from pathlib import Path

import rclpy
from rviz_2d_overlay_msgs.msg import OverlayText
from std_msgs.msg import Bool

ROOT = Path(__file__).resolve().parent.parent


def main():
    if len(sys.argv) != 2 or not re.fullmatch(r"TC-S-\d{3}", sys.argv[1]):
        sys.exit("usage: python3 tools/save_evaluation.py TC-S-001")
    tc_id = sys.argv[1]

    rclpy.init()
    node = rclpy.create_node("save_evaluation")
    got = {}
    node.create_subscription(OverlayText, "/text_evaluation_result", lambda m: got.__setitem__("text", m.text), 10)
    node.create_subscription(Bool, "/ego/track_end", lambda m: got.__setitem__("end", m.data), 10)
    end = datetime.datetime.now() + datetime.timedelta(seconds=5)
    while datetime.datetime.now() < end and not ("text" in got and "end" in got):
        rclpy.spin_once(node, timeout_sec=0.1)
    node.destroy_node()
    rclpy.shutdown()

    if "text" not in got:
        sys.exit("no message on /text_evaluation_result within 5 s: is the evaluation node running?")
    if not got.get("end", False):
        print("WARNING: the vehicle has not reached the track end yet (/ego/track_end is false)")

    folder = ROOT / "results" / tc_id
    folder.mkdir(parents=True, exist_ok=True)
    n = 1
    while (folder / f"run_{n}.txt").exists():
        n += 1
    commit = subprocess.run(["git", "rev-parse", "--short", "HEAD"], cwd=ROOT,
                            capture_output=True, text=True).stdout.strip() or "unknown"
    path = folder / f"run_{n}.txt"
    path.write_text(f"test case: {tc_id}\nrun: {n}\n"
                    f"saved: {datetime.datetime.now():%Y-%m-%d %H:%M:%S}\ngit commit: {commit}\n"
                    f"{got['text']}\n", encoding="utf-8")
    print(f"saved {path.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
