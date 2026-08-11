#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RUNS="${1:-5}"
OUT="${2:-$ROOT/.phase0/benchmark_$(date -u +%Y%m%dT%H%M%SZ).log}"
TMP_TIME="${OUT}.time"

if ! [[ "$RUNS" =~ ^[1-9][0-9]*$ ]]; then
    printf 'usage: %s [positive-runs] [output-file]\n' "$0" >&2
    exit 64
fi

mkdir -p "$(dirname "$OUT")"
: > "$OUT"

cleanup_runtime_state() {
    rm -f "$ROOT/walia.state" "$ROOT/walia.wld"
}

trap 'rm -f "$TMP_TIME"; cleanup_runtime_state' EXIT

log() {
    printf '%s\n' "$*" | tee -a "$OUT"
}

log "phase0_benchmark_timestamp=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
log "compiler=$(gcc --version | head -n1)"
log "architecture=$(uname -m)"
log "kernel=$(uname -sr)"
log "runs=$RUNS"

cleanup_runtime_state
log "--- release build ---"
(
    cd "$ROOT"
    make clean
)
/usr/bin/time -f 'build_seconds=%e\nbuild_user_seconds=%U\nbuild_system_seconds=%S\nmax_rss_kb=%M' \
    -o "$TMP_TIME" \
    bash -c 'cd "$1" && make' -- "$ROOT" 2>&1 | tee -a "$OUT"
cat "$TMP_TIME" | tee -a "$OUT"

for run in $(seq 1 "$RUNS"); do
    cleanup_runtime_state
    log "--- cold integrated test run=$run ---"
    /usr/bin/time -f "test_seconds=%e\ntest_user_seconds=%U\ntest_system_seconds=%S\nmax_rss_kb=%M" \
        -o "$TMP_TIME" \
        bash -c 'cd "$1" && "$1/walia" --test tests/core_suite.wal' -- "$ROOT" 2>&1 | tee -a "$OUT"
    cat "$TMP_TIME" | tee -a "$OUT"
done

cleanup_runtime_state
log "--- telemetry smoke run ---"
(
    cd "$ROOT"
    ./walia --telemetry --test tests/core_suite.wal
) 2>&1 | tee -a "$OUT"

log "--- benchmark complete ---"
