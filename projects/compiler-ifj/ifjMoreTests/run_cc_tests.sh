#!/usr/bin/env bash
# run_tests.sh - run tests in TEST_DIR where filenames end in _<num>
# Requires environment variables: TEST_DIR, PROJECT_BIN, INTERPRETER_BIN, OUT_FILE

set -u
# required variables
TEST_DIR="exampleCodesIFJ25/codegen_tests"
PROJECT_BIN="../src/compiler"
INTERPRETER_BIN="./tools/ic25int-linux-x86_64"
OUT_FILE="./tools/output.ifjcode25"

# sanity checks
if [ ! -d "$TEST_DIR" ]; then
  echo "ERROR: TEST_DIR '$TEST_DIR' is not a directory." >&2
  exit 2
fi
if [ ! -x "$PROJECT_BIN" ]; then
  echo "ERROR: PROJECT_BIN '$PROJECT_BIN' not found or not executable." >&2
  exit 2
fi
if [ ! -x "$INTERPRETER_BIN" ]; then
  echo "ERROR: INTERPRETER_BIN '$INTERPRETER_BIN' not found or not executable." >&2
  exit 2
fi

fail_count=0
processed_count=0
skipped_count=0

# find files (non-recursive), handle arbitrary filenames safely
while IFS= read -r -d '' file; do
  base="$(basename "$file")"

  # Only process files that end with .txt and have a suffix _<num> before .txt
  # Extract the name without the .txt suffix (literal .txt)
  case "$base" in
    *.txt) noext="${base%.txt}" ;;
    *)
      skipped_count=$((skipped_count + 1))
      continue
      ;;
  esac

  # expected exit code is the part after the last underscore in noext
  expected_raw="${noext##*_}"

  # if there was no underscore (no change), skip
  if [ "$expected_raw" = "$noext" ]; then
    echo "SKIP: '$base' — missing '_<num>.txt' suffix." >&2
    skipped_count=$((skipped_count + 1))
    continue
  fi

  # validate expected is a sequence of digits
  if ! [[ "$expected_raw" =~ ^[0-9]+$ ]]; then
    echo "SKIP: '$base' — suffix '_$expected_raw' is not a non-negative integer." >&2
    skipped_count=$((skipped_count + 1))
    continue
  fi

  # parse expected in base 10 to avoid octal interpretation on leading zeros
  expected=$((10#$expected_raw))

  processed_count=$((processed_count + 1))

  echo "----"
  echo "TEST: $base    expected exit: $expected"

  # Run project (redirect input -> OUT_FILE). Note: we continue even if PROJECT_BIN returns nonzero.
  if ! "$PROJECT_BIN" < "$file" > "$OUT_FILE"; then
    echo "NOTE: ${PROJECT_BIN} exited non-zero for '$base' (continuing to interpreter)." >&2
  fi

  # Run interpreter on the generated output file and capture its exit code
  "$INTERPRETER_BIN" "$OUT_FILE"
  rc=$?

  echo ""
  if [ "$rc" -eq "$expected" ]; then
    echo "PASS: '$base' (rc=$rc)"
  else
    echo "FAIL: '$base' (got $rc, expected $expected)"
    fail_count=$((fail_count + 1))
  fi

done < <(find "$TEST_DIR" -maxdepth 1 -type f -print0 | sort -z)

echo "----"
if [ "$processed_count" -eq 0 ]; then
  echo "No test files matching '*_<num>.txt' were found in '$TEST_DIR'." >&2
  echo "Skipped $skipped_count other files." >&2
  exit 2
fi

if [ "$fail_count" -eq 0 ]; then
  echo "All $processed_count tests passed (skipped $skipped_count files)."
  exit 0
else
  echo "$fail_count of $processed_count tests failed (skipped $skipped_count files)."
  exit 1
fi
