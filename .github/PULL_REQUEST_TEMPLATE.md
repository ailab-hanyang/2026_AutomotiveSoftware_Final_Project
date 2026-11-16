## Related IDs
- SWR: <!-- e.g. SWR-001 (required) -->
- Test cases: <!-- e.g. TC-U-003, TC-S-001 (required) -->

## Summary

## Verification
<!-- Tests run and results. Link evidence in results/ if any. -->

## Checklist
- [ ] Code implementing a requirement has `// @req SWR-xxx`
- [ ] Tests have `@verifies SWR-xxx` and the test case ID in their name (`TC_U_003_...`)
- [ ] Commit messages start with `[SWR-xxx]` or a test case ID such as `[TC-U-003]`
- [ ] `docs/traceability.csv` is updated
- [ ] `python3 tools/check_traceability.py` prints `OK`
- [ ] `docs/ai_usage_log.md` has 3 to 5 lines for this PR
