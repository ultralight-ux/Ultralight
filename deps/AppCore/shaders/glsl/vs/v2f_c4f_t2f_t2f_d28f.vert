#version 150

// Program Uniforms
uniform vec4 State;
uniform mat4 Transform;
uniform vec4 Scalar4[2];
uniform vec4 Vector[8];
uniform uint ClipSize;
uniform mat4 Clip[8];

// Uniform Accessor Functions
float Time() { return State[0]; }
float ScreenWidth() { return State[1]; }
float ScreenHeight() { return State[2]; }
float ScreenScale() { return State[3]; }
float Scalar(uint i) { if (i < 4u) return Scalar4[0][i]; else return Scalar4[1][i - 4u]; }

// Vertex Attributes
in vec2 in_Position;
in vec4 in_Color;
in vec2 in_TexCoord;
in vec2 in_ObjCoord;
in vec4 in_Data0;
in vec4 in_Data1;
in vec4 in_Data2;
in vec4 in_Data3;
in vec4 in_Data4;
in vec4 in_Data5;
in vec4 in_Data6;

// Out Params
out vec4 ex_Color;
out vec2 ex_TexCoord;
out vec4 ex_Data0;
out vec4 ex_Data1;
out vec4 ex_Data2;
out vec4 ex_Data3;
out vec4 ex_Data4;
out vec4 ex_Data5;
out vec4 ex_Data6;
out vec2 ex_ObjectCoord;
out vec2 ex_ScreenCoord;

vec2 ScreenToDeviceCoords(vec2 screen_coord)
{
  screen_coord *= 2.0 / vec2(ScreenWidth(), -ScreenHeight());
  screen_coord += vec2(-1.0, 1.0);

  // We compensate for OpenGL's vertically-flipped coordinate system by flipping
  // device coordinates here so all FBOs are rendered correctly without modifying
  // texture coordinates. We do a final flip when drawing overlays to screen by
  // modifying transforms (see GPUDriverGL::DrawGeometry).
  screen_coord *= vec2(1.0, -1.0);

  return screen_coord;
}

void main(void)
{
  ex_ObjectCoord = in_ObjCoord;
  ex_ScreenCoord = (Transform * vec4(in_Position, 1.0, 1.0)).xy;
  gl_Position = vec4(ScreenToDeviceCoords(ex_ScreenCoord), 1.0, 1.0);
  ex_Color = in_Color;
  ex_TexCoord = in_TexCoord;
  ex_Data0 = in_Data0;
  ex_Data1 = in_Data1;
  ex_Data2 = in_Data2;
  ex_Data3 = in_Data3;
  ex_Data4 = in_Data4;
  ex_Data5 = in_Data5;
  ex_Data6 = in_Data6;
}
