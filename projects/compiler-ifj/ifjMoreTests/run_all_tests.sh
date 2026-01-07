#!/usr/bin/env bash
# Runs lex/syntax/semantic suites and prints only their success percentages.

set -u

LEX_SCRIPT="./run_lex_tests.sh"
SYNTAX_SCRIPT="./run_stx_tests.sh"
SEM_SCRIPT="./run_sem_tests.sh"
CODEGEN_SCRIPT="./run_cc_tests.sh"

if [[ ! -x "${LEX_SCRIPT}" || ! -x "${SYNTAX_SCRIPT}" || ! -x "${SEM_SCRIPT}" ]]; then
	echo "Required test scripts are missing or not executable." >&2
	exit 1
fi

if [[ -t 1 ]]; then
	GREEN=$'\033[32m'
	YELLOW=$'\033[33m'
	RED=$'\033[31m'
	BOLD=$'\033[1m'
	RESET=$'\033[0m'
else
	GREEN=""
	YELLOW=""
	RED=""
	BOLD=""
	RESET=""
fi

log_and_parse() {
	local label="$1"
	local script="$2"
	local tmp
	tmp="$(mktemp)"
	local status=0
	if ! "${script}" >"${tmp}" 2>&1; then
		status=$?
	fi
	local summary
	summary="$(perl -pe 's/\e\[[0-9;]*m//g' "${tmp}" | grep 'Summary:' | tail -n 1)"
	rm -f "${tmp}"
	if [[ -z "${summary}" ]]; then
		echo "${label}" 0 0
		return
	fi
	if [[ "${summary}" =~ Summary:\ ([0-9]+)\ total\ \|\ ([0-9]+)\ passed ]]; then
		echo "${label}" "${BASH_REMATCH[1]}" "${BASH_REMATCH[2]}"
	else
		echo "${label}" 0 0
	fi
}

print_percent() {
	local label="$1"
	local total="$2"
	local passed="$3"
	local percent="0.0"
	if [[ "${total}" -gt 0 ]]; then
		percent=$(awk -v p="${passed}" -v t="${total}" 'BEGIN { printf "%.1f", (p*100.0)/t }')
	fi
	local color="${GREEN}"
	if (( $(awk -v pct="${percent}" 'BEGIN {print (pct < 90)}') )); then
		color="${YELLOW}"
	fi
	if (( $(awk -v pct="${percent}" 'BEGIN {print (pct < 70)}') )); then
		color="${RED}"
	fi
	printf "%s%-10s%s %s%5.1f%%%s\n" "${BOLD}" "${label}:" "${RESET}" "${color}" "${percent}" "${RESET}"
}

lex_data=($(log_and_parse "Lexical" "${LEX_SCRIPT}"))
syn_data=($(log_and_parse "Syntax" "${SYNTAX_SCRIPT}"))
sem_data=($(log_and_parse "Semantic" "${SEM_SCRIPT}"))

print_percent "${lex_data[0]}" "${lex_data[1]:-0}" "${lex_data[2]:-0}"
print_percent "${syn_data[0]}" "${syn_data[1]:-0}" "${syn_data[2]:-0}"
print_percent "${sem_data[0]}" "${sem_data[1]:-0}" "${sem_data[2]:-0}"

echo "CodeGen: "
${CODEGEN_SCRIPT};
