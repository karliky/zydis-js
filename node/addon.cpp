#include <nan.h>
#include "encode.h"   // NOLINT(build/include)

using v8::FunctionTemplate;
using v8::String;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

NAN_MODULE_INIT(InitAll) {
  Set(target, New<String>("encode").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(Encode)).ToLocalChecked());
}

NODE_MODULE(addon, InitAll)
