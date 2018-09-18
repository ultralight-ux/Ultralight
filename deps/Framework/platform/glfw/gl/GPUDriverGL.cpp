#include "GPUDriverGL.h"
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/FileSystem.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define SHADER_PATH "glsl/"

#ifdef _DEBUG
#if _WIN32
#define INFO(x) { std::cerr << "[INFO] " << __FUNCSIG__ << " @ Line " << __LINE__ << ":\n\t" << x << std::endl; }
#else
#define INFO(x) { std::cerr << "[INFO] " << __PRETTY_FUNCTION__ << " @ Line " << __LINE__ << ":\n\t" << x << std::endl; }
#endif
#else
#define INFO(x)
#endif

#if _WIN32
#define FATAL(x) { std::cerr << "[ERROR] " << __FUNCSIG__ << " @ Line " << __LINE__ << ":\n\t" << x << std::endl; \
  __debugbreak(); std::cin.get(); exit(-1); }
#else
#define FATAL(x) { std::cerr << "[ERROR] " << __PRETTY_FUNCTION__ << " @ Line " << __LINE__ << ":\n\t" << x << std::endl; \
  std::cin.get(); exit(-1); }
#endif

static void ReadFile(const char* filepath, std::string& result) {
  // To maintain predictable behavior across platforms we use
  // whatever FileSystem that Ultralight is using:
  ultralight::FileSystem* fs = ultralight::Platform::instance().file_system();
  if (!fs)
    FATAL("No FileSystem defined.");

  auto handle = fs->OpenFile(filepath, false);
  if (handle == ultralight::invalidFileHandle)
    FATAL("Could not open file path: " << filepath);

  int64_t fileSize = 0;
  fs->GetFileSize(handle, fileSize);
  result.resize((size_t)fileSize);
  fs->ReadFromFile(handle, &result[0], fileSize);
  fs->CloseFile(handle);
}

inline char const* glErrorString(GLenum const err) noexcept
{
  switch (err)
  {
  // OpenGL 2.0+ Errors:
  case GL_NO_ERROR: return "GL_NO_ERROR";
  case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
  case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
  case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
  case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
  case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
  case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
  // OpenGL 3.0+ Errors
  case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
  default: return "UNKNOWN ERROR";
  }
}

inline std::string GetShaderLog(GLuint shader_id) {
  GLint length, result;
  glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
  std::string str(length, ' ');
  glGetShaderInfoLog(shader_id, (GLsizei)str.length(), &result, &str[0]);
  return str;
}

inline std::string GetProgramLog(GLuint program_id) {
  GLint length, result;
  glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);
  std::string str(length, ' ');
  glGetProgramInfoLog(program_id, (GLsizei)str.length(), &result, &str[0]);
  return str;
}

static GLuint LoadShaderFromFile(GLenum shader_type, const char* filename) {
  std::string shader_source;
  std::string path = std::string(SHADER_PATH) + filename;
  ReadFile(path.c_str(), shader_source);
  GLint compileStatus;
  const char* shader_source_str = shader_source.c_str();
  GLuint shader_id = glCreateShader(shader_type);
  glShaderSource(shader_id, 1, &shader_source_str, NULL);
  glCompileShader(shader_id);
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compileStatus);
  if (compileStatus == GL_FALSE)
    FATAL("Unable to compile shader. Filename: " << filename << "\n\tError:" 
      << glErrorString(glGetError()) << "\n\tLog: " << GetShaderLog(shader_id))
  return shader_id;
}

#ifdef _DEBUG
#define CHECK_GL()  {if (GLenum err = glGetError()) FATAL(glErrorString(err)) }                                                     
#else
#define CHECK_GL()
#endif   

namespace ultralight {

GPUDriverGL::GPUDriverGL(GLuint viewport_width, GLuint viewport_height, GLfloat scale) :
  viewport_width_(viewport_width),
  viewport_height_(viewport_height),
  scale_(scale) {
  // Render Buffer ID 0 is reserved for the screen, set its viewport dimensions now.
  frame_buffer_map[0] = { 0, viewport_width, viewport_height };
}

void GPUDriverGL::CreateTexture(uint32_t texture_id,
  Ref<Bitmap> bitmap) {
  GLuint tex_id;
  glGenTextures(1, &tex_id);
  texture_map[texture_id] = tex_id;

  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, tex_id);

  CHECK_GL();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  CHECK_GL();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  CHECK_GL();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, bitmap->row_bytes() / bitmap->bpp());
  CHECK_GL();

  if (bitmap->format() == kBitmapFormat_A8) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, bitmap->width(), bitmap->height(), 0,
      GL_RED, GL_UNSIGNED_BYTE, bitmap->pixels());
  } else if (bitmap->format() == kBitmapFormat_RGBA8) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap->width(), bitmap->height(), 0,
      GL_BGRA, GL_UNSIGNED_BYTE, bitmap->pixels());
  } else {
    FATAL("Unhandled texture format: " << bitmap->format())
  }

  CHECK_GL();
  glGenerateMipmap(GL_TEXTURE_2D);
  CHECK_GL();
}

void GPUDriverGL::UpdateTexture(uint32_t texture_id,
  Ref<Bitmap> bitmap) {
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, texture_map[texture_id]);
  CHECK_GL();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, bitmap->row_bytes() / bitmap->bpp());

  if (!bitmap->IsEmpty()) {
    if (bitmap->format() == kBitmapFormat_A8)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, bitmap->width(), bitmap->height(), 0,
        GL_RED, GL_UNSIGNED_BYTE, bitmap->pixels());
    else if (bitmap->format() == kBitmapFormat_RGBA8)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap->width(), bitmap->height(), 0,
        GL_BGRA, GL_UNSIGNED_BYTE, bitmap->pixels());
    else
      FATAL("Unhandled texture format: " << bitmap->format());

    CHECK_GL();
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  CHECK_GL();
}

void GPUDriverGL::BindTexture(uint8_t texture_unit, uint32_t texture_id) {
  glActiveTexture(GL_TEXTURE0 + texture_unit);
  glBindTexture(GL_TEXTURE_2D, texture_map[texture_id]);
  CHECK_GL();
}

void GPUDriverGL::DestroyTexture(uint32_t texture_id) {
  GLuint tex_id = texture_map[texture_id];
  glDeleteTextures(1, &tex_id);
}

void GPUDriverGL::CreateRenderBuffer(uint32_t render_buffer_id,
  const RenderBuffer& buffer) {
  if (render_buffer_id == 0) {
    INFO("Should not be reached! Render Buffer ID 0 is reserved for default framebuffer.");
    return;
  }

  GLuint frame_buffer_id = 0;
  glGenFramebuffers(1, &frame_buffer_id);
  CHECK_GL();
  frame_buffer_map[render_buffer_id] = { frame_buffer_id, 
    (GLuint)buffer.viewport_width, (GLuint)buffer.viewport_height };
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);
  CHECK_GL();

  glViewport(0, 0, buffer.viewport_width, buffer.viewport_height);

  GLuint rbuf_texture_id = texture_map[buffer.texture_id];
  glBindTexture(GL_TEXTURE_2D, rbuf_texture_id);
  CHECK_GL();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rbuf_texture_id, 0);
  CHECK_GL();

  GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, drawBuffers);
  CHECK_GL();

  GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (result != GL_FRAMEBUFFER_COMPLETE)
    FATAL("Error creating FBO: " << result);
  CHECK_GL();
}

void GPUDriverGL::BindRenderBuffer(uint32_t render_buffer_id) {
  FrameBufferInfo framebuffer = frame_buffer_map[render_buffer_id];
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
  CHECK_GL();
  render_buffer_width_ = (GLfloat)framebuffer.width;
  render_buffer_height_ = (GLfloat)framebuffer.height;
  glViewport(0, 0, static_cast<GLsizei>(framebuffer.width * scale_),
                   static_cast<GLsizei>(framebuffer.height * scale_));
}

void GPUDriverGL::SetRenderBufferViewport(uint32_t render_buffer_id, uint32_t width, uint32_t height) {
  auto i = frame_buffer_map.find(render_buffer_id);
  if (i != frame_buffer_map.end()) {
    i->second.width = width;
    i->second.height = height;
  }
}

void GPUDriverGL::ClearRenderBuffer(uint32_t render_buffer_id) {
  BindRenderBuffer(render_buffer_id);
  glDisable(GL_SCISSOR_TEST);
  CHECK_GL();
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  CHECK_GL();
  glClear(GL_COLOR_BUFFER_BIT);
  CHECK_GL();
}

void GPUDriverGL::DestroyRenderBuffer(uint32_t render_buffer_id) {
  if (render_buffer_id == 0)
    return;
  FrameBufferInfo framebuffer = frame_buffer_map[render_buffer_id];
  glDeleteFramebuffers(1, &framebuffer.id);
}


void GPUDriverGL::CreateGeometry(uint32_t geometry_id,
  const VertexBuffer& vertices,
  const IndexBuffer& indices) {
  GeometryEntry geometry;
  glGenVertexArrays(1, &geometry.vao);
  glBindVertexArray(geometry.vao);

  glGenBuffers(1, &geometry.vbo_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, geometry.vbo_vertices);
  glBufferData(GL_ARRAY_BUFFER, vertices.size, vertices.data, GL_DYNAMIC_DRAW);

  if (vertices.format == kVertexBufferFormat_2f_4ub_2f_2f_28f) {
    GLsizei stride = 140;

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (GLvoid*)8);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)12);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)20);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)28);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)44);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)60);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)76);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)92);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)108);
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)124);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glEnableVertexAttribArray(6);
    glEnableVertexAttribArray(7);
    glEnableVertexAttribArray(8);
    glEnableVertexAttribArray(9);
    glEnableVertexAttribArray(10);

    CHECK_GL();
  } else {
    FATAL("Unhandled vertex format: " << vertices.format);
  }

  glGenBuffers(1, &geometry.vbo_indices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.vbo_indices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size, indices.data,
    GL_STATIC_DRAW);
  CHECK_GL();

  geometry_map[geometry_id] = geometry;

  glBindVertexArray(0);
  CHECK_GL();
}

void GPUDriverGL::UpdateGeometry(uint32_t geometry_id,
  const VertexBuffer& vertices, const IndexBuffer& indices) {
  GeometryEntry& geometry = geometry_map[geometry_id];
  glBindVertexArray(geometry.vao);
  CHECK_GL();
  glBindBuffer(GL_ARRAY_BUFFER, geometry.vbo_vertices);
  glBufferData(GL_ARRAY_BUFFER, vertices.size, vertices.data, GL_DYNAMIC_DRAW);
  CHECK_GL();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.vbo_indices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size, indices.data, GL_STATIC_DRAW);
  CHECK_GL();
  glBindVertexArray(0);
  CHECK_GL();
}

void GPUDriverGL::DrawGeometry(uint32_t geometry_id,
  uint32_t indices_count,
  uint32_t indices_offset,
  const GPUState& state) {
  if (programs_.empty())
    LoadPrograms();

  BindRenderBuffer(state.render_buffer_id);

  GeometryEntry& geometry = geometry_map[geometry_id];
  SelectProgram((ProgramType)state.shader_type);
  SetUniformDefaults();
  CHECK_GL();
  if (state.render_buffer_id == 0) {
    // Anything drawn to screen needs to be flipped vertically to compensate for
    // flipping in vertex shader.
    ultralight::Matrix4x4 mat = state.transform;
    mat.data[4] *= -1.0;
    mat.data[5] *= -1.0;
    mat.data[12] += -render_buffer_height_ * mat.data[4];
    mat.data[13] += -render_buffer_height_ * mat.data[5];
    SetUniformMatrix4fv("Transform", 1, &mat.data[0]);
  } else {
    SetUniformMatrix4fv("Transform", 1, &state.transform.data[0]);
  }
  CHECK_GL();
  SetUniform4fv("Scalar4", 2, &state.uniform_scalar[0]);
  CHECK_GL();
  SetUniform4fv("Vector", 8, &state.uniform_vector[0].value[0]);
  CHECK_GL();
  SetUniform1ui("ClipSize", state.clip_size);
  CHECK_GL();
  SetUniformMatrix4fv("Clip", 8, &state.clip[0].data[0]);

  CHECK_GL();

  glBindVertexArray(geometry.vao);

  CHECK_GL();

  BindTexture(0, state.texture_1_id);
  BindTexture(1, state.texture_2_id);
  BindTexture(2, state.texture_3_id);

  CHECK_GL();

  glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT,
    (GLvoid*)(indices_offset * sizeof(unsigned int)));

  glBindVertexArray(0);

  batch_count_++;

  CHECK_GL();
}

void GPUDriverGL::DestroyGeometry(uint32_t geometry_id) {
  GeometryEntry& geometry = geometry_map[geometry_id];

  glDeleteBuffers(1, &geometry.vbo_indices);
  glDeleteBuffers(1, &geometry.vbo_vertices);

  glDeleteVertexArrays(1, &geometry.vao);

  geometry_map.erase(geometry_id);
}

void GPUDriverGL::UpdateCommandList(const CommandList& list) {
  if (list.size) {
    command_list.resize(list.size);
    memcpy(&command_list[0], list.commands, sizeof(Command) * list.size);
  }
}

void GPUDriverGL::DrawCommandList() {
  if (command_list.empty())
    return;

  CHECK_GL();

  batch_count_ = 0;

  glEnable(GL_BLEND);
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_DEPTH_TEST);
  glDepthFunc(GL_NEVER);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

  CHECK_GL();

  for (auto i = command_list.begin(); i != command_list.end(); ++i) {
    switch (i->command_type) {
    case kCommandType_DrawGeometry:
      DrawGeometry(i->geometry_id, i->indices_count, i->indices_offset, i->gpu_state);
      break;
    case kCommandType_ClearRenderBuffer:
      ClearRenderBuffer(i->gpu_state.render_buffer_id);
      break;
    };
  }

  command_list.clear();
  glDisable(GL_SCISSOR_TEST);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GPUDriverGL::SaveRenderState() {
  render_state_.shader_program = 0;
  glGetIntegerv(GL_CURRENT_PROGRAM, &render_state_.shader_program);

  render_state_.blend_state = 0;
  glGetBooleanv(GL_BLEND, &render_state_.blend_state);

  if (render_state_.blend_state) {
    render_state_.blend_src = GL_ONE;
    render_state_.blend_dst = GL_ONE_MINUS_SRC_ALPHA;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &render_state_.blend_src);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &render_state_.blend_dst);
  }

  render_state_.depth_state = 0;
  glGetBooleanv(GL_DEPTH_TEST, &render_state_.depth_state);

  if (render_state_.depth_state) {
    render_state_.depth_func = GL_NEVER;
    glGetIntegerv(GL_DEPTH_FUNC, &render_state_.depth_func);
  }

  render_state_.scissor_state = 0;
  glGetBooleanv(GL_SCISSOR_TEST, &render_state_.scissor_state);
}

void GPUDriverGL::RestoreRenderState() {
  glUseProgram(render_state_.shader_program);

  if (render_state_.blend_state) {
    glEnable(GL_BLEND);
    glBlendFunc(render_state_.blend_src, render_state_.blend_dst);
  }
  else {
    glDisable(GL_BLEND);
  }

  if (render_state_.depth_state) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(render_state_.depth_func);
  }
  else {
    glDisable(GL_DEPTH_TEST);
  }

  if (render_state_.scissor_state) {
    glEnable(GL_SCISSOR_TEST);
  }
  else {
    glDisable(GL_SCISSOR_TEST);
  }
}

void GPUDriverGL::BindUltralightTexture(uint32_t ultralight_texture_id) {
  GLuint tex_id = texture_map[ultralight_texture_id];
  glBindTexture(GL_TEXTURE_2D, tex_id);
}

void GPUDriverGL::ResizeViewport(int width, int height) {
  SetRenderBufferViewport(0, width, height);
}

void GPUDriverGL::LoadPrograms(void) {
  const char* vert_program = "vs/v2f_c4f_t2f_t2f_d28f.vert";
  LoadProgram(ultralight::kShaderType_Fill, vert_program, "ps/fill.frag");
}

void GPUDriverGL::DestroyPrograms(void) {
  GLenum ErrorCheckValue = glGetError();
  glUseProgram(0);
  for (auto i = programs_.begin(); i != programs_.end(); i++) {
    ProgramEntry& prog = i->second;
    glDetachShader(prog.program_id, prog.vert_shader_id);
    glDetachShader(prog.program_id, prog.frag_shader_id);
    glDeleteShader(prog.vert_shader_id);
    glDeleteShader(prog.frag_shader_id);
    glDeleteProgram(prog.program_id);
  }
  programs_.clear();
}

void GPUDriverGL::LoadProgram(ProgramType type,
  const char* vert, const char* frag) {
  GLenum ErrorCheckValue = glGetError();
  ProgramEntry prog;
  prog.vert_shader_id = LoadShaderFromFile(GL_VERTEX_SHADER, vert);
  prog.frag_shader_id = LoadShaderFromFile(GL_FRAGMENT_SHADER, frag);
  prog.program_id = glCreateProgram();
  glAttachShader(prog.program_id, prog.vert_shader_id);
  glAttachShader(prog.program_id, prog.frag_shader_id);

  glBindAttribLocation(prog.program_id, 0, "in_Position");
  glBindAttribLocation(prog.program_id, 1, "in_Color");
  glBindAttribLocation(prog.program_id, 2, "in_TexCoord");
  glBindAttribLocation(prog.program_id, 3, "in_ObjCoord");
  glBindAttribLocation(prog.program_id, 4, "in_Data0");
  glBindAttribLocation(prog.program_id, 5, "in_Data1");
  glBindAttribLocation(prog.program_id, 6, "in_Data2");
  glBindAttribLocation(prog.program_id, 7, "in_Data3");
  glBindAttribLocation(prog.program_id, 8, "in_Data4");
  glBindAttribLocation(prog.program_id, 9, "in_Data5");
  glBindAttribLocation(prog.program_id, 10, "in_Data6");

  glLinkProgram(prog.program_id);
  glUseProgram(prog.program_id);
  glUniform1i(glGetUniformLocation(prog.program_id, "Texture1"), 0);
  glUniform1i(glGetUniformLocation(prog.program_id, "Texture2"), 1);
  glUniform1i(glGetUniformLocation(prog.program_id, "Texture3"), 2);

  if (glGetError())
    FATAL("Unable to link shader.\n\tError:" << glErrorString(glGetError()) << "\n\tLog: " << GetProgramLog(prog.program_id))
 
  programs_[type] = prog;
}

void GPUDriverGL::SelectProgram(ProgramType type) {
  auto i = programs_.find(type);
  if (i != programs_.end()) {
    cur_program_id_ = i->second.program_id;
    glUseProgram(i->second.program_id);
  } else {
    FATAL("Missing shader type: " << type);
  }
}

void GPUDriverGL::SetUniformDefaults() {
  float params[4] = { (float)(glfwGetTime() / 1000.0), (float)render_buffer_width_, (float)render_buffer_height_, scale_ };
  SetUniform4f("State", params);
}

void GPUDriverGL::SetUniform1ui(const char* name, GLuint val) {
  glUniform1ui(glGetUniformLocation(cur_program_id_, name), val);
}

void GPUDriverGL::SetUniform1f(const char* name, float val) {
  glUniform1f(glGetUniformLocation(cur_program_id_, name), (GLfloat)val);
}

void GPUDriverGL::SetUniform1fv(const char* name, size_t count, const float* val) {
  glUniform1fv(glGetUniformLocation(cur_program_id_, name), (GLsizei)count, val);
}

void GPUDriverGL::SetUniform4f(const char* name, const float val[4]) {
  glUniform4f(glGetUniformLocation(cur_program_id_, name),
    (GLfloat)val[0], (GLfloat)val[1], (GLfloat)val[2], (GLfloat)val[3]);
}

void GPUDriverGL::SetUniform4fv(const char* name, size_t count, const float* val) {
  glUniform4fv(glGetUniformLocation(cur_program_id_, name), (GLsizei)count, val);
}

void GPUDriverGL::SetUniformMatrix4fv(const char* name, size_t count, const float* val) {
  glUniformMatrix4fv(glGetUniformLocation(cur_program_id_, name), (GLsizei)count, false, val);
}

}  // namespace ultralight
