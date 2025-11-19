#!/bin/bash
ELF="$1"
EXPECTED="$2"
LOG="$3"

# Run the program; the exit code will be stored in $?
qemu-riscv64 "$ELF"
rv=$?  # capture exit status

echo "Program exit code: $rv" > "$LOG"

if [ "$rv" -ne "$EXPECTED" ]; then
    echo "Test $ELF FAILED (expected $EXPECTED)"
    exit 1
fi
