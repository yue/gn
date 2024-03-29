// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gn/build_settings.h"

#include <utility>

#include "base/files/file_util.h"
#include "base/strings/string_util.h"
#include "gn/filesystem_utils.h"
#include "util/exe_path.h"

BuildSettings::BuildSettings()
    : chromium_config_path_(GetExePath().DirName()),
      chromium_config_path_utf8_(FilePathToUTF8(chromium_config_path_)) {}

BuildSettings::BuildSettings(const BuildSettings& other)
    : dotfile_name_(other.dotfile_name_),
      root_path_(other.root_path_),
      root_path_utf8_(other.root_path_utf8_),
      secondary_source_path_(other.secondary_source_path_),
      chromium_config_path_(other.chromium_config_path_),
      chromium_config_path_utf8_(other.chromium_config_path_utf8_),
      python_path_(other.python_path_),
      ninja_required_version_(other.ninja_required_version_),
      use_chromium_config_(other.use_chromium_config_),
      build_config_file_(other.build_config_file_),
      arg_file_template_path_(other.arg_file_template_path_),
      build_dir_(other.build_dir_),
      build_args_(other.build_args_) {}

void BuildSettings::SetRootTargetLabel(const Label& r) {
  root_target_label_ = r;
}

void BuildSettings::SetRootPath(const base::FilePath& r) {
  DCHECK(r.value()[r.value().size() - 1] != base::FilePath::kSeparators[0]);
  root_path_ = r.NormalizePathSeparatorsTo('/');
  root_path_utf8_ = FilePathToUTF8(root_path_);
}

void BuildSettings::SetSecondarySourcePath(const SourceDir& d) {
  secondary_source_path_ = GetFullPath(d).NormalizePathSeparatorsTo('/');
}

void BuildSettings::SetBuildDir(const SourceDir& d) {
  build_dir_ = d;
}

base::FilePath BuildSettings::GetFullPath(const SourceFile& file) const {
  return file.Resolve(root_path_).NormalizePathSeparatorsTo('/');
}

base::FilePath BuildSettings::GetFullPath(const SourceDir& dir) const {
  return dir.Resolve(root_path_).NormalizePathSeparatorsTo('/');
}

base::FilePath BuildSettings::GetFullPath(const std::string& path,
                                          bool as_file) const {
  return ResolvePath(path, as_file, root_path_).NormalizePathSeparatorsTo('/');
}

base::FilePath BuildSettings::GetFullPathSecondary(
    const SourceFile& file) const {
  return file.Resolve(secondary_source_path_).NormalizePathSeparatorsTo('/');
}

base::FilePath BuildSettings::GetFullPathSecondary(const SourceDir& dir) const {
  return dir.Resolve(secondary_source_path_).NormalizePathSeparatorsTo('/');
}

base::FilePath BuildSettings::GetFullPathSecondary(const std::string& path,
                                                   bool as_file) const {
  return ResolvePath(path, as_file, secondary_source_path_)
      .NormalizePathSeparatorsTo('/');
}

base::FilePath BuildSettings::GetFullPathChromium(
    const SourceFile& file) const {
  return file.Resolve(chromium_config_path_).NormalizePathSeparatorsTo('/');
}

base::FilePath BuildSettings::GetFullPathChromium(
    const SourceDir& dir) const {
  return dir.Resolve(chromium_config_path_).NormalizePathSeparatorsTo('/');
}

base::FilePath BuildSettings::GetFullPathChromium(const std::string& path,
                                                  bool as_file) const {
  return ResolvePath(path, as_file, chromium_config_path_)
      .NormalizePathSeparatorsTo('/');
}

bool BuildSettings::IsChromiumPath(const std::string& path) const {
  return use_chromium_config() &&
      (base::StartsWith(path, "//build/", base::CompareCase::SENSITIVE) ||
       base::StartsWith(path, "//buildtools/", base::CompareCase::SENSITIVE) ||
       base::StartsWith(path, "//testing/", base::CompareCase::SENSITIVE) ||
       base::StartsWith(path, "//tools/", base::CompareCase::SENSITIVE) ||
       base::StartsWith(path, "//third_party/catapult/", base::CompareCase::SENSITIVE) ||
       base::StartsWith(path, "//third_party/googletest/", base::CompareCase::SENSITIVE) ||
       base::StartsWith(path, "//third_party/libc++/", base::CompareCase::SENSITIVE) ||
       base::StartsWith(path, "//third_party/libc++abi/", base::CompareCase::SENSITIVE) ||
       base::StartsWith(path, "//third_party/libunwind/", base::CompareCase::SENSITIVE) ||
       base::StartsWith(path, "//third_party/llvm-build/", base::CompareCase::SENSITIVE));
}

std::string BuildSettings::TranslateChromiumFullPath(
    const base::FilePath& path) const {
  std::string real_path = FilePathToUTF8(path);
  std::string relative_path_to_real_path;
  if (MakeAbsolutePathRelativeIfPossible(root_path_utf8(),
                                         real_path,
                                         &relative_path_to_real_path)) {
    return relative_path_to_real_path;
  } else {
#if defined(OS_WIN)
    return "/" + real_path;
#else
    return real_path;
#endif
  }
}

void BuildSettings::ItemDefined(std::unique_ptr<Item> item) const {
  DCHECK(item);
  if (item_defined_callback_)
    item_defined_callback_(std::move(item));
}
