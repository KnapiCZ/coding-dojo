#!/usr/bin/env bash
# Runs all lex tests located in exampleCodesIFJ25/lex_tests.
# Expected exit code is encoded in the filename as the number after the last
# underscore (e.g., foo_4.txt -> expects exit 4). Each test file is fed to
# ./project via standard input so no temporary copies are needed.

set -u

TEST_DIR="exampleCodesIFJ25/lex_tests"
PROJECT_BIN="../src/compiler"

# timeout per test (format accepted by `timeout`, e.g. 5s). Set TEST_TIMEOUT=0 to disable.
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
    echo "Binary ${PROJECT_BIN} not found or not executable. Run 'make' first." >&2
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

printf "${BOLD}Running lexical tests in %s (timeout=%s)${RESET}\n\n" "${TEST_DIR}" \
    "$([[ ${USE_TIMEOUT} -eq 1 ]] && echo "${TEST_TIMEOUT}" || echo "disabled")"

shopt -s nullglob
for file in "${TEST_DIR}"/*.txt; do
    ((total++))
    base="$(basename "${file}")"
    suffix="${base##*_}"
    expected="${suffix%.txt}"

    if ! [[ "${expected}" =~ ^-?[0-9]+$ ]]; then
        printf "${YELLOW}[SKIP]${RESET} %-30s reason: cannot parse expected code from filename\n" "${base}"
        ((skipped++))
        continue
    fi

    # Run scanner only (important!!)
    timed_out=0
    if [[ ${USE_TIMEOUT} -eq 1 ]]; then
        if timeout "${TEST_TIMEOUT}" "${PROJECT_BIN}"  < "${file}" > /dev/null 2>&1; then
            exit_code=0
        else
            exit_code=$?
            if [[ "${exit_code}" -eq 124 ]]; then
                exit_code=99
                timed_out=1
            fi
        fi
    else
        if "${PROJECT_BIN}" < "${file}" > /dev/null 2>&1; then
            exit_code=0
        else
            exit_code=$?
        fi
    fi

    if [[ "${exit_code}" -eq "${expected}" ]]; then
        printf "${GREEN}[PASS]${RESET} %-30s expected=%s got=%s%s\n" \
            "${base}" "${expected}" "${exit_code}" "$([[ ${timed_out} -eq 1 ]] && echo ' (timeout)' || echo '')"
        ((passed++))
    else
        printf "${RED}[FAIL]${RESET} %-30s expected=%s got=%s%s\n" \
            "${base}" "${expected}" "${exit_code}" "$([[ ${timed_out} -eq 1 ]] && echo ' (timeout)' || echo '')"
        ((failed++))
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
