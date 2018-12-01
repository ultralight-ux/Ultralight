cbuffer Uniforms : register(b0)
{
  float4 State;
  matrix Transform;
  float4 Scalar4[2];
  float4 Vector[8];
  uint ClipSize;
  matrix Clip[8];
};

float Time() { return State[0]; }
float ScreenWidth() { return State[1]; }
float ScreenHeight() { return State[2]; }
float Scalar(int i) { if (i < 4) return Scalar4[0][i]; else return Scalar4[1][i - 4]; }

float2 ScreenToDeviceCoords(float2 screen_coord) {
  screen_coord *= 2.0 / float2(ScreenWidth(), -ScreenHeight());
  screen_coord += float2(-1.0, 1.0);
  return screen_coord;
}

struct VS_OUTPUT
{
  float4 Position    : SV_POSITION;
  float4 Color       : COLOR0;
  float2 ObjectCoord : TEXCOORD0;
};

VS_OUTPUT VS(float2 Position : POSITION,
             uint4  Color    : COLOR0,
             float2 ObjCoord : TEXCOORD0)
{
  VS_OUTPUT output;
  float2 screen_coord = mul(Transform, float4(Position, 1.0, 1.0)).xy;
  output.Position = float4(ScreenToDeviceCoords(screen_coord), 1.0, 1.0);
  output.Color = float4(Color) / 255.0;
  output.ObjectCoord = ObjCoord;
  return output;
}