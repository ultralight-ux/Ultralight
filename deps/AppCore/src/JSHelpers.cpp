#include <AppCore/JSHelpers.h>
#include <cstring>
#include <cassert>
#include <vector>

namespace ultralight {

static JSContextRef g_context = nullptr;

void SetJSContext(JSContextRef ctx) { g_context = ctx; }

JSContextRef GetJSContext() { return g_context; }

JSString::JSString() {
  instance_ = JSStringCreateWithCharacters(nullptr, 0);
}

JSString::JSString(const char* str) {
  instance_ = JSStringCreateWithUTF8CString(str);
}

JSString::JSString(const String& str) {
  instance_ = JSStringCreateWithCharacters((JSChar*)str.utf16().data(), str.utf16().length());
}

JSString::JSString(JSStringRef str) {
  instance_ = str;
}

JSString::JSString(const JSString& other) : instance_(other.instance_) {
  JSStringRetain(instance_);
}

JSString::~JSString() {
  JSStringRelease(instance_);
}

JSString& JSString::operator=(const JSString& other) {
  JSStringRelease(instance_);
  instance_ = other.instance_;
  JSStringRetain(instance_);

  return *this;
}

JSString::operator String() {
  return String16((Char16*)JSStringGetCharactersPtr(instance_),
                              JSStringGetLength(instance_));
}

JSValue::JSValue() : ctx_(GetJSContext()) {
  instance_ = JSValueMakeNull(ctx_);
  JSValueProtect(ctx_, instance_);
}

JSValue::JSValue(JSValueNullTag) : ctx_(GetJSContext()) {
  instance_ = JSValueMakeNull(GetJSContext());
  JSValueProtect(GetJSContext(), instance_);
}

JSValue::JSValue(JSValueUndefinedTag) : ctx_(GetJSContext()) {
  instance_ = JSValueMakeUndefined(ctx_);
  JSValueProtect(ctx_, instance_);
}

JSValue::JSValue(bool val) : ctx_(GetJSContext()) {
  instance_ = JSValueMakeBoolean(ctx_, val);
  JSValueProtect(ctx_, instance_);
}

JSValue::JSValue(uint32_t val) : ctx_(GetJSContext()) {
  instance_ = JSValueMakeNumber(ctx_, static_cast<double>(val));
  JSValueProtect(ctx_, instance_);
}

JSValue::JSValue(int32_t val) : ctx_(GetJSContext()) {
  instance_ = JSValueMakeNumber(ctx_, static_cast<double>(val));
  JSValueProtect(ctx_, instance_);
}

JSValue::JSValue(uint64_t val) : ctx_(GetJSContext()) {
  instance_ = JSValueMakeNumber(ctx_, static_cast<double>(val));
  JSValueProtect(ctx_, instance_);
}

JSValue::JSValue(int64_t val) : ctx_(GetJSContext()) {
  instance_ = JSValueMakeNumber(ctx_, static_cast<double>(val));
  JSValueProtect(ctx_, instance_);
}

JSValue::JSValue(double val) : ctx_(GetJSContext()) {
  instance_ = JSValueMakeNumber(ctx_, val);
  JSValueProtect(ctx_, instance_);
}

JSValue::JSValue(const char* val) : ctx_(GetJSContext()) {
  instance_ = JSValueMakeString(ctx_, JSString(val));
  JSValueProtect(ctx_, instance_);
}

JSValue::JSValue(const String& val) : ctx_(GetJSContext()) {
  instance_ = JSValueMakeString(ctx_, JSString(val));
  JSValueProtect(ctx_, instance_);
}

JSValue::JSValue(JSString val) : ctx_(GetJSContext()) {
  instance_ = JSValueMakeString(ctx_, val);
  JSValueProtect(ctx_, instance_);
}

JSValue::JSValue(JSValueRef val) : ctx_(GetJSContext()) {
  instance_ = val;
  JSValueProtect(ctx_, instance_);
}

JSValue::JSValue(JSObjectRef obj) : ctx_(GetJSContext()) {
  instance_ = obj;
  JSValueProtect(ctx_, instance_);
}

JSValue::JSValue(JSContextRef ctx) : ctx_(ctx), instance_(nullptr) {}

JSValue::JSValue(JSContextRef ctx, JSValueRef val) : ctx_(ctx), instance_(val) {
  JSValueProtect(ctx_, instance_);
}

JSValue::JSValue(const JSValue& other) : ctx_(other.ctx_), instance_(other.instance_) {
  JSValueProtect(ctx_, instance_);
}

JSValue::~JSValue() {
  if (instance_)
    JSValueUnprotect(ctx_, instance_);
}

JSValue& JSValue::operator=(const JSValue& other) {
  JSValueUnprotect(ctx_, instance_);
  ctx_ = other.ctx_;
  instance_ = other.instance_;
  JSValueProtect(ctx_, instance_);

  return *this;
}

bool JSValue::IsNull() const { return JSValueGetType(ctx_, instance()) == kJSTypeNull; }

bool JSValue::IsUndefined() const { return JSValueGetType(ctx_, instance()) == kJSTypeUndefined; }

bool JSValue::IsBoolean() const { return JSValueGetType(ctx_, instance()) == kJSTypeBoolean; }

bool JSValue::IsNumber() const { return JSValueGetType(ctx_, instance()) == kJSTypeNumber; }

bool JSValue::IsString() const { return JSValueGetType(ctx_, instance()) == kJSTypeString; }

bool JSValue::IsObject() const { return JSValueGetType(ctx_, instance()) == kJSTypeObject; }

bool JSValue::IsArray() const { return JSValueIsArray(ctx_, instance()); }

bool JSValue::IsFunction() const { 
  if (!IsObject())
    return false;
  JSObjectRef obj = JSValueToObject(ctx_, instance(), nullptr);
  return JSObjectIsFunction(ctx_, obj);
}

bool JSValue::ToBoolean() const { return JSValueToBoolean(ctx_, instance()); }

double JSValue::ToNumber() const { return JSValueToNumber(ctx_, instance(), nullptr); }

JSString JSValue::ToString() const { 
  JSString result(JSValueToStringCopy(ctx_, instance(), nullptr));

  return result;
}

JSObject JSValue::ToObject() const {
  assert(IsObject()); 
  return JSObject(ctx_, instance());
}

JSArray JSValue::ToArray() const {
  assert(IsArray());
  return JSArray(ctx_, instance());
}

JSFunction JSValue::ToFunction() const {
  assert(IsFunction());
  return JSFunction(ctx_, instance());
}

JSValue::operator JSObject() const { return ToObject(); }

JSValue::operator JSObjectRef() const { return ToObject(); }

JSValue::operator JSArray() const { return ToArray(); }

JSValue::operator JSFunction() const { return ToFunction(); }

JSValueRef JSValue::instance() const { return instance_; }

// JSArgs -------------------------------------------------
#define GetVector() (*reinterpret_cast<std::vector<JSValue>*>(instance_))
#define GetVectorConst() (*reinterpret_cast<const std::vector<JSValue>*>(instance_))

JSArgs::JSArgs() {
  instance_ = new std::vector<JSValue>();
}

JSArgs::JSArgs(const std::initializer_list<JSValue>& values) {
  instance_ = new std::vector<JSValue>(values);
}

JSArgs::JSArgs(const JSArgs& other) {
  instance_ = new std::vector<JSValue>(*reinterpret_cast<std::vector<JSValue>*>(other.instance_));
}

JSArgs::~JSArgs() {
  delete reinterpret_cast<std::vector<JSValue>*>(instance_);
}

JSArgs& JSArgs::operator=(const JSArgs& other) {
  GetVector() = *reinterpret_cast<std::vector<JSValue>*>(other.instance_);
  return *this;
}

JSValue JSArgs::operator[](size_t pos) {
  return GetVector()[pos];
}

const JSValue JSArgs::operator[](size_t pos) const {
  return GetVectorConst()[pos];
}

bool JSArgs::empty() const {
  return GetVectorConst().empty();
}

size_t JSArgs::size() const {
  return GetVectorConst().size();
}

void JSArgs::clear() {
  GetVector().clear();
}

void JSArgs::push_back(const JSValue& val) {
  GetVector().push_back(val);
}

void JSArgs::pop_back() {
  GetVector().pop_back();
}

JSValue* JSArgs::data() {
  return GetVector().data();
}

const JSValue* JSArgs::data() const {
  return GetVectorConst().data();
}

// JSPropertyValue ----------------------------------------

JSPropertyValue::JSPropertyValue(JSContextRef ctx, JSObjectRef proxy_obj, unsigned idx)
  : JSValue(ctx),
    proxyObj_(new JSObject(ctx, proxy_obj)),
    using_numeric_idx_(true), 
    numeric_idx_(idx) {
}

JSPropertyValue::JSPropertyValue(JSContextRef ctx, JSObjectRef proxy_obj, JSString idx) 
  : JSValue(ctx),
    proxyObj_(new JSObject(ctx, proxy_obj)),
    using_numeric_idx_(false),
    numeric_idx_(0),
    string_idx_(idx) {
}

JSPropertyValue::~JSPropertyValue() {
  delete proxyObj_;
}

JSPropertyValue& JSPropertyValue::operator=(const JSValue& value) {
  if (using_numeric_idx_)
    JSObjectSetPropertyAtIndex(ctx_, *proxyObj_, numeric_idx_, value, nullptr);
  else
    JSObjectSetProperty(ctx_, *proxyObj_, string_idx_, value, kJSPropertyAttributeNone, nullptr);

  return *this;
}

JSValueRef NativeFunctionCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
  size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
  JSCallback* callback = static_cast<JSCallback*>(JSObjectGetPrivate(function));
  if (!callback)
    return JSValueMakeNull(ctx);

  JSContextRef old_ctx = GetJSContext();
  SetJSContext(ctx);
  JSArgs args;
  for (size_t i = 0; i < argumentCount; ++i)
    args.push_back(arguments[i]);

  (*callback)(thisObject, args);
  SetJSContext(old_ctx);

  return JSValueMakeNull(ctx);
}

void NativeFunctionFinalize(JSObjectRef function) {
  JSCallback* callback = static_cast<JSCallback*>(JSObjectGetPrivate(function));
  delete callback;
}

JSClassRef NativeFunctionClass() {
  static JSClassRef instance = nullptr;
  if (!instance) {
    JSClassDefinition def;
    memset(&def, 0, sizeof(def));
    def.className = "NativeFunction";
    def.attributes = kJSClassAttributeNone;
    def.callAsFunction = NativeFunctionCallback;
    def.finalize = NativeFunctionFinalize;
    instance = JSClassCreate(&def);
  }
  return instance;
}

JSValueRef NativeFunctionWithRetvalCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
  size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
  JSCallbackWithRetval* callback = static_cast<JSCallbackWithRetval*>(JSObjectGetPrivate(function));
  if (!callback)
    return JSValueMakeNull(ctx);

  JSContextRef old_ctx = GetJSContext();
  SetJSContext(ctx);
  JSArgs args;
  for (size_t i = 0; i < argumentCount; ++i)
    args.push_back(arguments[i]);

  JSValueRef result = (*callback)(thisObject, args);
  SetJSContext(old_ctx);

  return result;
}

void NativeFunctionWithRetvalFinalize(JSObjectRef function) {
  JSCallbackWithRetval* callback = static_cast<JSCallbackWithRetval*>(JSObjectGetPrivate(function));
  delete callback;
}

JSClassRef NativeFunctionWithRetvalClass() {
  static JSClassRef instance = nullptr;
  if (!instance) {
    JSClassDefinition def;
    memset(&def, 0, sizeof(def));
    def.className = "NativeFunctionWithRetval";
    def.attributes = kJSClassAttributeNone;
    def.callAsFunction = NativeFunctionWithRetvalCallback;
    def.finalize = NativeFunctionWithRetvalFinalize;
    instance = JSClassCreate(&def);
  }
  return instance;
}

JSPropertyValue& JSPropertyValue::operator=(const JSCallback& callback) {
  JSObjectRef nativeFunction = JSObjectMake(ctx_, NativeFunctionClass(), new JSCallback(callback));
  if (using_numeric_idx_) {
    JSObjectSetPropertyAtIndex(ctx_, *proxyObj_, numeric_idx_, nativeFunction, nullptr);
    JSObjectSetProperty(ctx_, nativeFunction, JSString("name"), JSValue(numeric_idx_), kJSPropertyAttributeReadOnly, nullptr);
  } else {
    JSObjectSetProperty(ctx_, *proxyObj_, string_idx_, nativeFunction, kJSPropertyAttributeNone, nullptr);
    JSObjectSetProperty(ctx_, nativeFunction, JSString("name"), JSValue(string_idx_), kJSPropertyAttributeReadOnly, nullptr);
  }

  return *this;
}

JSPropertyValue& JSPropertyValue::operator=(const JSCallbackWithRetval& callback) {
  JSObjectRef nativeFunction = JSObjectMake(ctx_, NativeFunctionWithRetvalClass(), new JSCallbackWithRetval(callback));
  if (using_numeric_idx_) {
    JSObjectSetPropertyAtIndex(ctx_, *proxyObj_, numeric_idx_, nativeFunction, nullptr);
    JSObjectSetProperty(ctx_, nativeFunction, JSString("name"), JSValue(numeric_idx_), kJSPropertyAttributeReadOnly, nullptr);
  } else {
    JSObjectSetProperty(ctx_, *proxyObj_, string_idx_, nativeFunction, kJSPropertyAttributeNone, nullptr);
    JSObjectSetProperty(ctx_, nativeFunction, JSString("name"), JSValue(string_idx_), kJSPropertyAttributeReadOnly, nullptr);
  }

  return *this;
}

JSValueRef JSPropertyValue::instance() const {
  if (using_numeric_idx_)
    return JSObjectGetPropertyAtIndex(ctx_, *proxyObj_, numeric_idx_, nullptr);
  else
    return JSObjectGetProperty(ctx_, *proxyObj_, string_idx_, nullptr);
}

// JSArray ------------------------------------------------

JSArray::JSArray() : ctx_(GetJSContext()) {
  instance_ = JSObjectMakeArray(ctx_, 0, nullptr, nullptr);
  JSValueProtect(ctx_, instance_);
}

JSArray::JSArray(const std::initializer_list<JSValue>& values) : ctx_(GetJSContext()) {
  size_t argCount = values.size();
  JSValueRef* args = nullptr;
  if (argCount) {
    args = new JSValueRef[argCount];
    size_t i = 0;
    for (auto arg : values)
      args[i++] = arg;
  }

  instance_ = JSObjectMakeArray(ctx_, argCount, args, nullptr);

  if (args)
    delete[] args;

  JSValueProtect(ctx_, instance_);
}

JSArray::JSArray(JSObjectRef array_obj) : ctx_(GetJSContext()) {
  instance_ = array_obj;
  JSValueProtect(ctx_, instance_);
}

JSArray::JSArray(JSContextRef ctx, JSValueRef val) : ctx_(ctx) {
  instance_ = JSValueToObject(ctx_, val, nullptr);
  JSValueProtect(ctx_, instance_);
}

JSArray::JSArray(const JSArray& other) : ctx_(other.ctx_), instance_(other.instance_) {
  JSValueProtect(ctx_, instance_);
}

JSArray::~JSArray() {
  JSValueUnprotect(ctx_, instance_);
}

JSArray& JSArray::operator=(const JSArray& other) {
  JSValueUnprotect(ctx_, instance_);
  ctx_ = other.ctx_;
  instance_ = other.instance_;
  JSValueProtect(ctx_, instance_);

  return *this;
}

unsigned JSArray::length() {
  JSValueRef val = JSObjectGetProperty(ctx_, instance_, JSString("length"), nullptr);
  if (JSValueIsNumber(ctx_, val))
    return static_cast<unsigned>(JSValueToNumber(ctx_, val, nullptr));

  return 0;
}

void JSArray::push(const JSValue& val) {
  JSValueRef prop = JSObjectGetProperty(ctx_, instance_, JSString("push"), nullptr);
  if (JSValueIsObject(ctx_, prop)) {
    JSObjectRef func = JSValueToObject(ctx_, prop, nullptr);
    if (JSObjectIsFunction(ctx_, func)) {
      JSValueRef arg = val;
      JSObjectCallAsFunction(ctx_, func, instance_, 1, &arg, nullptr);
    }
  }
}

int JSArray::indexOf(const JSValue& val, int start) const {
  JSValueRef prop = JSObjectGetProperty(ctx_, instance_, JSString("indexOf"), nullptr);
  if (JSValueIsObject(ctx_, prop)) {
    JSObjectRef func = JSValueToObject(ctx_, prop, nullptr);
    if (JSObjectIsFunction(ctx_, func)) {
      JSValueRef args[2] = { val, JSValueMakeNumber(ctx_, start) };
      JSValueRef result = JSObjectCallAsFunction(ctx_, func, instance_, 2, args, nullptr);
      if (JSValueIsNumber(ctx_, result)) {
        return static_cast<int>(JSValueToNumber(ctx_, result, nullptr));
      }
    }
  }

  return -1;
}

JSPropertyValue JSArray::operator[](unsigned idx) const {
  return JSPropertyValue(ctx_, instance_, idx);
}

// JSObject ----------------------------------------------------

JSObject::JSObject() : ctx_(GetJSContext()) {
  instance_ = JSObjectMake(ctx_, nullptr, nullptr);
  JSValueProtect(ctx_, instance_);
}

JSObject::JSObject(JSObjectRef obj) : ctx_(GetJSContext()) {
  instance_ = obj;
  JSValueProtect(ctx_, instance_);
}

JSObject::JSObject(JSContextRef ctx, JSValueRef val) : ctx_(ctx) {
  instance_ = JSValueToObject(ctx_, val, nullptr);
  JSValueProtect(ctx_, instance_);
}

JSObject::JSObject(JSContextRef ctx, JSObjectRef obj) : ctx_(ctx) {
  instance_ = obj;
  JSValueProtect(ctx, instance_);
}

JSObject::JSObject(const JSObject& other) : ctx_(other.ctx_), instance_(other.instance_) {
  JSValueProtect(ctx_, instance_);
}

JSObject::~JSObject() {
  if (instance_)
    JSValueUnprotect(ctx_, instance_);
}

JSObject& JSObject::operator=(const JSObject& other) {
  JSValueUnprotect(ctx_, instance_);
  ctx_ = other.ctx_;
  instance_ = other.instance_;
  JSValueProtect(ctx_, instance_);

  return *this;
}

JSPropertyValue JSObject::operator[](JSString propertyName) const {
  return JSPropertyValue(ctx_, instance_, propertyName);
}

bool JSObject::HasProperty(JSString propertyName) const {
  return JSObjectHasProperty(ctx_, instance_, propertyName);
}

bool JSObject::DeleteProperty(JSString propertyName) {
  return JSObjectDeleteProperty(ctx_, instance_, propertyName, nullptr);
}


JSFunction::JSFunction() : ctx_(nullptr), instance_(nullptr) {
}

JSFunction::JSFunction(JSContextRef ctx, JSValueRef val) : ctx_(ctx) {
  instance_ = JSValueToObject(ctx_, val, nullptr);
  JSValueProtect(ctx_, instance_);
}

JSFunction::JSFunction(const JSFunction& other) : ctx_(other.ctx_), instance_(other.instance_) {
  JSValueProtect(ctx_, instance_);
}

JSFunction::~JSFunction() {
  if (instance_)
    JSValueUnprotect(ctx_, instance_);
}

JSFunction& JSFunction::operator=(const JSFunction& other) {
  if (instance_)
    JSValueUnprotect(ctx_, instance_);
  ctx_ = other.ctx_;
  instance_ = other.instance_;
  JSValueProtect(ctx_, instance_);

  return *this;
}

bool JSFunction::IsValid() const { return !!instance_ && JSObjectIsFunction(ctx_, instance_); }

JSValue JSFunction::operator()(const JSArgs& args) {
  return operator()(JSGlobalObject(), args);
}

JSValue JSFunction::operator()(const JSObject& thisObject, const JSArgs& args) {
  assert(IsValid());
  size_t argCount = args.size();
  JSValueRef* _args = nullptr;
  if (argCount) {
    _args = new JSValueRef[argCount];
    for (size_t i = 0; i < argCount; ++i)
      _args[i] = args[i];
  }

  JSValue result(ctx_, JSObjectCallAsFunction(ctx_, instance_, thisObject, argCount, _args, nullptr));

  if (_args)
    delete[] _args;

  return result;
}

JSObject JSGlobalObject() {
  return JSContextGetGlobalObject(GetJSContext());
}

JSValue JSEval(const JSString& str) {
  return JSEvaluateScript(GetJSContext(), str, 0, 0, 0, 0);
}

}  // namespace ultralight
