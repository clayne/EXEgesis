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

#include "exegesis/x86/registers.h"

#include "exegesis/base/registers.h"
#include "exegesis/util/proto_util.h"
#include "strings/str_cat.h"

namespace exegesis {
namespace x86 {
namespace {

RegisterSetProto GetGeneralPurposeRegisters() {
  RegisterSetProto register_set;
  // RAX, RBX, RCX and RDX, and their aliased variants.
  register_set.MergeFrom(MakeRegistersFromBaseNames({{"R", "X", 0, 63, 0, ""},
                                                     {"E", "X", 0, 31, 0, ""},
                                                     {"", "X", 0, 15, 0, ""},
                                                     {"", "L", 0, 7, 0, ""},
                                                     {"", "H", 8, 15, 4, ""}},
                                                    {"A", "C", "D", "B"}, 0));
  // RBP, RSP, RSI, RDI and their aliased variants.
  register_set.MergeFrom(MakeRegistersFromBaseNames({{"R", "", 0, 63, 0, ""},
                                                     {"E", "", 0, 31, 0, ""},
                                                     {"", "", 0, 15, 0, ""},
                                                     {"", "L", 0, 7, 0, ""}},
                                                    {"BP", "SP", "SI", "DI"},
                                                    4));
  // The 64-bit only registers R8-R15 and their aliased variants.
  register_set.MergeFrom(
      MakeRegistersFromBaseNameAndIndices({{"", "", 0, 63, 0, ""},
                                           {"", "D", 0, 31, 0, ""},
                                           {"", "W", 0, 15, 0, ""},
                                           {"", "B", 0, 7, 0, ""}},
                                          "R", 8, 16, 8));

  return register_set;
}

RegisterSetProto GetControlRegisters() {
  // TODO(ondrasej): This is a rough approximation of the actual state: not all
  // CR* registers are defined in the manual, and those that are all
  // have documented subfields.
  // For more information about the control registers, see the Intel 64 and
  // IA-32 Architectures Software Developer's Manual (March 2017), Volume 3A,
  // Section 2.5.
  return MakeRegistersFromBaseNameAndIndices({{"", "", 0, 63, 0, ""}}, "CR", 0,
                                             9, 0);
}

RegisterSetProto GetDebugRegisters() {
  // TODO(ondrasej): This is a rough approximation of the actual state: Some of
  // the debug registers have documented subfields.
  // For more information about the control registers, see the Intel 64 and
  // IA-32 Architectures Software Developer's Manual (March 2017), Volume 3A,
  // Section 17.2.
  return MakeRegistersFromBaseNameAndIndices({{"", "", 0, 63, 0, ""}}, "DR", 0,
                                             9, 0);
}

RegisterSetProto GetFlagsRegisters() {
  constexpr char kFlagsRegisters[] = R"(
      register_groups {
        name: "RFLAGS group"
        description: "The flags registers"
        registers {
          name: "EFLAGS"
          implicit_encoding_only: true
          position_in_group { lsb: 0 msb: 31 }
          subfields {
            bit_range { lsb: 0 msb: 0 }
            name: "CF"
            description: "The carry flag."
          }
          subfields {
            bit_range { lsb: 1 msb: 1 }
            name: "reserved"
          }
          subfields {
            bit_range { lsb: 2 msb: 2 }
            name: "PF"
            description: "The parity flag."
          }
          subfields {
            bit_range { lsb: 3 msb: 3 }
            name: "reserved"
          }
          subfields {
            bit_range { lsb: 4 msb: 4 }
            name: "AF"
            description: "The auxiliary carry flag."
          }
          subfields {
            bit_range { lsb: 5 msb: 5 }
            name: "reserved"
          }
          subfields {
            bit_range { lsb: 6 msb: 6 }
            name: "ZF"
            description: "The zero flag."
          }
          subfields {
            bit_range { lsb: 7 msb: 7 }
            name: "SF"
            description: "The sign flag."
          }
          subfields {
            bit_range { lsb: 8 msb: 8 }
            name: "TF"
            description: "The trap flag."
          }
          subfields {
            bit_range { lsb: 9 msb: 9 }
            name: "IF"
            description: "The interrupt enable flag."
          }
          subfields {
            bit_range { lsb: 10 msb: 10 }
            name: "DF"
            description: "The direction flag."
          }
          subfields {
            bit_range { lsb: 11 msb: 11 }
            name: "OF"
            description: "The overflow flag."
          }
          subfields {
            bit_range { lsb: 12 msb: 13 }
            name: "IOPL"
            description: "The IO privilege level."
          }
          subfields {
            bit_range { lsb: 14 msb: 14 }
            name: "NT"
            description: "The nested task."
          }
          subfields {
            bit_range { lsb: 15 msb: 15 }
            name: "reserved"
          }
          subfields {
            bit_range { lsb: 16 msb: 16 }
            name: "RF"
            description: "The resume flag."
          }
          subfields {
            bit_range { lsb: 17 msb: 17 }
            name: "VM"
            description: "The virtual-8086 mode."
          }
          subfields {
            bit_range { lsb: 18 msb: 18 }
            name: "AC"
            description: "The alignment check/access control."
          }
          subfields {
            bit_range { lsb: 19 msb: 19 }
            name: "VIF"
            description: "The virtual interrupt flag."
          }
          subfields {
            bit_range { lsb: 20 msb: 20 }
            name: "VIP"
            description: "Virtual interrupt pending."
          }
          subfields {
            bit_range { lsb: 21 msb: 21 }
            name: "ID"
            description: "The ID flag."
          }
          subfields {
            bit_range { lsb: 22 msb: 31 }
            name: "reserved"
          }
        }
        registers {
          name: "RFLAGS"
          implicit_encoding_only: true
          position_in_group { lsb: 0 msb: 63 }
          subfields {
            bit_range { lsb: 0 msb: 0 }
            name: "CF"
            description: "The carry flag."
          }
          subfields {
            bit_range { lsb: 1 msb: 1 }
            name: "reserved"
          }
          subfields {
            bit_range { lsb: 2 msb: 2 }
            name: "PF"
            description: "The parity flag."
          }
          subfields {
            bit_range { lsb: 3 msb: 3 }
            name: "reserved"
          }
          subfields {
            bit_range { lsb: 4 msb: 4 }
            name: "AF"
            description: "The auxiliary carry flag."
          }
          subfields {
            bit_range { lsb: 5 msb: 5 }
            name: "reserved"
          }
          subfields {
            bit_range { lsb: 6 msb: 6 }
            name: "ZF"
            description: "The zero flag."
          }
          subfields {
            bit_range { lsb: 7 msb: 7 }
            name: "SF"
            description: "The sign flag."
          }
          subfields {
            bit_range { lsb: 8 msb: 8 }
            name: "TF"
            description: "The trap flag."
          }
          subfields {
            bit_range { lsb: 9 msb: 9 }
            name: "IF"
            description: "The interrupt enable flag."
          }
          subfields {
            bit_range { lsb: 10 msb: 10 }
            name: "DF"
            description: "The direction flag."
          }
          subfields {
            bit_range { lsb: 11 msb: 11 }
            name: "OF"
            description: "The overflow flag."
          }
          subfields {
            bit_range { lsb: 12 msb: 13 }
            name: "IOPL"
            description: "The IO privilege level."
          }
          subfields {
            bit_range { lsb: 14 msb: 14 }
            name: "NT"
            description: "The nested task."
          }
          subfields {
            bit_range { lsb: 15 msb: 15 }
            name: "reserved"
          }
          subfields {
            bit_range { lsb: 16 msb: 16 }
            name: "RF"
            description: "The resume flag."
          }
          subfields {
            bit_range { lsb: 17 msb: 17 }
            name: "VM"
            description: "The virtual-8086 mode."
          }
          subfields {
            bit_range { lsb: 18 msb: 18 }
            name: "AC"
            description: "The alignment check/access control."
          }
          subfields {
            bit_range { lsb: 19 msb: 19 }
            name: "VIF"
            description: "The virtual interrupt flag."
          }
          subfields {
            bit_range { lsb: 20 msb: 20 }
            name: "VIP"
            description: "Virtual interrupt pending."
          }
          subfields {
            bit_range { lsb: 21 msb: 21 }
            name: "ID"
            description: "The ID flag."
          }
          subfields {
            bit_range { lsb: 22 msb: 63 }
            name: "reserved"
          }
        }
      })";
  return ParseProtoFromStringOrDie<RegisterSetProto>(kFlagsRegisters);
}

RegisterSetProto GetFpuAndMmxRegisters() {
  constexpr char kX87FpuStatusAndControlRegisters[] = R"(
      register_groups {
        name: "FPU status word"
        description: "The x87 FPU status word."
        registers {
          name: "FPSW"  # Note that the name FPSW is not used in the Intel SDM,
                        # but it is used in LLVM TD files.
          implicit_encoding_only: true
          position_in_group { lsb: 0 msb: 15 }
          subfields {
            bit_range { lsb: 0 msb: 0 }
            name: "IE"
            description: "Invalid Operation"
          }
          subfields {
            bit_range { lsb: 1 msb: 1 }
            name: "DE"
            description: "Denormalized Operand"
          }
          subfields {
            bit_range { lsb: 2 msb: 2 }
            name: "ZE"
            description: "Zero Divide"
          }
          subfields {
            bit_range { lsb: 3 msb: 3 }
            name: "OE"
            description: "Overflow"
          }
          subfields {
            bit_range { lsb: 4 msb: 4 }
            name: "UE"
            description: "Underflow"
          }
          subfields {
            bit_range { lsb: 5 msb: 5 }
            name: "PE"
            description: "Precision"
          }
          subfields {
            bit_range { lsb: 6 msb: 6 }
            name: "SF"
            description: "Stack Fault"
          }
          subfields {
            bit_range { lsb: 7 msb: 7 }
            name: "ES"
            description: "Exception Summary Status"
          }
          subfields {
            bit_range { lsb: 8 msb: 10 }
            name: "C0-C2"
            description: "Condition Code 0-2"
          }
          subfields {
            bit_range { lsb: 11 msb: 13 }
            name: "TOP"
            description: "Top of Stack Pointer"
          }
          subfields {
            bit_range { lsb: 14 msb: 14 }
            name: "C3"
            description: "Condition Code 3"
          }
          subfields {
            bit_range { lsb: 15 msb: 15 }
            name: "B"
            description: "FPU Busy"
          }
        }
      }
      register_groups {
        name: "FPU control word"
        description: "The x87 FPU control word."
        registers {
          name: "FPCW"  # Note that the name FPCW is used neither in the Intel
                        # SDM nor in LLVM; we use this name because it follows
                        # The same convention as FPSW.
          implicit_encoding_only: true
          position_in_group { lsb: 0 msb: 15 }
          subfields {
            bit_range { lsb: 0 msb: 0 }
            name: "IM"
            description: "Invalid Operation Mask"
          }
          subfields {
            bit_range { lsb: 1 msb: 1 }
            name: "DM"
            description: "Denormalized Operand Mask"
          }
          subfields {
            bit_range { lsb: 2 msb: 2 }
            name: "ZM"
            description: "Zero Divide Mask"
          }
          subfields {
            bit_range { lsb: 3 msb: 3 }
            name: "OM"
            description: "Overflow Mask"
          }
          subfields {
            bit_range { lsb: 4 msb: 4 }
            name: "UM"
            description: "Underflow Mask"
          }
          subfields {
            bit_range { lsb: 5 msb: 5 }
            name: "PM"
            description: "Precision Mask"
          }
          subfields {
            bit_range { lsb: 6 msb: 7 }
            name: "reserved"
          }
          subfields {
            bit_range { lsb: 8 msb: 9 }
            name: "PC"
            description: "Precision Control"
          }
          subfields {
            bit_range { lsb: 10 msb: 11 }
            name: "RC"
            description: "Rounding Control"
          }
          subfields {
            bit_range { lsb: 12 msb: 12 }
            name: "X"
            description: "Infinity Control"
          }
          subfields {
            bit_range { lsb: 13 msb: 15 }
            name: "reserved"
          }
        }
      })";
  RegisterSetProto register_set = MakeRegistersFromBaseNameAndIndices(
      {{"ST", "", 0, 79, 0, "FPU"}, {"MM", "", 0, 63, 0, "MMX"}}, "", 0, 8, 0);
  register_set.MergeFrom(ParseProtoFromStringOrDie<RegisterSetProto>(
      kX87FpuStatusAndControlRegisters));
  return register_set;
}

RegisterSetProto GetOpmaskRegisters() {
  return MakeRegistersFromBaseNameAndIndices({{"", "", 0, 63, 0, "AVX512"}},
                                             "k", 0, 8, 0);
}

// Returns a RegisterSetProto that contains the definitions of the segment
// registers.
RegisterSetProto GetSegmentRegisters() {
  return MakeRegistersFromBaseNames({{"", "S", 0, 15, 0, ""}},
                                    {"E", "C", "S", "D", "F", "G"}, 0);
}

// Returns a RegisterSetProto that contains definitions of the XMM* registers.
RegisterSetProto GetXmmRegisters() {
  static constexpr char kXmmControlRegister[] = R"(
      register_groups {
        name: "MXCSR group"
        description: "The SIMD floating point operation control register."
        registers {
          name: "MXCSR"
          description: "The SIMD floating point operation control register."
          implicit_encoding_only: true
          position_in_group { lsb: 0 msb: 31 }
          subfields {
            bit_range { lsb: 0 msb: 0 }
            name: "IE"
            description: "Invalid Operation Flag"
          }
          subfields {
            bit_range { lsb: 1 msb: 1 }
            name: "DE"
            description: "Denormal Flag"
          }
          subfields {
            bit_range { lsb: 2 msb: 2 }
            name: "ZE"
            description: "Divide-by-zero Flag"
          }
          subfields {
            bit_range { lsb: 3 msb: 3 }
            name: "OE"
            description: "Overflow Flag"
          }
          subfields {
            bit_range { lsb: 4 msb: 4 }
            name: "UE"
            description: "Underflow Flag"
          }
          subfields {
            bit_range { lsb: 5 msb: 5 }
            name: "PE"
            description: "Precision Flag"
          }
          subfields {
            bit_range { lsb: 6 msb: 6 }
            name: "DAZ"
            description: "Denormals Are Zeros"
          }
          subfields {
            bit_range { lsb: 7 msb: 7 }
            name: "IM"
            description: "Invalid Operation Mask"
          }
          subfields {
            bit_range { lsb: 8 msb: 8 }
            name: "DM"
            description: "Denormal Operation Mask"
          }
          subfields {
            bit_range { lsb: 9 msb: 9 }
            name: "ZM"
            description: "Divide-by-zero Mask"
          }
          subfields {
            bit_range { lsb: 10 msb: 10 }
            name: "OM"
            description: "Overflow Mask"
          }
          subfields {
            bit_range { lsb: 11 msb: 11 }
            name: "UM"
            description: "Underflow Mask"
          }
          subfields {
            bit_range { lsb: 12 msb: 12 }
            name: "PM"
            description: "Precision Mask"
          }
          subfields {
            bit_range { lsb: 13 msb: 14 }
            name: "RC"
            description: "Rounding Control"
          }
          subfields {
            bit_range { lsb: 15 msb: 15 }
            name: "FZ"
            description: "Flush to Zero"
          }
          subfields {
            bit_range { lsb: 16 msb: 31 }
            name: "reserved"
          }
        }
      })";
  RegisterSetProto register_set =
      ParseProtoFromStringOrDie<RegisterSetProto>(kXmmControlRegister);
  register_set.MergeFrom(
      MakeRegistersFromBaseNameAndIndices({{"X", "", 0, 127, 0, "SSE"},
                                           {"Y", "", 0, 255, 0, "AVX"},
                                           {"Z", "", 0, 511, 0, "AVX512"}},
                                          "MM", 0, 16, 0));
  // The registers 16-31 can be encoded only with the EVEX encoding, which makes
  // them available only on AVX-512-enabled CPUs.
  register_set.MergeFrom(
      MakeRegistersFromBaseNameAndIndices({{"X", "", 0, 127, 0, "AVX512"},
                                           {"Y", "", 0, 255, 0, "AVX512"},
                                           {"Z", "", 0, 511, 0, "AVX512"}},
                                          "MM", 16, 32, 16));
  return register_set;
}

RegisterSetProto GetMemoryControlRegisters() {
  static constexpr char kRegisterSet[] = R"(
      register_groups {
        name: "GDTR group"
        description: "The Global Descriptor Table Register group"
        registers {
          name: "GDTR"
          position_in_group { lsb: 0 msb: 63 }
        }
      }
      register_groups {
        name: "LDTR group"
        description: "The Local Descriptor Table Register group"
        registers {
          name: "LDTR"
          position_in_group { lsb: 0 msb: 63 }
        }
      }
      register_groups {
        name: "IDTR group"
        description: "The Interrupt Descriptor Table Register group"
        registers {
          name: "IDTR"
          position_in_group { lsb: 0 msb: 63 }
        }
      }
      register_groups {
        name: "TR group"
        description: "The Task Register group"
        registers {
          name: "TR"
          position_in_group { lsb: 0 msb: 63 }
        }
      })";
  return ParseProtoFromStringOrDie<RegisterSetProto>(kRegisterSet);
}

}  // namespace

const RegisterSetProto& GetRegisterSet() {
  // TODO(ondrasej): Add the missing register definitions:
  // - segment registers,
  // - x87 status registers,
  // - MXCSR
  // - system table pointer registers,
  // - machine specific registers.
  static const RegisterSetProto* const register_set = []() {
    auto* register_set = new RegisterSetProto();
    register_set->MergeFrom(GetGeneralPurposeRegisters());
    register_set->MergeFrom(GetControlRegisters());
    register_set->MergeFrom(GetDebugRegisters());
    register_set->MergeFrom(GetFlagsRegisters());
    register_set->MergeFrom(GetFpuAndMmxRegisters());
    register_set->MergeFrom(GetOpmaskRegisters());
    register_set->MergeFrom(GetSegmentRegisters());
    register_set->MergeFrom(GetXmmRegisters());
    register_set->MergeFrom(GetMemoryControlRegisters());
    return register_set;
  }();
  return *register_set;
}

}  // namespace x86
}  // namespace exegesis
