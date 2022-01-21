// Copyright 2017 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "exegesis/util/file_util.h"

#include <string>

#include "absl/flags/flag.h"
#include "absl/strings/str_cat.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace exegesis {
namespace {

TEST(FileUtilTest, WriteToFileAndReadItAgain) {
  constexpr char kContents[] = "Hello world!";
  const std::string test_file = absl::StrCat(getenv("TEST_TMPDIR"), "testfile");
  WriteTextToFileOrStdOutOrDie(test_file, kContents);
  const std::string contents_from_file =
      ReadTextFromFileOrStdInOrDie(test_file);
  EXPECT_EQ(contents_from_file, kContents);
}

}  // namespace
}  // namespace exegesis
