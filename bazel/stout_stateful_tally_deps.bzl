"""Load dependencies needed to compile and test the library as a 3rd-party consumer."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def stout_stateful_tally_deps():
  if "com_github_gflags_gflags" not in native.existing_rules():
    http_archive(
        name = "com_github_gflags_gflags",
        url = "https://github.com/gflags/gflags/archive/v2.2.2.tar.gz",
        sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
        strip_prefix = "gflags-2.2.2",
    )

  if "com_github_google_glog" not in native.existing_rules():
    http_archive(
        name = "com_github_google_glog",
        url = "https://github.com/google/glog/archive/v0.4.0.tar.gz",
        sha256 = "f28359aeba12f30d73d9e4711ef356dc842886968112162bc73002645139c39c",
        strip_prefix = "glog-0.4.0",
    )

  if "stout-atomic-backoff" not in native.existing_rules():
    http_archive(
       name = "stout-atomic-backoff",
       url = "https://github.com/3rdparty/stout-atomic-backoff/archive/0.1.0.tar.gz",
       sha256 = "f705ae3475aa2a5011df9bcb04a7c7425e4339449f366780c15251fa8cbd3758",
       strip_prefix = "stout-atomic-backoff-0.1.0",
    )
