#!/usr/bin/env bash
# Runs all generator tests located in tests/generator_tests.
# A test case 'foo' consists of:
#   - foo.wren (the source code)
#   - foo.in   (stdin for the interpreter)
#   - foo.out  (expected stdout from the interpreter)

set -u

TEST_DIR="exampleCodesIFJ25/gen_tests"
PROJECT_BIN="../src/compiler"
INTERPRETER_BIN="tools/ic25int-linux-x86_64"

# timeout per test (format accepted by `timeout`, e.g., 5s). Set TEST_TIMEOUT=0 to disable.
TEST_TIMEOUT="${TEST_TIMEOUT:-5s}"
USE_TIMEOUT=1
if [[ "${TEST_TIMEOUT}" == "0" ]]; then
	USE_TIMEOUT=0
else
	if ! command -v timeout >/dev/null 2>&1; then
		echo "Utility 'timeout' not found but TEST_TIMEOUT is enabled." >&2
		exit 1
	fi
fi

# simple ANSI colors (disabled when stdout is not a TTY)
if [[ -t 1 ]]; then
	GREEN=$'\033[32m'
	RED=$'\033[31m'
	YELLOW=$'\033[33m'
	BOLD=$'\033[1m'
	RESET=$'\033[0m'
else
	GREEN=""
	RED=""
	YELLOW=""
	BOLD=""
	RESET=""
fi

if [[ ! -x "${PROJECT_BIN}" ]]; then
	echo "Compiler ${PROJECT_BIN} not found or not executable. Run 'make' first." >&2
	exit 1
fi

if [[ ! -x "${INTERPRETER_BIN}" ]]; then
	echo "Interpreter ${INTERPRETER_BIN} not found or not executable." >&2
	exit 1
fi

if [[ ! -d "${TEST_DIR}" ]]; then
	echo "Test directory ${TEST_DIR} not found." >&2
	exit 1
fi

total=0
passed=0
failed=0
skipped=0

printf "${BOLD}Running generator tests in %s (timeout=%s)${RESET}\n\n" "${TEST_DIR}" \
	"$([[ ${USE_TIMEOUT} -eq 1 ]] && echo "${TEST_TIMEOUT}" || echo "disabled")"

# Create temporary files for the generated code and output
TMP_CODE=$(mktemp)
TMP_OUT=$(mktemp)
# Ensure cleanup on exit
trap "rm -f '${TMP_CODE}' '${TMP_OUT}'" EXIT

shopt -s nullglob
for src_file in "${TEST_DIR}"/*.wren; do
	((total++))
	base="$(basename "${src_file}" .wren)"
	test_name="${base}"

	# Define all required files
	in_file="${TEST_DIR}/${test_name}.in"
	out_file="${TEST_DIR}/${test_name}.out"

	# Check if all required files exist
	if [[ ! -f "${in_file}" ]]; then
		printf "${YELLOW}[SKIP]${RESET} %-30s reason: missing %s.in\n" "${test_name}" "${test_name}"
		((skipped++))
		continue
	fi
	if [[ ! -f "${out_file}" ]]; then
		printf "${YELLOW}[SKIP]${RESET} %-30s reason: missing %s.out\n" "${test_name}" "${test_name}"
		((skipped++))
		continue
	fi

	# 1. COMPILE
	compile_output=$("${PROJECT_BIN}" --test-codegen "${src_file}" 2>&1)
	compile_exit_code=$?
	
	# Save generated code if compilation succeeded
	if [[ ${compile_exit_code} -eq 0 ]]; then
		echo "${compile_output}" > "${TMP_CODE}"
	else
		# On error, save error message and show it
		echo "${compile_output}" > "${TMP_CODE}"
		printf "${RED}[FAIL]${RESET} %-30s reason: ${BOLD}Compile Error${RESET} (code %s)\n" "${test_name}" "${compile_exit_code}"
		# Show why the compiler failed
		echo "${compile_output}" | awk '{print "       | " $0}' >&2
		((failed++))
		continue
	fi

	# 2. INTERPRET (with timeout)
	timed_out=0
	inter_err_msg=""
	if [[ ${USE_TIMEOUT} -eq 1 ]]; then
		inter_err_msg=$(timeout "${TEST_TIMEOUT}" "${INTERPRETER_BIN}" "${TMP_CODE}" < "${in_file}" > "${TMP_OUT}" 2>&1)
		inter_exit_code=$?
		if [[ "${inter_exit_code}" -eq 124 ]]; then
			timed_out=1
		fi
	else
		inter_err_msg=$("${INTERPRETER_BIN}" "${TMP_CODE}" < "${in_file}" > "${TMP_OUT}" 2>&1)
		inter_exit_code=$?
	fi

	if [[ ${timed_out} -eq 1 ]]; then
		printf "${RED}[FAIL]${RESET} %-30s reason: ${BOLD}Interpreter Timeout${RESET}\n" "${test_name}"
		((failed++))
		continue
	fi

	if [[ ${inter_exit_code} -ne 0 ]]; then
		printf "${RED}[FAIL]${RESET} %-30s reason: ${BOLD}Runtime Error${RESET} (code %s)\n" "${test_name}" "${inter_exit_code}"
		# Show the interpreter's error message
		echo "${inter_err_msg}" | awk '{print "       | " $0}' >&2
		((failed++))
		continue
	fi

	# 3. COMPARE
	if ! diff -u "${out_file}" "${TMP_OUT}" > /dev/null 2>&1; then
		printf "${RED}[FAIL]${RESET} %-30s reason: ${BOLD}Wrong Output${RESET}\n" "${test_name}"
		((failed++))
		# You can uncomment this to see the diff
		diff -u "${out_file}" "${TMP_OUT}" | awk '{print "       | " $0}' >&2
	else
		printf "${GREEN}[PASS]${RESET} %-30s\n" "${test_name}"
		((passed++))
	fi
done
shopt -u nullglob

summary_color="${GREEN}"
fail_color="${RED}"
(( failed > 0 )) && summary_color="${RED}"
(( failed == 0 )) && fail_color="${GREEN}"
skip_color="${YELLOW}"

printf "\n${summary_color}Summary:${RESET} %d total | ${GREEN}%d passed${RESET} | ${fail_color}%d failed${RESET} | ${skip_color}%d skipped${RESET}\n" \
	"${total}" "${passed}" "${failed}" "${skipped}"

(( failed == 0 )) || exit 1
exit 0