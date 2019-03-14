///
/// @file GPUDriver.h
///
/// @brief The header for the GPUDriver interface.
///
/// @author
///
/// This file is a part of Ultralight, a fast, lightweight, HTML UI engine
///
/// Website: <http://ultralig.ht>
///
/// Copyright (C) 2019 Ultralight, Inc. All rights reserved.
///
#pragma once
#pragma warning(disable: 4251)
#include <Ultralight/Defines.h>
#include <Ultralight/Geometry.h>
#include <Ultralight/Matrix.h>
#include <Ultralight/Bitmap.h>

namespace ultralight {

///
/// @note  This pragma pack(push, 1) command is important! Vertex layouts
///	       should not be padded with any bytes.
///
#pragma pack(push, 1)

///
/// RenderBuffer description, @see GPUDriver::CreateRenderBuffer.
///
struct UExport RenderBuffer {
  uint32_t texture_id;
  uint32_t width;
  uint32_t height;
  bool has_stencil_buffer;
  bool has_depth_buffer;
};

///
/// Vertex layout for path vertices, useful for synthesizing or modifying
/// vertex data.
///
struct Vertex_2f_4ub_2f {
  float pos[2];
  unsigned char color[4];
  float obj[2];
};

///
/// Vertex layout for quad vertices, useful for synthesizing or modifying
/// vertex data.
///
struct Vertex_2f_4ub_2f_2f_28f {
  float pos[2];
  unsigned char color[4];
  float tex[2];
  float obj[2];
  float data0[4];
  float data1[4];
  float data2[4];
  float data3[4];
  float data4[4];
  float data5[4];
  float data6[4];
};

///
/// Vertex formats
///
enum UExport VertexBufferFormat {
  kVertexBufferFormat_2f_4ub_2f,
  kVertexBufferFormat_2f_4ub_2f_2f_28f,
};

///
/// Vertex buffer, @see GPUDriver::CreateGeometry
///
struct UExport VertexBuffer {
  VertexBufferFormat format;
  uint32_t size;
  uint8_t* data;
};

///
/// Vertex index type
///
typedef uint32_t IndexType;

///
/// Vertex index buffer, @see GPUDriver::CreateGeometry
///
struct UExport IndexBuffer {
  uint32_t size;
  uint8_t* data;
};

///
/// Shader types, used by GPUState::shader_type
///
enum UExport ShaderType {
  kShaderType_Fill,
  kShaderType_FillPath,
};

///
/// GPU state description.
///
struct UExport GPUState {
  float viewport_width;
  float viewport_height;
  Matrix4x4 transform;
  bool enable_texturing;
  bool enable_blend;
  uint8_t shader_type;
  uint32_t render_buffer_id;
  uint32_t texture_1_id;
  uint32_t texture_2_id;
  uint32_t texture_3_id;
  float uniform_scalar[8];
  vec4 uniform_vector[8];
  uint8_t clip_size;
  Matrix4x4 clip[8];
};

///
/// Command types, used by Command::command_type
///
enum UExport CommandType {
  kCommandType_ClearRenderBuffer,
  kCommandType_DrawGeometry,
};

///
/// Command description, handling one of these should result in either a call to
/// GPUDriver::ClearRenderBuffer or GPUDriver::DrawGeometry.
///
struct UExport Command {
  uint8_t command_type;
  GPUState gpu_state;
  uint32_t geometry_id;
  uint32_t indices_count;
  uint32_t indices_offset;
};

///
/// Command list, @see GPUDriver::UpdateCommandList
///
struct UExport CommandList {
  uint32_t size;
  Command* commands;
};

#pragma pack(pop)

///
/// @brief  GPUDriver interface, dispatches GPU calls to the native driver.
///
/// By default, Ultralight uses an offscreen, OpenGL GPUDriver that draws each
/// View to an offscreen Bitmap (@see View::bitmap). You can override this to
/// provide your own GPUDriver and integrate directly with your own 3D engine.
///
/// This is intended to be implemented by users and defined before creating the
/// Renderer. @see Platform::set_gpu_driver
///
class UExport GPUDriver {
public:
  virtual ~GPUDriver();

  ///
  /// Called before any commands are dispatched during a frame.
  ///
  virtual void BeginSynchronize() = 0;

  ///
  /// Called after any commands are dispatched during a frame.
  ///
  virtual void EndSynchronize() = 0;

  ///
  /// Get the next available texture ID.
  ///
  virtual uint32_t NextTextureId() = 0;

  /// Create a texture with a certain ID and optional bitmap. If the Bitmap is
  /// empty (Bitmap::IsEmpty), then a RTT Texture should be created instead.
  ///
  virtual void CreateTexture(uint32_t texture_id,
                             Ref<Bitmap> bitmap) = 0;

  ///
  /// Update an existing non-RTT texture with new bitmap data.
  ///
  virtual void UpdateTexture(uint32_t texture_id,
                             Ref<Bitmap> bitmap) = 0;

  ///
  /// Bind a texture to a certain texture unit.
  ///
  virtual void BindTexture(uint8_t texture_unit,
                           uint32_t texture_id) = 0;

  ///
  /// Destroy a texture.
  ///
  virtual void DestroyTexture(uint32_t texture_id) = 0;

  ///
  /// Generate the next available render buffer ID.
  ///
  virtual uint32_t NextRenderBufferId() = 0;

  ///
  /// Create a render buffer with certain ID and buffer description.
  ///
  virtual void CreateRenderBuffer(uint32_t render_buffer_id,
                                  const RenderBuffer& buffer) = 0;

  ///
  /// Bind a render buffer
  ///
  virtual void BindRenderBuffer(uint32_t render_buffer_id) = 0;

  ///
  /// Clear a render buffer (flush pixels)
  ///
  virtual void ClearRenderBuffer(uint32_t render_buffer_id) = 0;

  ///
  /// Destroy a render buffer
  ///
  virtual void DestroyRenderBuffer(uint32_t render_buffer_id) = 0;

  ///
  /// Generate the next available geometry ID.
  ///
  virtual uint32_t NextGeometryId() = 0;

  ///
  /// Create geometry with certain ID and vertex/index data.
  ///
  virtual void CreateGeometry(uint32_t geometry_id,
                              const VertexBuffer& vertices,
                              const IndexBuffer& indices) = 0;

  ///
  /// Update existing geometry with new vertex/index data.
  ///
  virtual void UpdateGeometry(uint32_t geometry_id,
                              const VertexBuffer& vertices,
                              const IndexBuffer& indices) = 0;

  ///
  /// Draw geometry using the specific index count/offset and GPUState.
  ///
  virtual void DrawGeometry(uint32_t geometry_id,
                            uint32_t indices_count,
                            uint32_t indices_offset,
                            const GPUState& state) = 0;

  ///
  /// Destroy geometry.
  ///
  virtual void DestroyGeometry(uint32_t geometry_id) = 0;

  ///
  /// Update command list (you should copy the commands to your own structure).
  ///
  virtual void UpdateCommandList(const CommandList& list) = 0;

  ///
  /// Check if any commands need drawing.
  ///
  virtual bool HasCommandsPending() = 0;

  ///
  /// Iterate through stored command list and dispatch to ClearRenderBuffer or
  /// DrawGeometry, respectively. Command list should be cleared at end of call.
  ///
  virtual void DrawCommandList() = 0;
};

}  // namespace ultralight
