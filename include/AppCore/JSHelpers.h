#pragma once
#include <AppCore/Defines.h>
#include <JavaScriptCore/JavaScript.h>
#include <JavaScriptCore/JSStringRef.h>
#include <Ultralight/String.h>
#include <functional>
#include <memory>

/**
* This is a set of common JavaScriptCore Helper functions
* to simplify sample code.
*/

namespace ultralight {

/**
 * Set the current JSContext.
 *
 * NOTE: You MUST set this before using most of the classes/functions below.
 *
 *       The only things you can create without a JSContext are JSString's and
 *       the empty JSFunction constructor.
 */
void AExport SetJSContext(JSContextRef ctx);

/**
 * Get the current JSContext.
 */
JSContextRef AExport GetJSContext();

/**
 * JSString wrapper that automatically manages lifetime
 * and provides helpful conversions.
 */
class AExport JSString {
public:
  // Create empty string
  JSString();

  // Create from C-string
  JSString(const char* str);

  // Create from Ultralight String
  JSString(const String& str);

  // Take ownership of existing JSStringRef
  JSString(JSStringRef str);

  // Copy constructor 
  JSString(const JSString& other);

  ~JSString();

  // Assignment operator
  JSString& operator=(const JSString& other);

  // Cast to String
  operator String();

  // Cast to JSStringRef
  operator JSStringRef() const { return instance_; }

protected:
  JSStringRef instance_;
};

class JSArray;
class JSObject;
class JSFunction;

// Used with the JSValue constructor to create "Null" types
struct AExport JSValueNullTag {};

// Used with the JSValue constructor to create "Undefined" types
struct AExport JSValueUndefinedTag {};

/**
 * JSValue wrapper that automatically manages lifetime and
 * provides helpful conversions.
 */
class AExport JSValue {
public:
  // Create null (empty) JSValue
  JSValue();

  // Create null JSValue explicitly
  JSValue(JSValueNullTag);

  // Create undefined JSValue
  JSValue(JSValueUndefinedTag);

  // Create boolean JSValue
  JSValue(bool val);

  // Create unsigned integer JSValue (aka, Number) [will be cast to double]
  JSValue(uint32_t val);

  // Create integer JSValue (aka, Number) [will be cast to double]
  JSValue(int32_t val);

  // Create unsigned integer JSValue (aka, Number) [will be cast to double]
  JSValue(uint64_t val);

  // Create integer JSValue (aka, Number) [will be cast to double]
  JSValue(int64_t val);

  // Create double JSValue (aka, Number)
  JSValue(double val);

  // Create string JSValue
  JSValue(const char* val);

  // Create string JSValue
  JSValue(const String& val);

  // Create string JSValue
  JSValue(JSString val);

  // Create from existing JSValueRef
  JSValue(JSValueRef val);

  // Create object JSValue
  JSValue(JSObjectRef obj);

  // Copy constructor, a shallow copy is made, will end up pointing to same instance
  JSValue(const JSValue& other);

  virtual ~JSValue();

  // A shallow copy is made, will end up pointing to same instance
  virtual JSValue& operator=(const JSValue& other);

  bool IsNull() const;

  bool IsUndefined() const;

  bool IsBoolean() const;

  bool IsNumber() const;

  bool IsString() const;

  bool IsObject() const;

  bool IsArray() const;

  bool IsFunction() const;

  // Convert to Boolean
  bool ToBoolean() const;

  // Convert to Number (Double)
  double ToNumber() const;

  // Convert to Number (Integer)
  int64_t ToInteger() const { return static_cast<int64_t>(ToNumber()); }

  // Convert to String
  JSString ToString() const;

  // Convert to Object (will debug assert if not an Object)
  JSObject ToObject() const;

  // Convert to Array (will debug asset if not an Array)
  JSArray ToArray() const;

  // Convert to Function (will debug asset if not a Function)
  JSFunction ToFunction() const;

  operator bool() const { return ToBoolean(); }

  operator double() const { return ToNumber(); }

  operator uint32_t() const { return static_cast<uint32_t>(ToNumber()); }

  operator int32_t() const { return static_cast<uint32_t>(ToNumber()); }

  operator uint64_t() const { return static_cast<uint64_t>(ToNumber()); }

  operator int64_t() const { return ToInteger(); }

  operator String() const { return ToString(); }

  operator JSString() const { return ToString(); }

  operator JSObject() const;

  operator JSObjectRef() const;

  operator JSArray() const;

  operator JSFunction() const;

  // Get the underlying JSValueRef
  operator JSValueRef() const { return instance(); }

protected:
  JSValue(JSContextRef ctx);
  JSValue(JSContextRef ctx, JSValueRef val);
  virtual JSValueRef instance() const;

  JSContextRef ctx_;
  JSValueRef instance_ = nullptr;
  friend class JSFunction;
};

/**
* A vector of JSValues, used for passing around arguments in JSCallback.
* We don't expose std::vector directly because of ABI concerns.
*/
class AExport JSArgs {
public:
  JSArgs();
  JSArgs(const std::initializer_list<JSValue>& values);
  JSArgs(const JSArgs& other);
  ~JSArgs();

  JSArgs& operator=(const JSArgs& other);

  JSValue operator[](size_t pos);
  const JSValue operator[](size_t pos) const;

  bool empty() const;

  size_t size() const;

  void clear();

  void push_back(const JSValue& val);
  void pop_back();

  JSValue* data();
  const JSValue* data() const;
protected:
  void* instance_;
};

typedef std::function<void(const JSObject&, const JSArgs&)> JSCallback;
typedef std::function<JSValue(const JSObject&, const JSArgs&)> JSCallbackWithRetval;

// Macro to help bind JSCallback member function to JSPropertyValue
#define BindJSCallback(fn) (JSCallback)std::bind(fn, this, std::placeholders::_1, std::placeholders::_2)
#define BindJSCallbackWithRetval(fn) (JSCallbackWithRetval)std::bind(fn, this, std::placeholders::_1, std::placeholders::_2)

/**
 * Wrapper for JSObject property value (JSValue subclass). Allows new value assignment
 * to object property, binding C++ callbacks to object properties via function objects,
 * as well as value query via the JSValue interface.
 */
class AExport JSPropertyValue : public JSValue {
public:
  virtual ~JSPropertyValue();

  // Assign a new value to the property (internally calls JSObjectSetProperty)
  virtual JSPropertyValue& operator=(const JSValue& value);

  // Bind to native C++ callback (creates a Function object that can be called from JS)
  JSPropertyValue& operator=(const JSCallback& callback);

  // Bind to native C++ callback with return value (creates a Function object that can be called from JS)
  JSPropertyValue& operator=(const JSCallbackWithRetval& callback);

protected:
  virtual JSValueRef instance() const;
  JSPropertyValue(JSContextRef ctx, JSObjectRef proxy_obj, unsigned idx);
  JSPropertyValue(JSContextRef ctx, JSObjectRef proxy_obj, JSString idx);
  JSPropertyValue(const JSPropertyValue&) = default;
  JSPropertyValue& operator=(const JSPropertyValue&) = delete;

  JSObject* proxyObj_;
  bool using_numeric_idx_;
  unsigned numeric_idx_;
  JSString string_idx_;
  friend class JSArray;
  friend class JSObject;
};

/**
 * JSArray wrapper that automatically manages lifetime and provides
 * convenient access to indices and Array functions.
 */
class AExport JSArray {
public:
  // Create empty Array
  JSArray();

  // Create Array from list of JSValues
  JSArray(const std::initializer_list<JSValue>& values);

  // Create Array from existing JSObjectRef (JavaScriptCore C API)
  JSArray(JSObjectRef array_obj);

  // Copy constructor (shallow copy, will point to same instance)
  JSArray(const JSArray& other);

  ~JSArray();

  // Assignment (shallow assignment, will point to same instance)
  JSArray& operator=(const JSArray& other);

  // Get number of elements in the Array
  unsigned length();

  // Push an element to back of Array
  void push(const JSValue& val);

  // Find the index (location) of a certain value, will return -1 if not found
  int indexOf(const JSValue& val, int start = 0) const;

  // Get a property by array index (numbering starts at 0)
  JSPropertyValue operator[](unsigned idx) const;

  // Get the underlying JSObjectRef (JavaScriptCore C API)
  operator JSObjectRef() const { return instance_; }

protected:
  JSArray(JSContextRef ctx, JSValueRef val);

  JSContextRef ctx_;
  JSObjectRef instance_;
  friend class JSValue;
};

/**
 * JSObject wrapper that automatically manages lifetime and provides
 * convenient access to properties.
 */
class AExport JSObject {
public:
  // Create empty Object
  JSObject();

  // Create from existing JSObjectRef from JavaScriptCore C API
  JSObject(JSObjectRef obj);
  
  // Copy constructor (shallow copy, will point to same instance)
  JSObject(const JSObject& other);

  ~JSObject();

  // Assignment (shallow assignment, will point to same instance)
  JSObject& operator=(const JSObject& other);

  // Get a property by name
  JSPropertyValue operator[](JSString propertyName) const;

  // Check if a property exists
  bool HasProperty(JSString propertyName) const;

  // Remove a property
  bool DeleteProperty(JSString propertyName);

  // Get the underlying JSObjectRef (JavaScriptCore C API)
  operator JSObjectRef() const { return instance_; }

protected:
  JSObject(JSContextRef ctx, JSValueRef val);
  JSObject(JSContextRef ctx, JSObjectRef obj);

  JSContextRef ctx_;
  JSObjectRef instance_;
  friend class JSValue;
  friend class JSPropertyValue;
};

/**
* JSFunction wrapper that automatically manages lifetime and provides
* convenient function invocation operators.
*/
class AExport JSFunction {
public:
  // Create an empty Function.
  // NOTE: It is OKAY to create this without calling SetJSContext() first.
  JSFunction();

  // Copy constructor (shallow copy, will point to same instance)
  JSFunction(const JSFunction& other);

  ~JSFunction();

  // Assignment (shallow assignment, will point to same instance)
  JSFunction& operator=(const JSFunction& other);

  // Whether or not this is a valid, callable Function object.
  bool IsValid() const;

  // Call function (using Global Object for 'this') and return the result.
  JSValue operator()(const JSArgs& args);

  // Call function (with explicit object for 'this') and return the result
  JSValue operator()(const JSObject& thisObject, const JSArgs& args);

  // Get the underlying JSObjectRef (JavaScriptCore C API)
  operator JSObjectRef() const { return instance_; }

protected:
  JSFunction(JSContextRef ctx, JSValueRef val);

  JSContextRef ctx_;
  JSObjectRef instance_;
  friend class JSValue;  
};

/**
 * Get the Global Object for the current JSContext.
 * In JavaScript, this would be equivalent to the "window" object.
 */
JSObject AExport JSGlobalObject();

/**
 * Evaluate a string of JavaScript and return a result.
 */
JSValue AExport JSEval(const JSString& str);

}  // namespace ultralight
