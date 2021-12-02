#include <nan.h>
#include "encode.h"  // NOLINT(build/include)
#include <sstream>

#include <Zydis/Zydis.h>
#include <Zycore/LibC.h>

NAN_METHOD(Encode) {
  ZyanU8 encoded_instruction[ZYDIS_MAX_INSTRUCTION_LENGTH];
  ZyanUSize encoded_length = sizeof(encoded_instruction);
  ZydisEncoderRequest req;
  ZYAN_MEMSET(&req, 0, sizeof(req));
  req.mnemonic = ZYDIS_MNEMONIC_MOV;
  req.machine_mode = ZYDIS_MACHINE_MODE_LONG_64;
  req.operand_count = 2;
  req.operands[0].type = ZYDIS_OPERAND_TYPE_REGISTER;
  req.operands[0].reg.value = ZYDIS_REGISTER_RAX;
  req.operands[1].type = ZYDIS_OPERAND_TYPE_IMMEDIATE;
  req.operands[1].imm.u = 0x1337;
  if (ZYAN_FAILED(ZydisEncoderEncodeInstruction(&req, encoded_instruction, &encoded_length)))
  {
      ZYAN_PUTS("Failed to encode instruction");
  }
  char result [encoded_length];
  int length = 0;
  for (ZyanUSize i = 0; i < encoded_length; ++i)
  {
      // ZYAN_PRINTF("%02X ", encoded_instruction[i]);
      auto format = ( i == encoded_length - 1) ? "%02X" : "%02X ";
      length += sprintf(result+length, format, encoded_instruction[i]);
  }
  // ZYAN_PUTS("");

  info.GetReturnValue().Set(Nan::New(result).ToLocalChecked());
}
