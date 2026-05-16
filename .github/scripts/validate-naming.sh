#!/usr/bin/env sh

set -eu

branch_name=""

while [ "$#" -gt 0 ]; do
  case "$1" in
    --branch)
      branch_name="$2"
      shift 2
      ;;
    *)
      echo "Unknown argument: $1"
      echo "Usage: $0 --branch <name>"
      exit 1
      ;;
  esac
done

if [ -z "$branch_name" ]; then
  echo "Missing required --branch argument"
  exit 1
fi

branch_regex='^(ds|algo|nix|ci|docs|bench|chore)[-/.][a-z0-9][a-z0-9-]*$'

if ! printf '%s' "$branch_name" | grep -Eq "$branch_regex"; then
  echo "Invalid branch name: $branch_name"
  echo "Expected format: <area><sep><topic>"
  echo "Allowed separators after <area>: '-', '/', '.'"
  echo "Allowed areas: ds, algo, nix, ci, docs, bench, chore"
  echo "Examples: ds-vector-push, ds/vector-push, ds.vector-push"
  exit 1
fi
