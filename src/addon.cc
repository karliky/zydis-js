#include <nan.h>
#include <Zydis/Zydis.h>
#include <inttypes.h>

NAN_METHOD(getVersion) {
  ZyanU64 version = ZydisGetVersion();
  info.GetReturnValue().Set(Nan::New((double)version));
}

NAN_MODULE_INIT(Init) {
  NAN_EXPORT(target, getVersion);
}

NODE_MODULE(ZydisJS, Init)