#!/bin/bash

PROGRAM="./frontend/frontend"
TEST_PATH="../frontend/tests/end_to_end/test_logical_operators/logical_operators.txt"

out=$("$PROGRAM" "$TEST_PATH")

norm=$(printf "%s" "$out" | tr -s '[:space:]' ' ' | sed 's/^ //; s/ $//')

if [ "$norm" = "1 0 1 1 1 1 1 1 1" ]; then
  echo "test_logical_operatorsr success"
  exit 0
else
  echo "test_logical_operators fail"
  exit 1
fi