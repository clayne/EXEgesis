// Copyright 2016 Google Inc.
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

#include "exegesis/x86/cleanup_instruction_set_utils.h"

#include "exegesis/proto/instructions.pb.h"
#include "exegesis/proto/x86/encoding_specification.pb.h"
#include "exegesis/testing/test_util.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "src/google/protobuf/text_format.h"

namespace exegesis {
namespace x86 {
namespace {

using ::exegesis::testing::EqualsProto;

TEST(AddOperandSizeOverrideToInstructionProtoTest, AddsPrefix) {
  constexpr char kInstructionProto[] = R"pb(
    vendor_syntax {
      mnemonic: 'ADC'
      operands {
        name: 'r/m16'
        addressing_mode: ANY_ADDRESSING_WITH_FLEXIBLE_REGISTERS
        encoding: MODRM_RM_ENCODING
        value_size_bits: 16
      }
      operands {
        name: 'imm16'
        addressing_mode: NO_ADDRESSING
        encoding: IMMEDIATE_VALUE_ENCODING
        value_size_bits: 16
      }
    }
    raw_encoding_specification: '81 /2 iw'
    x86_encoding_specification {
      legacy_prefixes {}
      opcode: 0x81
      modrm_usage: OPCODE_EXTENSION_IN_MODRM
      modrm_opcode_extension: 2
      immediate_value_bytes: 2
    })pb";
  constexpr char kExpectedInstructionProto[] = R"pb(
    vendor_syntax {
      mnemonic: 'ADC'
      operands {
        name: 'r/m16'
        addressing_mode: ANY_ADDRESSING_WITH_FLEXIBLE_REGISTERS
        encoding: MODRM_RM_ENCODING
        value_size_bits: 16
      }
      operands {
        name: 'imm16'
        addressing_mode: NO_ADDRESSING
        encoding: IMMEDIATE_VALUE_ENCODING
        value_size_bits: 16
      }
    }
    raw_encoding_specification: '66 81 /2 iw'
    x86_encoding_specification {
      legacy_prefixes { operand_size_override_prefix: PREFIX_IS_REQUIRED }
      opcode: 0x81
      modrm_usage: OPCODE_EXTENSION_IN_MODRM
      modrm_opcode_extension: 2
      immediate_value_bytes: 2
    })pb";
  InstructionProto instruction;
  ASSERT_TRUE(::google::protobuf::TextFormat::ParseFromString(kInstructionProto,
                                                              &instruction));
  AddOperandSizeOverrideToInstructionProto(&instruction);
  EXPECT_THAT(instruction, EqualsProto(kExpectedInstructionProto));
}

TEST(AddOperandSizeOverrideToInstructionProtoTest, DoesNotDuplicatePrefix) {
  constexpr char kInstructionProto[] = R"pb(
    vendor_syntax {
      mnemonic: 'ADC'
      operands {
        name: 'r/m16'
        addressing_mode: ANY_ADDRESSING_WITH_FLEXIBLE_REGISTERS
        encoding: MODRM_RM_ENCODING
        value_size_bits: 16
      }
      operands {
        name: 'imm16'
        addressing_mode: NO_ADDRESSING
        encoding: IMMEDIATE_VALUE_ENCODING
        value_size_bits: 16
      }
    }
    raw_encoding_specification: '66 81 /2 iw'
    x86_encoding_specification {
      legacy_prefixes { operand_size_override_prefix: PREFIX_IS_REQUIRED }
      opcode: 0x81
      modrm_usage: OPCODE_EXTENSION_IN_MODRM
      modrm_opcode_extension: 2
      immediate_value_bytes: 2
    })pb";
  InstructionProto instruction;
  ASSERT_TRUE(::google::protobuf::TextFormat::ParseFromString(kInstructionProto,
                                                              &instruction));
  AddOperandSizeOverrideToInstructionProto(&instruction);
  EXPECT_THAT(instruction, EqualsProto(kInstructionProto));
}

TEST(AddOperandSizeOverrideToInstructionProtoTest,
     DoesNotUpdateSpecificationIfNotParsed) {
  constexpr char kInstructionProto[] = R"pb(
    vendor_syntax {
      mnemonic: 'ADC'
      operands {
        name: 'r/m16'
        addressing_mode: ANY_ADDRESSING_WITH_FLEXIBLE_REGISTERS
        encoding: MODRM_RM_ENCODING
        value_size_bits: 16
      }
      operands {
        name: 'imm16'
        addressing_mode: NO_ADDRESSING
        encoding: IMMEDIATE_VALUE_ENCODING
        value_size_bits: 16
      }
    }
    raw_encoding_specification: '81 /2 iw')pb";
  constexpr char kExpectedInstructionProto[] = R"pb(
    vendor_syntax {
      mnemonic: 'ADC'
      operands {
        name: 'r/m16'
        addressing_mode: ANY_ADDRESSING_WITH_FLEXIBLE_REGISTERS
        encoding: MODRM_RM_ENCODING
        value_size_bits: 16
      }
      operands {
        name: 'imm16'
        addressing_mode: NO_ADDRESSING
        encoding: IMMEDIATE_VALUE_ENCODING
        value_size_bits: 16
      }
    }
    raw_encoding_specification: '66 81 /2 iw'
  )pb";
  InstructionProto instruction;
  ASSERT_TRUE(::google::protobuf::TextFormat::ParseFromString(kInstructionProto,
                                                              &instruction));
  AddOperandSizeOverrideToInstructionProto(&instruction);
  EXPECT_THAT(instruction, EqualsProto(kExpectedInstructionProto));
}

TEST(AddPrefixUsageToLegacyInstructions, Test) {
  constexpr char kInstructionSetProto[] = R"pb(
    instructions {
      vendor_syntax { mnemonic: "CPUID" }
      available_in_64_bit: true
      legacy_instruction: true
      raw_encoding_specification: "0F A2"
      x86_encoding_specification {
        opcode: 0x0FA2
        legacy_prefixes {}
      }
    }
    instructions {
      vendor_syntax {
        mnemonic: "ADC"
        operands { name: "AX" usage: USAGE_WRITE }
        operands { name: "imm16" usage: USAGE_READ }
      }
      available_in_64_bit: true
      legacy_instruction: true
      raw_encoding_specification: "66 15 iw"
      x86_encoding_specification {
        opcode: 0x15
        legacy_prefixes { operand_size_override_prefix: PREFIX_IS_REQUIRED }
        immediate_value_bytes: 2
      }
    }
    instructions {
      vendor_syntax {
        mnemonic: "ADC"
        operands { name: "EAX" usage: USAGE_WRITE }
        operands { name: "imm32" usage: USAGE_READ }
      }
      available_in_64_bit: true
      legacy_instruction: true
      raw_encoding_specification: "15 id"
      x86_encoding_specification {
        opcode: 0x15
        legacy_prefixes {}
        immediate_value_bytes: 4
      }
    }
    instructions {
      vendor_syntax {
        mnemonic: "ADC"
        operands { name: "RAX" usage: USAGE_WRITE }
        operands { name: "imm32" usage: USAGE_READ }
      }
      available_in_64_bit: true
      raw_encoding_specification: "REX.W + 15 id"
      x86_encoding_specification {
        opcode: 0x15
        legacy_prefixes { rex_w_prefix: PREFIX_IS_REQUIRED }
        immediate_value_bytes: 4
      }
    }
    instructions {
      vendor_syntax {
        mnemonic: "ANDN"
        operands { name: "r32a" usage: USAGE_WRITE }
        operands { name: "r32b" usage: USAGE_READ }
        operands { name: "m32" usage: USAGE_READ }
      }
      feature_name: "BMI1"
      available_in_64_bit: true
      legacy_instruction: true
      raw_encoding_specification: "VEX.NDS.LZ.0F38.W0 F2 /r"
      x86_encoding_specification {
        opcode: 0x0F38F2
        modrm_usage: FULL_MODRM
        vex_prefix {
          prefix_type: VEX_PREFIX
          vex_operand_usage: VEX_OPERAND_IS_FIRST_SOURCE_REGISTER
          vector_size: VEX_VECTOR_SIZE_BIT_IS_ZERO
          map_select: MAP_SELECT_0F38
          vex_w_usage: VEX_W_IS_ZERO
        }
      }
    }
    instructions {
      vendor_syntax {
        mnemonic: "CALL"
        operands { name: "m64" usage: USAGE_READ }
      }
      available_in_64_bit: true
      raw_encoding_specification: "FF /2"
      x86_encoding_specification {
        opcode: 255
        modrm_usage: OPCODE_EXTENSION_IN_MODRM
        modrm_opcode_extension: 2
        legacy_prefixes {}
      }
    }
    instructions {
      vendor_syntax {
        mnemonic: "INC"
        operands { name: "m16" usage: USAGE_READ_WRITE }
      }
      available_in_64_bit: true
      legacy_instruction: true
      raw_encoding_specification: "66 FF /0"
      x86_encoding_specification {
        opcode: 255
        modrm_usage: OPCODE_EXTENSION_IN_MODRM
        legacy_prefixes { operand_size_override_prefix: PREFIX_IS_REQUIRED }
      }
    }
    instructions {
      vendor_syntax {
        mnemonic: "INC"
        operands { name: "m32" usage: USAGE_READ_WRITE }
      }
      available_in_64_bit: true
      legacy_instruction: true
      raw_encoding_specification: "FF /0"
      x86_encoding_specification {
        opcode: 255
        modrm_usage: OPCODE_EXTENSION_IN_MODRM
        legacy_prefixes {}
      }
    }
    instructions {
      vendor_syntax {
        mnemonic: "INC"
        operands { name: "m64" usage: USAGE_READ_WRITE }
      }
      available_in_64_bit: true
      raw_encoding_specification: "REX.W + FF /0"
      x86_encoding_specification {
        opcode: 255
        modrm_usage: OPCODE_EXTENSION_IN_MODRM
        legacy_prefixes { rex_w_prefix: PREFIX_IS_REQUIRED }
      }
    })pb";
  constexpr char kExpectedInstructionSetProto[] = R"pb(
    instructions {
      vendor_syntax { mnemonic: "CPUID" }
      available_in_64_bit: true
      legacy_instruction: true
      raw_encoding_specification: "0F A2"
      x86_encoding_specification {
        opcode: 0x0FA2
        legacy_prefixes { rex_w_prefix: PREFIX_IS_IGNORED }
      }
    }
    instructions {
      vendor_syntax {
        mnemonic: "ADC"
        operands { name: "AX" usage: USAGE_WRITE }
        operands { name: "imm16" usage: USAGE_READ }
      }
      available_in_64_bit: true
      legacy_instruction: true
      raw_encoding_specification: "66 15 iw"
      x86_encoding_specification {
        opcode: 0x15
        legacy_prefixes {
          rex_w_prefix: PREFIX_IS_NOT_PERMITTED
          operand_size_override_prefix: PREFIX_IS_REQUIRED

        }
        immediate_value_bytes: 2
      }
    }
    instructions {
      vendor_syntax {
        mnemonic: "ADC"
        operands { name: "EAX" usage: USAGE_WRITE }
        operands { name: "imm32" usage: USAGE_READ }
      }
      available_in_64_bit: true
      legacy_instruction: true
      raw_encoding_specification: "15 id"
      x86_encoding_specification {
        opcode: 0x15
        legacy_prefixes { rex_w_prefix: PREFIX_IS_NOT_PERMITTED }
        immediate_value_bytes: 4
      }
    }
    instructions {
      vendor_syntax {
        mnemonic: "ADC"
        operands { name: "RAX" usage: USAGE_WRITE }
        operands { name: "imm32" usage: USAGE_READ }
      }
      available_in_64_bit: true
      raw_encoding_specification: "REX.W + 15 id"
      x86_encoding_specification {
        opcode: 0x15
        legacy_prefixes { rex_w_prefix: PREFIX_IS_REQUIRED }
        immediate_value_bytes: 4
      }
    }
    instructions {
      vendor_syntax {
        mnemonic: "ANDN"
        operands { name: "r32a" usage: USAGE_WRITE }
        operands { name: "r32b" usage: USAGE_READ }
        operands { name: "m32" usage: USAGE_READ }
      }
      feature_name: "BMI1"
      available_in_64_bit: true
      legacy_instruction: true
      raw_encoding_specification: "VEX.NDS.LZ.0F38.W0 F2 /r"
      x86_encoding_specification {
        opcode: 0x0F38F2
        modrm_usage: FULL_MODRM
        vex_prefix {
          prefix_type: VEX_PREFIX
          vex_operand_usage: VEX_OPERAND_IS_FIRST_SOURCE_REGISTER
          vector_size: VEX_VECTOR_SIZE_BIT_IS_ZERO
          map_select: MAP_SELECT_0F38
          vex_w_usage: VEX_W_IS_ZERO
        }
      }
    }
    instructions {
      vendor_syntax {
        mnemonic: "CALL"
        operands { name: "m64" usage: USAGE_READ }
      }
      available_in_64_bit: true
      raw_encoding_specification: "FF /2"
      x86_encoding_specification {
        opcode: 255
        modrm_usage: OPCODE_EXTENSION_IN_MODRM
        modrm_opcode_extension: 2
        legacy_prefixes { rex_w_prefix: PREFIX_IS_IGNORED }
      }
    }
    instructions {
      vendor_syntax {
        mnemonic: "INC"
        operands { name: "m16" usage: USAGE_READ_WRITE }
      }
      available_in_64_bit: true
      legacy_instruction: true
      raw_encoding_specification: "66 FF /0"
      x86_encoding_specification {
        opcode: 255
        modrm_usage: OPCODE_EXTENSION_IN_MODRM
        legacy_prefixes {
          rex_w_prefix: PREFIX_IS_NOT_PERMITTED
          operand_size_override_prefix: PREFIX_IS_REQUIRED
        }
      }
    }
    instructions {
      vendor_syntax {
        mnemonic: "INC"
        operands { name: "m32" usage: USAGE_READ_WRITE }
      }
      available_in_64_bit: true
      legacy_instruction: true
      raw_encoding_specification: "FF /0"
      x86_encoding_specification {
        opcode: 255
        modrm_usage: OPCODE_EXTENSION_IN_MODRM
        legacy_prefixes { rex_w_prefix: PREFIX_IS_NOT_PERMITTED }
      }
    }
    instructions {
      vendor_syntax {
        mnemonic: "INC"
        operands { name: "m64" usage: USAGE_READ_WRITE }
      }
      available_in_64_bit: true
      raw_encoding_specification: "REX.W + FF /0"
      x86_encoding_specification {
        opcode: 255
        modrm_usage: OPCODE_EXTENSION_IN_MODRM
        legacy_prefixes { rex_w_prefix: PREFIX_IS_REQUIRED }
      }
    })pb";
  InstructionSetProto instruction_set;
  ASSERT_TRUE(::google::protobuf::TextFormat::ParseFromString(
      kInstructionSetProto, &instruction_set));
  AddPrefixUsageToLegacyInstructions(
      [](const InstructionProto& instruction) {
        return instruction.x86_encoding_specification()
            .legacy_prefixes()
            .rex_w_prefix();
      },
      [](InstructionProto* instruction, LegacyEncoding::PrefixUsage usage) {
        instruction->mutable_x86_encoding_specification()
            ->mutable_legacy_prefixes()
            ->set_rex_w_prefix(usage);
      },
      &instruction_set);
  EXPECT_THAT(instruction_set, EqualsProto(kExpectedInstructionSetProto));
}

}  // namespace
}  // namespace x86
}  // namespace exegesis
