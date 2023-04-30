#include <napi.h>

using namespace Napi;

thread_local int i;

Object Init(Env env, Object exports) { return exports; }
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
