// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gn/scope_per_file_provider.h"

#include <memory>

#include "gn/filesystem_utils.h"
#include "gn/settings.h"
#include "gn/source_file.h"
#include "gn/value.h"
#include "gn/variables.h"

#include "last_commit_position.h"

ScopePerFileProvider::ScopePerFileProvider(Scope* scope, bool allow_target_vars)
    : ProgrammaticProvider(scope), allow_target_vars_(allow_target_vars) {}

ScopePerFileProvider::~ScopePerFileProvider() = default;

const Value* ScopePerFileProvider::GetProgrammaticValue(
    std::string_view ident) {
  if (ident == variables::kCurrentToolchain)
    return GetCurrentToolchain();
  if (ident == variables::kDefaultToolchain)
    return GetDefaultToolchain();
  if (ident == variables::kGnVersion)
    return GetGnVersion();
  if (ident == variables::kPythonPath)
    return GetPythonPath();

  if (ident == variables::kRootBuildDir)
    return GetRootBuildDir();
  if (ident == variables::kRootGenDir)
    return GetRootGenDir();
  if (ident == variables::kRootOutDir)
    return GetRootOutDir();

  if (ident == "use_chromium_config")
    return GetUseChromiumConfig();
  if (ident == "chromium_config_dir")
    return GetChromiumConfigDir();

  if (allow_target_vars_) {
    if (ident == variables::kTargetGenDir)
      return GetTargetGenDir();
    if (ident == variables::kTargetOutDir)
      return GetTargetOutDir();
  }
  return nullptr;
}

const Value* ScopePerFileProvider::GetCurrentToolchain() {
  if (!current_toolchain_) {
    current_toolchain_ = std::make_unique<Value>(
        nullptr,
        scope_->settings()->toolchain_label().GetUserVisibleName(false));
  }
  return current_toolchain_.get();
}

const Value* ScopePerFileProvider::GetDefaultToolchain() {
  if (!default_toolchain_) {
    default_toolchain_ = std::make_unique<Value>(
        nullptr,
        scope_->settings()->default_toolchain_label().GetUserVisibleName(
            false));
  }
  return default_toolchain_.get();
}

const Value* ScopePerFileProvider::GetGnVersion() {
  if (!gn_version_) {
    gn_version_ = std::make_unique<Value>(
        nullptr, static_cast<int64_t>(LAST_COMMIT_POSITION_NUM));
  }
  return gn_version_.get();
}

const Value* ScopePerFileProvider::GetPythonPath() {
  if (!python_path_) {
    python_path_ = std::make_unique<Value>(
        nullptr,
        FilePathToUTF8(scope_->settings()->build_settings()->python_path()));
  }
  return python_path_.get();
}

const Value* ScopePerFileProvider::GetRootBuildDir() {
  if (!root_build_dir_) {
    root_build_dir_ = std::make_unique<Value>(
        nullptr, DirectoryWithNoLastSlash(
                     scope_->settings()->build_settings()->build_dir()));
  }
  return root_build_dir_.get();
}

const Value* ScopePerFileProvider::GetRootGenDir() {
  if (!root_gen_dir_) {
    root_gen_dir_ = std::make_unique<Value>(
        nullptr, DirectoryWithNoLastSlash(GetBuildDirAsSourceDir(
                     BuildDirContext(scope_), BuildDirType::GEN)));
  }
  return root_gen_dir_.get();
}

const Value* ScopePerFileProvider::GetRootOutDir() {
  if (!root_out_dir_) {
    root_out_dir_ = std::make_unique<Value>(
        nullptr, DirectoryWithNoLastSlash(GetScopeCurrentBuildDirAsSourceDir(
                     scope_, BuildDirType::TOOLCHAIN_ROOT)));
  }
  return root_out_dir_.get();
}

const Value* ScopePerFileProvider::GetTargetGenDir() {
  if (!target_gen_dir_) {
    target_gen_dir_ = std::make_unique<Value>(
        nullptr, DirectoryWithNoLastSlash(GetScopeCurrentBuildDirAsSourceDir(
                     scope_, BuildDirType::GEN)));
  }
  return target_gen_dir_.get();
}

const Value* ScopePerFileProvider::GetTargetOutDir() {
  if (!target_out_dir_) {
    target_out_dir_ = std::make_unique<Value>(
        nullptr, DirectoryWithNoLastSlash(GetScopeCurrentBuildDirAsSourceDir(
                     scope_, BuildDirType::OBJ)));
  }
  return target_out_dir_.get();
}

const Value* ScopePerFileProvider::GetUseChromiumConfig() {
  if (!use_chromium_config_) {
    use_chromium_config_.reset(new Value(
        nullptr,
        scope_->settings()->build_settings()->use_chromium_config()));
  }
  return use_chromium_config_.get();
}

const Value* ScopePerFileProvider::GetChromiumConfigDir() {
  if (!chromium_config_dir_) {
    chromium_config_dir_.reset(new Value(
        nullptr,
        DirectoryWithNoLastSlash(
            SourceDir(scope_->settings()->build_settings()->
                chromium_config_path_utf8()))));
  }
  return chromium_config_dir_.get();
}
