#!/usr/bin/env python3
"""Checks the rosbag criteria of TC-S-001 (docs/05_test_specification.md) for one run.

usage (from the repository root, after colcon build):
    source install/setup.bash
    python3 test/scenario/check_rosbag.py results/TC-S-001/rosbag/run_1

Record the run with
    ros2 bag record -o results/TC-S-001/rosbag/run_<n> /ego/vehicle_state /ego/track_end /ego/vehicle_command \
        /ego/computation_time/perception /ego/computation_time/control

speed (SWR-002): speed 10 +/- 1 m/s from 10 s after the vehicle starts to the track end
    (the track end is the first /ego/track_end = true of the evaluation node).
timing (SWR-003): computation time <= period in every message of both computation_time topics.
command (SWR-004): accel and brake in [0, 1] in every vehicle command.
"""
import sys

import rosbag2_py
from rclpy.serialization import deserialize_message
from rosidl_runtime_py.utilities import get_message


def read(path, topics):
    reader = rosbag2_py.SequentialCompressionReader()  # also reads bags recorded with --compression-mode file
    reader.open(rosbag2_py.StorageOptions(uri=path, storage_id='sqlite3'),
                rosbag2_py.ConverterOptions('cdr', 'cdr'))
    types = {t.name: get_message(t.type) for t in reader.get_all_topics_and_types()}
    out = {t: [] for t in topics}
    while reader.has_next():
        topic, data, stamp = reader.read_next()
        if topic in out:
            out[topic].append((stamp * 1e-9, deserialize_message(data, types[topic])))
    return out


def check_speed(data):
    states = data['/ego/vehicle_state']
    end = next((t for t, m in data['/ego/track_end'] if m.data), None)
    if end is None:
        print('speed: the vehicle did not reach the track end -> Fail')
        return False
    start = next(t for t, m in states if m.velocity > 0.1)
    window = [m.velocity for t, m in states if start + 10.0 <= t <= end]
    low, high = min(window), max(window)
    ok = 9.0 <= low and high <= 11.0
    print('speed from 10.0 s after the start to the track end (%.1f s): min %.2f m/s, max %.2f m/s -> %s'
          % (end - start, low, high, 'Pass' if ok else 'Fail'))
    return ok


def check_timing(data):
    ok = True
    for name in ('perception', 'control'):
        msgs = [m for _, m in data['/ego/computation_time/' + name]]
        if not msgs:
            print('timing %s: no messages -> Fail' % name)
            ok = False
            continue
        late = sum(m.computation_time > m.period for m in msgs)
        longest = max(m.computation_time for m in msgs)
        print('timing %s: %d periods of %.1f ms, longest %.3f ms, %d over the period -> %s'
              % (name, len(msgs), msgs[0].period, longest, late, 'Pass' if late == 0 else 'Fail'))
        ok = ok and late == 0
    return ok


def check_command(data):
    cmds = [m for _, m in data['/ego/vehicle_command']]
    bad = sum(not (0.0 <= m.accel <= 1.0 and 0.0 <= m.brake <= 1.0) for m in cmds)
    ok = len(cmds) > 0 and bad == 0
    print('command: %d vehicle commands, %d with accel or brake outside [0, 1] -> %s'
          % (len(cmds), bad, 'Pass' if ok else 'Fail'))
    return ok


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    bag = read(sys.argv[1], ['/ego/vehicle_state', '/ego/track_end', '/ego/vehicle_command',
                             '/ego/computation_time/perception', '/ego/computation_time/control'])
    results = [check_speed(bag), check_timing(bag), check_command(bag)]
    print('TC-S-001 rosbag criteria:', 'Pass' if all(results) else 'Fail')
    sys.exit(0 if all(results) else 1)
