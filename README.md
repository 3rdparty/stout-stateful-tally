Provides `stout::AtomicBackoff` as well as some thread helpers (such as `stout::this_thread::pause()` which compliments `std::this_thread::yield()`).

# Bazel build rules

Follows a "repos/deps" pattern (in order to help with recursive dependencies). To use:

1. Copy `bazel/repos.bzl` into your repository at `3rdparty/stout-stateful-tally/repos.bzl` and add an empty `BUILD` (or `BUILD.bazel`) to `3rdparty/stout-stateful-tally` as well.

2. Either ... add the following to your `WORKSPACE` (or `WORKSPACE.bazel`):

```bazel
load("//3rdparty/stout-stateful-tally:repos.bzl", stout_stateful_tally_repos="repos")
stout_stateful_tally_repos()

load("@com_github_3rdparty_stout_stateful_tally//bazel:deps.bzl", stout_stateful_tally_deps="deps")
stout_stateful_tally_deps()
```

Or ... to simplify others depending on ***your*** repository, add the following to your `repos.bzl`:

```bazel
load("//3rdparty/stout-stateful-tally:repos.bzl", stout_stateful_tally="repos")

def repos():
    stout_stateful_tally()
```

And the following to your `deps.bzl`:

```bazel
load("@com_github_3rdparty_stout_stateful_tally//bazel:deps.bzl", stout_stateful_tally="deps")

def deps():
    stout_stateful_tally()
```

4. You can then use `@com_github_stout_stateful_tally//:stateful-tally` in your target's `deps`.

5. Repeat the steps starting at (1) at the desired version of this repository that you want to use.