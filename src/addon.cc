#include <nan.h>
#include <Zydis/Zydis.h>

NAN_METHOD(ZydisGetVersion) {
  ZyanU64 version = ZydisGetVersion();
  info.GetReturnValue().Set(Nan::New((double)version));
}

NAN_METHOD(ZydisFeature_) {
  v8::Local<v8::Object> obj = Nan::New<v8::Object>();
   obj->Set(Nan::New("ZYDIS_FEATURE_DECODER").ToLocalChecked(), Nan::New<v8::Integer>(ZYDIS_FEATURE_DECODER));
   obj->Set(Nan::New("ZYDIS_FEATURE_FORMATTER").ToLocalChecked(), Nan::New<v8::Integer>(ZYDIS_FEATURE_FORMATTER));
   obj->Set(Nan::New("ZYDIS_FEATURE_AVX512").ToLocalChecked(), Nan::New<v8::Integer>(ZYDIS_FEATURE_AVX512));
   obj->Set(Nan::New("ZYDIS_FEATURE_KNC").ToLocalChecked(), Nan::New<v8::Integer>(ZYDIS_FEATURE_KNC));
   obj->Set(Nan::New("ZYDIS_FEATURE_MAX_VALUE").ToLocalChecked(), Nan::New<v8::Integer>(ZYDIS_FEATURE_MAX_VALUE));
   obj->Set(Nan::New("ZYDIS_FEATURE_REQUIRED_BITS").ToLocalChecked(), Nan::New<v8::Integer>(ZYDIS_FEATURE_REQUIRED_BITS));
  info.GetReturnValue().Set(obj);
}

NAN_METHOD(ZydisIsFeatureEnabled) {
  ZyanStatus isFeatureEnabled = ZydisIsFeatureEnabled((ZydisFeature) ZYDIS_FEATURE_DECODER);
  info.GetReturnValue().Set(Nan::New((bool)isFeatureEnabled));
}

NAN_MODULE_INIT(Init) {
  NAN_EXPORT(target, ZydisGetVersion);
  NAN_EXPORT(target, ZydisIsFeatureEnabled);
  NAN_EXPORT(target, ZydisFeature_);
}

NODE_MODULE(ZydisJS, Init)