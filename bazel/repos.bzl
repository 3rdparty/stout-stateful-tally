"""Adds repostories/archives."""

########################################################################
# DO NOT EDIT THIS FILE unless you are inside the
# https://github.com/3rdparty/stout-stateful-tally repository. If you
# encounter it anywhere else it is because it has been copied there in
# order to simplify adding transitive dependencies. If you want a
# different version of stout-stateful-tally follow the Bazel build
# instructions at https://github.com/3rdparty/stout-stateful-tally.
########################################################################

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

load("//3rdparty/stout-atomic-backoff:repos.bzl", stout_atomic_backoff_repos="repos")

def repos(external = True, repo_mapping = {}):
    stout_atomic_backoff_repos(
        repo_mapping = repo_mapping
    )
    
    if external and "com_github_3rdparty_stout_stateful_tally" not in native.existing_rules():
        git_repository(
            name = "com_github_3rdparty_stout_stateful_tally",
            remote = "https://github.com/3rdparty/stout-stateful-tally",
            commit = "19e88a688df0e2bfaf976545b4f5661eec7d421d",
            shallow_since = "1629610177 +0200",
            repo_mapping = repo_mapping,
        )
