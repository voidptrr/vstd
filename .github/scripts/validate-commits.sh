#!/usr/bin/env sh

set -eu

if [ "$#" -gt 0 ]; then
  echo "Usage: $0"
  echo "Reads BASE_SHA and HEAD_SHA from environment."
  exit 1
fi

base_sha="${BASE_SHA:-}"
head_sha="${HEAD_SHA:-}"

if [ -z "$base_sha" ] || [ -z "$head_sha" ]; then
  echo "Missing BASE_SHA or HEAD_SHA environment variables"
  exit 1
fi

if ! git rev-parse --verify --quiet "$head_sha^{commit}" >/dev/null; then
  echo "Invalid HEAD_SHA commit: $head_sha"
  exit 1
fi

if ! git rev-parse --verify --quiet "$base_sha^{commit}" >/dev/null; then
  echo "BASE_SHA not found locally: $base_sha"
  echo "Falling back to origin/main for merge base calculation"
  if ! git rev-parse --verify --quiet "origin/main^{commit}" >/dev/null; then
    echo "origin/main is not available locally"
    exit 1
  fi
  base_sha="origin/main"
fi

merge_base="$(git merge-base "$base_sha" "$head_sha")"
commit_range="$merge_base..$head_sha"

commit_regex='^(ds|algo|nix|ci|docs|bench|chore)(\([a-z0-9][a-z0-9_-]*\))?: [a-z0-9].+$'

merge_commit_count="$(git rev-list --count --merges "$commit_range")"
if [ "$merge_commit_count" -ne 0 ]; then
  echo "Merge commits are not allowed in PR branches."
  echo "Found $merge_commit_count merge commit(s) in range: $commit_range"
  exit 1
fi

invalid_subjects="$({
  git log --format=%s "$commit_range" | while IFS= read -r subject; do
    if ! printf '%s' "$subject" | grep -Eq "$commit_regex"; then
      printf '%s\n' "$subject"
    fi
  done
} || true)"

if [ -n "$invalid_subjects" ]; then
  echo "Invalid commit subjects found in range: $commit_range"
  printf '%s\n' "$invalid_subjects"
  echo "Expected format: <area>(<specific>): <summary> or <area>: <summary>"
  echo "Allowed areas: ds, algo, nix, ci, docs, bench, chore"
  echo "Examples:"
  echo "  ds(vector): add push operation"
  echo "  docs: update vector API notes"
  echo "  nix(checks): split build and format checks"
  echo "  chore: format linked_list files"
  exit 1
fi
