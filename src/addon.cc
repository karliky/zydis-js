#include <nan.h>
#include <Zydis/Zydis.h>
#include <iostream>

NAN_METHOD(ZydisGetVersion) {
  ZyanU64 version = ZydisGetVersion();
  info.GetReturnValue().Set(Nan::New((double)version));
}

NAN_METHOD(ZydisFeature_) {
  v8::Local<v8::Object> obj = Nan::New<v8::Object>();
   Nan::Set(obj, Nan::New("ZYDIS_FEATURE_DECODER").ToLocalChecked(), Nan::New<v8::Integer>(ZYDIS_FEATURE_DECODER));
   Nan::Set(obj, Nan::New("ZYDIS_FEATURE_FORMATTER").ToLocalChecked(), Nan::New<v8::Integer>(ZYDIS_FEATURE_FORMATTER));
   Nan::Set(obj, Nan::New("ZYDIS_FEATURE_AVX512").ToLocalChecked(), Nan::New<v8::Integer>(ZYDIS_FEATURE_AVX512));
   Nan::Set(obj, Nan::New("ZYDIS_FEATURE_KNC").ToLocalChecked(), Nan::New<v8::Integer>(ZYDIS_FEATURE_KNC));
   Nan::Set(obj, Nan::New("ZYDIS_FEATURE_MAX_VALUE").ToLocalChecked(), Nan::New<v8::Integer>(ZYDIS_FEATURE_MAX_VALUE));
   Nan::Set(obj, Nan::New("ZYDIS_FEATURE_REQUIRED_BITS").ToLocalChecked(), Nan::New<v8::Integer>(ZYDIS_FEATURE_REQUIRED_BITS));
  info.GetReturnValue().Set(obj);
}

NAN_METHOD(ZydisIsFeatureEnabled) {
  ZyanStatus isFeatureEnabled = ZydisIsFeatureEnabled((ZydisFeature) ZYDIS_FEATURE_DECODER);
  info.GetReturnValue().Set(Nan::New((bool)isFeatureEnabled));
}

NAN_MODULE_INIT(Init) {
  Nan::Set(target , Nan::New("ZydisGetVersion").ToLocalChecked() , Nan::GetFunction(Nan::New<v8::FunctionTemplate>(ZydisGetVersion)).ToLocalChecked());
  Nan::Set(target , Nan::New("ZydisIsFeatureEnabled").ToLocalChecked() , Nan::GetFunction(Nan::New<v8::FunctionTemplate>(ZydisIsFeatureEnabled)).ToLocalChecked());
  Nan::Set(target , Nan::New("ZydisFeature_").ToLocalChecked() , Nan::GetFunction(Nan::New<v8::FunctionTemplate>(ZydisFeature_)).ToLocalChecked());
}

NODE_MODULE(ZydisJS, Init)