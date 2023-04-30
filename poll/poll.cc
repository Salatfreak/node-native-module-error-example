#include <napi.h>
#include <string>

#include <poll.h>
#include <errno.h>
#include <string.h>

using namespace Napi;
using namespace std;

class Worker : public AsyncWorker {
  public:
    Worker(Function& callback): AsyncWorker(callback) {}
    void Execute() {
      if (poll(nullptr, 0, -1) < 0)
        SetError(string("Poll failed: ") + strerror(errno));
    }
};

Value Poll(const CallbackInfo& info) {
  Function callback = info[0].As<Function>();
  (new Worker(callback))->Queue();
  return info.Env().Undefined();
}

Object Init(Env env, Object exports) {
  exports["poll"] = Function::New(env, Poll);
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
