#include "JSHelpers.h"

JSValueRef NativeFunctionCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
  size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
  JSCallback* callback = static_cast<JSCallback*>(JSObjectGetPrivate(function));
  if (!callback)
    return JSValueMakeNull(ctx);

  return (*callback)(ctx, argumentCount, arguments);
}

JSClassRef NativeFunctionClass() {
  static JSClassRef instance = nullptr;
  if (!instance) {
    JSClassDefinition def;
    memset(&def, 0, sizeof(def));

    def.className = "NativeFunction";
    def.attributes = kJSClassAttributeNone;
    def.callAsFunction = NativeFunctionCallback;

    instance = JSClassCreate(&def);
  }
  return instance;
}

JSCallbackHelper::JSCallbackHelper(JSContextRef ctx) : ctx_(ctx) {}

void JSCallbackHelper::Bind(const char* str, const JSCallback& callback) {
  std::unique_ptr<JSCallback> callbackCopy = std::make_unique<JSCallback>(callback);
  JSObjectRef globalObject = JSContextGetGlobalObject(ctx_);
  JSStringRef funcName = JSStr(str);
  JSObjectRef nativeFunction = JSObjectMake(ctx_, NativeFunctionClass(), nullptr);
  JSObjectSetPrivate(nativeFunction, callbackCopy.get());
  JSObjectSetProperty(ctx_, globalObject, funcName, nativeFunction, 0, 0);

  // Make sure callbackCopy stays alive during lifetime of JSCallbackHelper
  callbacks_.push_back(std::move(callbackCopy));
}
