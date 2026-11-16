# 05. Test Specification (SWE.4 to SWE.6)

## Verification Measures
Every SWR is verified by at least one test case, and every test case points to an existing SWR.

| Level | ID | Folder | Process |
|---|---|---|---|
| Unit | `TC-U-xxx` | `test/unit` (gtest) | SWE.4 |
| Integration | `TC-I-xxx` | `test/integration` (launch_testing) | SWE.5 |
| Scenario | `TC-S-xxx` | `test/scenario` (evaluation node) | SWE.6 |
| Regression | `RT-xxx` | re-run of an earlier test case | SWE.4 to SWE.6 |

Result is `Pass`, `Fail`, or `Not executed` (write `Not executed` until the test is run).
`Verifies` lists the SWR IDs; integration tests may also list the COMP / IF IDs they check.
Scenario tests (`TC-S`) state the map and the mission switches turned on in `Input / Scenario`.
Regression tests (`RT`) state the re-run test case and the change, e.g. `TC-S-001 again after changing GenerateDrivingWay`.

| ID | Verifies | Level | Input / Scenario | Pass Criteria | Result |
|---|---|---|---|---|---|

## Integration Sequence
<!-- Build and launch order of the components. -->

## Selection Set
<!-- Test cases run for each release, and the ones kept for regression. -->
| Release (tag) | Test cases run | Kept for regression |
|---|---|---|
