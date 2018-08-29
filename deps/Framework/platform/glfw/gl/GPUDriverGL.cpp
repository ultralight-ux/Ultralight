#include "GPUDriverGL.h"
#include <iostream>
#include <fstream>
#include <sstream>

#define VISUALIZE_BATCH 0
#if _WIN32
#define SHADER_PATH "assets\\glsl\\"
#else
#define SHADER_PATH "/Users/adam/Dev/ultralight/Source/Samples/common/shaders/glsl/"
#endif

// The vertices of our 3D quad
const GLfloat GVertices[] = {
  -1.0f, -1.0f, 0.0f,
  1.0f, -1.0f, 0.0f,
  -1.0f,  1.0f, 0.0f,
  1.0f,  1.0f, 0.0f,
};

// The UV texture coordinates of our 3D quad
const GLshort GTextures[] = {
  0, 1,
  1, 1,
  0, 0,
  1, 0,
};

struct Vertex {
  float x;
  float y;
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
  float u;
  float v;
};

void ReadFile(const char* filepath, std::string& result) {
  std::ifstream filestream(filepath, std::ios::binary);
  if (!filestream) {
    std::cerr << "Could not open %s\n" << filepath << std::endl;
    std::cin.get();
    exit(-1);
  }
  std::stringstream ostrm;
  ostrm << filestream.rdbuf();
  result = ostrm.str();
}

char const* glErrorString(GLenum const err) noexcept
{
  switch (err)
  {
    // opengl 2 errors (8)
  case GL_NO_ERROR:
    return "GL_NO_ERROR";

  case GL_INVALID_ENUM:
    return "GL_INVALID_ENUM";

  case GL_INVALID_VALUE:
    return "GL_INVALID_VALUE";

  case GL_INVALID_OPERATION:
    return "GL_INVALID_OPERATION";

  case GL_STACK_OVERFLOW:
    return "GL_STACK_OVERFLOW";

  case GL_STACK_UNDERFLOW:
    return "GL_STACK_UNDERFLOW";

  case GL_OUT_OF_MEMORY:
    return "GL_OUT_OF_MEMORY";

    // opengl 3 errors (1)
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "GL_INVALID_FRAMEBUFFER_OPERATION";

    // gles 2, 3 and gl 4 error are handled by the switch above
  default:
    return "UNKNOWN ERROR";
  }
}

GLuint LoadShaderFromFile(GLenum shader_type, const char* filename) {
  std::string shader_source;
  std::string path = std::string(SHADER_PATH) + filename;
  ReadFile(path.c_str(), shader_source);
  GLenum ErrorCheckValue = glGetError();
  GLint compileStatus;
  const char* shader_source_str = shader_source.c_str();
  GLuint shader_id = glCreateShader(shader_type);
  glShaderSource(shader_id, 1, &shader_source_str, NULL);
  glCompileShader(shader_id);
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compileStatus);
  if (compileStatus == GL_FALSE) {
    char *log;
    GLint length, result;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
    log = (char*)malloc(length);
    glGetShaderInfoLog(shader_id, length, &result, log);
    std::cerr <<
      "Error in LoadShaderFromFile(): Unable to compile shader: \n\tFilename: " <<
      filename << "\n\tError: " << glErrorString(ErrorCheckValue) << "\n\tLog: " <<
      log << std::endl;
    free(log);
    std::cin.get();
    exit(-1);
  }
  return shader_id;
}

extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }


void CheckGLError(const char* func, int line) {
  GLenum ErrorCheckValue = glGetError();
  if (ErrorCheckValue != GL_NO_ERROR) {
    fprintf(
      stderr,
      "ERROR in %s on line %d -- %s \n",
      func,
      line,
      glErrorString(ErrorCheckValue)
    );
    // __debugbreak();
  }
}


#if defined(NDEBUG) || 1
#define LOG(x)
#else
#define LOG(x) std::cerr << x << std::endl;
#endif

#if _WIN32
#define CHECK_GL() CheckGLError(__FUNCSIG__, __LINE__);
#else
#define CHECK_GL() CheckGLError(__PRETTY_FUNCTION__, __LINE__);
#endif

namespace ultralight {

GPUDriverGL::GPUDriverGL(GLuint viewport_width, GLuint viewport_height, GLfloat scale) :
  viewport_width_(viewport_width),
  viewport_height_(viewport_height),
  scale_(scale) {
  // Render Buffer Id = 0 always maps to Frame Buffer 0
  frame_buffer_map[0] = { 0, viewport_width, viewport_height };
  LoadPrograms();
}

void GPUDriverGL::CreateTexture(uint32_t texture_id,
  Ref<Bitmap> bitmap) {
  LOG("CreateTexture, id: " << texture_id);

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

  //GLfloat largest_supported_anisotropy;
  //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
  //                largest_supported_anisotropy);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, bitmap->row_bytes() / bitmap->bpp());

  CHECK_GL();

  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  if (bitmap->format() == kBitmapFormat_A8) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, bitmap->width(), bitmap->height(), 0,
      GL_RED, GL_UNSIGNED_BYTE, bitmap->pixels());
  }
  else if (bitmap->format() == kBitmapFormat_RGBA8) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap->width(), bitmap->height(), 0,
      GL_BGRA, GL_UNSIGNED_BYTE, bitmap->pixels());
  }
  else {
    std::cout << "WARNING: UNHANDLED TEXTURE FORMAT: " << bitmap->format() << std::endl;
  }

  CHECK_GL();

  glGenerateMipmap(GL_TEXTURE_2D);

  CHECK_GL();
}

void GPUDriverGL::UpdateTexture(uint32_t texture_id,
  Ref<Bitmap> bitmap) {
  LOG("UpdateTexture, id: " << texture_id);

  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, texture_map[texture_id]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, bitmap->row_bytes() / bitmap->bpp());

  if (!bitmap->IsEmpty()) {
    if (bitmap->format() == kBitmapFormat_A8) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, bitmap->width(), bitmap->height(), 0,
        GL_RED, GL_UNSIGNED_BYTE, bitmap->pixels());
    }
    else if (bitmap->format() == kBitmapFormat_RGBA8) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap->width(), bitmap->height(), 0,
        GL_BGRA, GL_UNSIGNED_BYTE, bitmap->pixels());
    }
    else {
      std::cout << "WARNING: UNHANDLED TEXTURE FORMAT: " << bitmap->format() << std::endl;
    }

    glGenerateMipmap(GL_TEXTURE_2D);
  }

  CHECK_GL();
}

void GPUDriverGL::BindTexture(uint8_t texture_unit, uint32_t texture_id) {
  LOG("BindTexture, tex unit: " << (uint32_t)texture_unit << ", id: " << texture_id);

  glActiveTexture(GL_TEXTURE0 + texture_unit);
  glBindTexture(GL_TEXTURE_2D, texture_map[texture_id]);
}

void GPUDriverGL::DestroyTexture(uint32_t texture_id) {
  LOG("DestroyTexture, id: " << texture_id);

  GLuint tex_id = texture_map[texture_id];
  glDeleteTextures(1, &tex_id);
}

void GPUDriverGL::CreateRenderBuffer(uint32_t render_buffer_id,
  const RenderBuffer& buffer) {
  LOG("CreateRenderBuffer, id:" << render_buffer_id << ", tex id: " << buffer.texture_id);

  if (render_buffer_id == 0) {
    LOG("This should not be reached! Render Buffer ID 0 is reserved for default framebuffer.");
    return;
  }

  CHECK_GL();

  GLuint frame_buffer_id = 0;
  glGenFramebuffers(1, &frame_buffer_id);
  CHECK_GL();
  frame_buffer_map[render_buffer_id] = { frame_buffer_id, 
    (GLuint)buffer.viewport_width, (GLuint)buffer.viewport_height };

  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);
  CHECK_GL();

  glViewport(0, 0, buffer.viewport_width, buffer.viewport_height);
  CHECK_GL();

  GLuint rbuf_texture_id = texture_map[buffer.texture_id];
  glBindTexture(GL_TEXTURE_2D, rbuf_texture_id);
  CHECK_GL();
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, rbuf_texture_id, 0);
  CHECK_GL();

  GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, drawBuffers);
  CHECK_GL();

  GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (result != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR in CreateRenderBuffer: " << result << std::endl;
  CHECK_GL();
}

void GPUDriverGL::BindRenderBuffer(uint32_t render_buffer_id) {
  LOG("BindRenderBuffer, id: " << render_buffer_id);

  /*
  if (render_buffer_id == 0) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    render_buffer_width_ = 
    return;
  }
  */

  FrameBufferInfo framebuffer = frame_buffer_map[render_buffer_id];
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);

  render_buffer_width_ = framebuffer.width;
  render_buffer_height_ = framebuffer.height;

  glViewport(0, 0, framebuffer.width * scale_, framebuffer.height * scale_);
}

void GPUDriverGL::SetRenderBufferViewport(uint32_t render_buffer_id,
  uint32_t width,
  uint32_t height) {
  auto i = frame_buffer_map.find(render_buffer_id);
  if (i != frame_buffer_map.end()) {
    i->second.width = width;
    i->second.height = height;
  }
}

void GPUDriverGL::ClearRenderBuffer(uint32_t render_buffer_id) {
  CHECK_GL();
  LOG("ClearRenderBuffer");

  BindRenderBuffer(render_buffer_id);

  glDisable(GL_SCISSOR_TEST);

  CHECK_GL();

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  CHECK_GL();

  glClear(GL_COLOR_BUFFER_BIT);

  CHECK_GL();

  //glEnable(GL_SCISSOR_TEST);

  //CHECK_GL();
}

void GPUDriverGL::DestroyRenderBuffer(uint32_t render_buffer_id) {
  LOG("DestroyRenderBuffer, id: " << render_buffer_id);
  //std::cout << "GPUDriverGL::DestroyRenderBuffer\n";

  if (render_buffer_id == 0)
    return;

  // TODO
}

#define DECLARE_VERTEX_ATTRIB_FLOAT(size) \
  glVertexAttribPointer(slot, size, GL_FLOAT, GL_FALSE, sizeof(Vertex_2f_4ub_2f_2f_28f), (GLvoid*)offset); \
  LOG("DECLARE_FLOAT " << slot << ", " << size << ", " << offset); \
  offset += sizeof(float)*size; \
  glEnableVertexAttribArray(slot++);

#define DECLARE_VERTEX_ATTRIB_UNSIGNED_BYTE(size) \
  glVertexAttribPointer(slot, size, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex_2f_4ub_2f_2f_28f), (GLvoid*)offset); \
  LOG("DECLARE_UBYTE " << slot << ", " << size << ", " << offset); \
  offset += size; \
  glEnableVertexAttribArray(slot++);

void GPUDriverGL::CreateGeometry(uint32_t geometry_id,
  const VertexBuffer& vertices,
  const IndexBuffer& indices) {

  LOG("CreateGeometry, geo id: " << geometry_id << ", vertex format: " << vertices.format << ", num vertices: " << vertices.size << ", indices: " << indices.size);
  LOG("Vertex Size: " << sizeof(Vertex_2f_4ub_2f_2f_28f));

  GeometryEntry geometry;
  glGenVertexArrays(1, &geometry.vao); // Create our Vertex Array Object
  glBindVertexArray(geometry.vao);

  glGenBuffers(1, &geometry.vbo_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, geometry.vbo_vertices);
  glBufferData(GL_ARRAY_BUFFER, vertices.size, vertices.data, GL_DYNAMIC_DRAW);

  if (vertices.format == kVertexBufferFormat_2f_4ub_2f_2f_28f) {
    size_t offset = 0;
    size_t slot = 0;
    /*
    DECLARE_VERTEX_ATTRIB_FLOAT(2);
    DECLARE_VERTEX_ATTRIB_UNSIGNED_BYTE(4)
    DECLARE_VERTEX_ATTRIB_FLOAT(2);
    DECLARE_VERTEX_ATTRIB_FLOAT(2);
    DECLARE_VERTEX_ATTRIB_FLOAT(4);
    DECLARE_VERTEX_ATTRIB_FLOAT(4);
    DECLARE_VERTEX_ATTRIB_FLOAT(4);
    DECLARE_VERTEX_ATTRIB_FLOAT(4);
    DECLARE_VERTEX_ATTRIB_FLOAT(4);
    DECLARE_VERTEX_ATTRIB_FLOAT(4);
    DECLARE_VERTEX_ATTRIB_FLOAT(4);
    */
    /*
    Vertex Size: stride
    DECLARE_FLOAT 0, 2, 0
    DECLARE_UBYTE 1, 4, 8
    DECLARE_FLOAT 2, 2, 12
    DECLARE_FLOAT 3, 2, 20
    DECLARE_FLOAT 4, 4, 28
    DECLARE_FLOAT 5, 4, 44
    DECLARE_FLOAT 6, 4, 60
    DECLARE_FLOAT 7, 4, 76
    DECLARE_FLOAT 8, 4, 92
    DECLARE_FLOAT 9, 4, 108
    DECLARE_FLOAT 10, 4, 124
    */
    size_t stride = 140;

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


  }
  else {
    std::cout << "WARNING: UNHANDLED VERTEX FORMAT: " << vertices.format << std::endl;
    return;
  }

  glGenBuffers(1, &geometry.vbo_indices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.vbo_indices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size, indices.data,
    GL_STATIC_DRAW);

  geometry_map[geometry_id] = geometry;

  glBindVertexArray(0);

  CHECK_GL();
}

void GPUDriverGL::UpdateGeometry(uint32_t geometry_id,
  const VertexBuffer& vertices, const IndexBuffer& indices) {
  LOG("UpdateGeometry, geo id: " << geometry_id << ", vertex format: " << vertices.format << ", num vertices: " << vertices.size / sizeof(Vertex_2f_4ub_2f_2f_28f));

  GeometryEntry& geometry = geometry_map[geometry_id];
  glBindVertexArray(geometry.vao);

  glBindBuffer(GL_ARRAY_BUFFER, geometry.vbo_vertices);
  glBufferData(GL_ARRAY_BUFFER, vertices.size, vertices.data, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.vbo_indices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size, indices.data, GL_STATIC_DRAW);

  glBindVertexArray(0);

  CHECK_GL();
}

void GPUDriverGL::DrawGeometry(uint32_t geometry_id,
  uint32_t indices_count,
  uint32_t indices_offset,
  const GPUState& state) {
  //std::cerr << "DrawGeometry" << std::endl;


  CHECK_GL();

  LOG("DrawGeometry, geo id: " << geometry_id << ", program: " << (ShaderType)state.shader_type << ", num indices: " << indices_count);

  BindRenderBuffer(state.render_buffer_id);

  GeometryEntry& geometry = geometry_map[geometry_id];

#if VISUALIZE_BATCH

  SelectProgram(kShaderType_Visualize_Batch);
  SetUniformDefaults();
  CHECK_GL();
  SetUniformMatrix4fv("Transform", 1, &state.transform.data[0]);
  CHECK_GL();
  SetUniform1fv("Scalar", 8, &state.uniform_scalar[0]);
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

#else

  CHECK_GL();

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
#endif


  glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT,
    (GLvoid*)(indices_offset * sizeof(unsigned int)));

  glBindVertexArray(0);

  batch_count_++;

  CHECK_GL();

  LOG("DrawGeometry ---- END");
}

void GPUDriverGL::DestroyGeometry(uint32_t geometry_id) {
  //std::cerr << "DestroyGeometry" << std::endl;

  LOG("DestroyGeometry, geo id: " << geometry_id);
  GeometryEntry& geometry = geometry_map[geometry_id];


  glDeleteBuffers(1, &geometry.vbo_indices);
  glDeleteBuffers(1, &geometry.vbo_vertices);

  glDeleteVertexArrays(1, &geometry.vao);

  geometry_map.erase(geometry_id);
}

void GPUDriverGL::UpdateCommandList(const CommandList& list) {
  //std::cerr << "UpdateCommandList " << list.size << std::endl;

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
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
      std::cerr << "ERROR: Could not destroy the shaders: " <<
        glErrorString(ErrorCheckValue) << std::endl;
      std::cin.get();
      exit(-1);
    }
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



  ErrorCheckValue = glGetError();
  if (ErrorCheckValue != GL_NO_ERROR) {
    char *log;
    GLint length, result;
    glGetProgramiv(prog.program_id, GL_INFO_LOG_LENGTH, &length);
    log = (char*)malloc(length);
    glGetProgramInfoLog(prog.program_id, length, &result, log);
    std::cerr << "Error in LoadProgram(): Unable to link program: \n\tType: " <<
      type << "\n\tError: " << glErrorString(ErrorCheckValue) << "\n\tLog: " << log
      << std::endl;
    std::cin.get();
    free(log);
    exit(-1);
  }
  programs_[type] = prog;
}

void GPUDriverGL::SelectProgram(ProgramType type) {
  auto i = programs_.find(type);
  if (i != programs_.end()) {
    cur_program_id_ = i->second.program_id;
    glUseProgram(i->second.program_id);
  }
  else {
    std::cout << "ERROR: Missing shader type: " << type << std::endl;
  }
}

void GPUDriverGL::SetUniformDefaults() {
  float params[4] = { glfwGetTime() / 1000.0f, (float)render_buffer_width_, (float)render_buffer_height_, scale_ };
  SetUniform4f("State", params);
}

void GPUDriverGL::SetUniform1ui(const char* name, GLuint val) {
  glUniform1ui(glGetUniformLocation(cur_program_id_, name), val);
}

void GPUDriverGL::SetUniform1f(const char* name, float val) {
  glUniform1f(glGetUniformLocation(cur_program_id_, name), (GLfloat)val);
}

void GPUDriverGL::SetUniform1fv(const char* name, size_t count, const float* val) {
  glUniform1fv(glGetUniformLocation(cur_program_id_, name), count, val);
}

void GPUDriverGL::SetUniform4f(const char* name, const float val[4]) {
  glUniform4f(glGetUniformLocation(cur_program_id_, name),
    (GLfloat)val[0], (GLfloat)val[1], (GLfloat)val[2], (GLfloat)val[3]);
}

void GPUDriverGL::SetUniform4fv(const char* name, size_t count, const float* val) {
  glUniform4fv(glGetUniformLocation(cur_program_id_, name), count, val);
}

void GPUDriverGL::SetUniformMatrix4fv(const char* name, size_t count, const float* val) {
  glUniformMatrix4fv(glGetUniformLocation(cur_program_id_, name), count, false, val);
}

}  // namespace ultralight
