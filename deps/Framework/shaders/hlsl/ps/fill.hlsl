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
float ScreenScale() { return State[3]; }
float Scalar(int i) { if (i < 4) return Scalar4[0][i]; else return Scalar4[1][i - 4]; }

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct VS_OUTPUT
{
  float4 Position    : SV_POSITION;
  float4 Color       : COLOR0;
  float2 TexCoord    : TEXCOORD0;
  float4 Data0       : COLOR1;
  float4 Data1       : COLOR2;
  float4 Data2       : COLOR3;
  float4 Data3       : COLOR4;
  float4 Data4       : COLOR5;
  float4 Data5       : COLOR6;
  float4 Data6       : COLOR7;
  float2 ObjectCoord : TEXCOORD1;
  float2 ScreenCoord : TEXCOORD2;
};

float OutColor;

uint FillType(VS_OUTPUT input) { return uint(input.Data0.x); }
float4 TileRectUV() { return Vector[0]; }
float2 TileSize() { return Vector[1].zw; }
float2 PatternTransformA() { return Vector[2].xy; }
float2 PatternTransformB() { return Vector[2].zw; }
float2 PatternTransformC() { return Vector[3].xy; }
uint Gradient_NumStops(VS_OUTPUT input) { return uint(input.Data0.y); }
bool Gradient_IsRadial(VS_OUTPUT input) { return bool(input.Data0.z); }
float Gradient_R0(VS_OUTPUT input) { return input.Data1.x; }
float Gradient_R1(VS_OUTPUT input) { return input.Data1.y; }
float2 Gradient_P0(VS_OUTPUT input) { return input.Data1.xy; }
float2 Gradient_P1(VS_OUTPUT input) { return input.Data1.zw; }
float SDFMaxDistance(VS_OUTPUT input) { return input.Data0.y; }

struct GradientStop { float percent; float4 color; };

GradientStop GetGradientStop(VS_OUTPUT input, uint offset) {
  GradientStop result;
  if (offset < 4) {
    result.percent = input.Data2[offset];
	if (offset == 0)
	  result.color = input.Data3;
	else if (offset == 1)
	  result.color = input.Data4;
	else if (offset == 2)
	  result.color = input.Data5;
	else if (offset == 3)
	  result.color = input.Data6;
  } else {
    result.percent = Scalar(offset - 4);
    result.color = Vector[offset - 4];
  }
  return result;
}

float antialias(in float d, in float width, in float median) {
  return smoothstep(median - width, median + width, d);
}

float sdRect(float2 p, float2 size) {
  float2 d = abs(p) - size;
  return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

// The below function "sdEllipse" is MIT licensed with following text:
//
// The MIT License
// Copyright 2013 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software
// is furnished to do so, subject to the following conditions: The above copyright
// notice and this permission notice shall be included in all copies or substantial
// portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
// EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

float sdEllipse(float2 p, float2 ab) {
  if (abs(ab.x - ab.y) < 0.1)
    return length(p) - ab.x;

  p = abs(p); if (p.x > p.y) { p = p.yx; ab = ab.yx; }

  float l = ab.y*ab.y - ab.x*ab.x;

  float m = ab.x*p.x / l;
  float n = ab.y*p.y / l;
  float m2 = m*m;
  float n2 = n*n;

  float c = (m2 + n2 - 1.0) / 3.0;
  float c3 = c*c*c;

  float q = c3 + m2*n2*2.0;
  float d = c3 + m2*n2;
  float g = m + m*n2;

  float co;

  if (d < 0.0) {
    float p = acos(q / c3) / 3.0;
    float s = cos(p);
    float t = sin(p)*sqrt(3.0);
    float rx = sqrt(-c*(s + t + 2.0) + m2);
    float ry = sqrt(-c*(s - t + 2.0) + m2);
    co = (ry + sign(l)*rx + abs(g) / (rx*ry) - m) / 2.0;
  } else {
    float h = 2.0*m*n*sqrt(d);
    float s = sign(q + h)*pow(abs(q + h), 1.0 / 3.0);
    float u = sign(q - h)*pow(abs(q - h), 1.0 / 3.0);
    float rx = -s - u - c*4.0 + 2.0*m2;
    float ry = (s - u)*sqrt(3.0);
    float rm = sqrt(rx*rx + ry*ry);
    float p = ry / sqrt(rm - rx);
    co = (p + 2.0*g / rm - m) / 2.0;
  }

  float si = sqrt(1.0 - co*co);

  float2 r = float2(ab.x*co, ab.y*si);

  return length(r - p) * sign(p.y - r.y);
}

// 1.0 = No softening, 0.1 = Max softening
#define SOFTEN_ELLIPSE 1.0

float sdRoundRect(float2 p, float2 size, float4 rx, float4 ry) {
  size *= 0.5;
  float2 corner;

  corner = float2(-size.x + rx.x, -size.y + ry.x);  // Top-Left
  float2 local = p - corner;
  if (dot(rx.x, ry.x) > 0.0 && p.x < corner.x && p.y <= corner.y)
    return sdEllipse(local, float2(rx.x, ry.x)) * SOFTEN_ELLIPSE;

  corner = float2(size.x - rx.y, -size.y + ry.y);   // Top-Right
  local = p - corner;
  if (dot(rx.y, ry.y) > 0.0 && p.x >= corner.x && p.y <= corner.y)
    return sdEllipse(local, float2(rx.y, ry.y)) * SOFTEN_ELLIPSE;

  corner = float2(size.x - rx.z, size.y - ry.z);  // Bottom-Right
  local = p - corner;
  if (dot(rx.z, ry.z) > 0.0 && p.x >= corner.x && p.y >= corner.y)
    return sdEllipse(local, float2(rx.z, ry.z)) * SOFTEN_ELLIPSE;

  corner = float2(-size.x + rx.w, size.y - ry.w); // Bottom-Left
  local = p - corner;
  if (dot(rx.w, ry.w) > 0.0 && p.x < corner.x && p.y > corner.y)
    return sdEllipse(local, float2(rx.w, ry.w)) * SOFTEN_ELLIPSE;

  return sdRect(p, size);
}

float4 fillSolid(VS_OUTPUT input) {
  float2 size = input.Data1.xy;
  float2 p = input.TexCoord * size;
  float aa_width = 0.5;
  float alpha_x = min(antialias(p.x, aa_width, 1.0), 1.0 - antialias(p.x, aa_width, size.x - 1));
  float alpha_y = min(antialias(p.y, aa_width, 1.0), 1.0 - antialias(p.y, aa_width, size.y - 1));
  float alpha = min(alpha_x, alpha_y) * input.Color.a;
  return float4(input.Color.rgb * alpha, alpha);
}

float4 fillImage(VS_OUTPUT input) {
  float4 col = float4(input.Color.rgb * input.Color.a, input.Color.a);
  return texture0.Sample(sampler0, input.TexCoord) * col;
}

float2 transformAffine(float2 val, float2 a, float2 b, float2 c) {
  return val.x * a + val.y * b + c;
}

float4 fillPatternImage(VS_OUTPUT input) {
  float4 tile_rect_uv = TileRectUV();
  float2 tile_size = TileSize();

  float2 p = input.ObjectCoord;

  // Apply the affine matrix
  float2 transformed_coords = transformAffine(p,
    PatternTransformA(), PatternTransformB(), PatternTransformC());

  // Convert back to uv coordinate space
  transformed_coords /= tile_size;

  // Wrap UVs to [0.0, 1.0] so texture repeats properly
  float2 uv = frac(transformed_coords);

  // Clip to tile-rect UV
  uv *= tile_rect_uv.zw - tile_rect_uv.xy;
  uv += tile_rect_uv.xy;

  return texture0.Sample(sampler0, uv) * input.Color;
}

float4 fillPatternGradient(VS_OUTPUT input) {
  float num_stops = Gradient_NumStops(input);
  bool is_radial = Gradient_IsRadial(input);
  float r0 = Gradient_R0(input);
  float r1 = Gradient_R1(input);
  float2 p0 = Gradient_P0(input);
  float2 p1 = Gradient_P1(input);

  if (!is_radial) {
    GradientStop stop0 = GetGradientStop(input, 0u);
    GradientStop stop1 = GetGradientStop(input, 1u);

    float2 V = p1 - p0;
    float t = dot(input.TexCoord - p0, V) / dot(V, V);
    t = saturate(t);
    return lerp(stop0.color, stop1.color, t);
  } else {
    // TODO: Handle radial gradients
    return input.Color;
  }
}

float stroke(float d, float s, float a) {
  if (d <= s)
    return 1.0;
  return 1.0 - smoothstep(s, s + a, d);
}

float samp(in float2 uv, float width, float median) {
  return antialias(texture0.Sample(sampler0, uv).a, width, median);
}

#define SHARPENING 0.7 // 0 = No sharpening, 0.9 = Max sharpening.
#define SHARPEN_MORE 1

float supersample(in float2 uv) {
  float dist = texture0.Sample(sampler0, uv).a;
  float width = fwidth(dist) * (1.0 - SHARPENING);
  const float median = 0.5;
  float alpha = antialias(dist, width, median);
  float2 dscale = float2(0.354, 0.354); // half of 1/sqrt2
  float2 duv = (ddx(uv) + ddy(uv)) * dscale;
  float4 box = float4(uv - duv, uv + duv);

  float asum = samp(box.xy, width, median)
             + samp(box.zw, width, median)
             + samp(box.xw, width, median)
             + samp(box.zy, width, median);
#if SHARPEN_MORE
  alpha = (alpha + 0.4 * asum) * 0.39;
#else
  alpha = (alpha + 0.5 * asum) / 3.0;
#endif
  return alpha;
}

float4 fillSDF(VS_OUTPUT input) {
  float a = supersample(input.TexCoord);
  return float4(input.Color.rgb * a, a);
}

float samp_stroke(in float2 uv, float w, float m, float max_d) {
  float d = abs(((texture0.Sample(sampler0, uv).r * 2.0) - 1.0) * max_d);
  return antialias(d, w, m);
}

float4 fillStrokeSDF(VS_OUTPUT input) {
  float max_distance = SDFMaxDistance(input);
  float2 uv = input.TexCoord;
  float dist = abs(((texture0.Sample(sampler0, uv).r * 2.0) - 1.0) * max_distance);
  float stroke_width = (max_distance - 2.0) * 0.5;
  float width = fwidth(dist) * (1.0 - SHARPENING);
  float alpha = antialias(dist, width, stroke_width);

#if SUPERSAMPLE_SDF
  float2 dscale = float2(0.354, 0.354); // half of 1/sqrt2
  float2 duv = (ddx(uv) + ddy(uv)) * dscale;
  float4 box = float4(uv - duv, uv + duv);

  float asum = samp_stroke(box.xy, width, stroke_width, max_distance)
    + samp_stroke(box.zw, width, stroke_width, max_distance)
    + samp_stroke(box.xw, width, stroke_width, max_distance)
    + samp_stroke(box.zy, width, stroke_width, max_distance);

  alpha = (alpha + 0.5 * asum) / 3.;
#endif

  alpha = 1.0 - alpha;
  return float4(input.Color.rgb * alpha, alpha);
}

void Unpack(float4 x, out float4 a, out float4 b) {
  const float s = 65536.0;
  a = floor(x / s);
  b = floor(x - a * s);
}

float epsilon = 0.75;

float antialias2(float d) {
  return smoothstep(-epsilon, +epsilon, d);
}

// Returns two values:
// [0] = distance of p to line segment.
// [1] = closest t on line segment, clamped to [0, 1]
float2 sdSegment(in float2 p, in float2 a, in float2 b)
{
  float2 pa = p - a, ba = b - a;
  float t = dot(pa, ba) / dot(ba, ba);
  return float2(length(pa - ba * t), t);
}

float testCross(float2 a, float2 b, float2 p) {
  return (b.y - a.y) * (p.x - a.x) - (b.x - a.x) * (p.y - a.y);
}

float sdLine(in float2 a, in float2 b, in float2 p)
{
  float2 pa = p - a, ba = b - a;
  float t = dot(pa, ba) / dot(ba, ba);
  return length(pa - ba*t) * sign(testCross(a, b, p));
}


float4 blend(float4 src, float4 dest) {
  float4 result;
  result.rgb = src.rgb + dest.rgb * (1.0 - src.a);
  result.a = src.a + dest.a * (1.0 - src.a);
  return result;
}

float minBorderLine(float d_border, float d_line, float width) {
  return (d_border < width) ? d_line : ((d_line > 0.0) ? 1e9 : d_border);
}

void drawBorderSide(inout float4 out_Color, float4 color, float2 p, float border_alpha, bool is_horiz,
                    float2 a, float2 b, float2 outer_a, float2 outer_b, inout float offset_px, float width) {
  float2 seg = sdSegment(p, a, b);
  float len = distance(a, b);

  float2 middle_a = is_horiz ? float2(a.x, 0.0) : float2(0.0, a.y);
  float2 middle_b = is_horiz ? float2(b.x, 0.0) : float2(0.0, b.y);

  // We already have an SDF representing the total interior border SDF (border alpha).
  // Let's clip it by 3 lines to obtain the (trapezoid) SDF for this side.
  float line_alpha;
  line_alpha = antialias(-sdLine(a, outer_a, p), width, 0.0);
  border_alpha = min(border_alpha, line_alpha);
  line_alpha = antialias(-sdLine(outer_b, b, p), width, 0.0);
  border_alpha = min(border_alpha, line_alpha);
  line_alpha = antialias(-sdLine(middle_b, middle_a, p), width, 0.0);
  border_alpha = min(border_alpha, line_alpha);

  float alpha = border_alpha * color.a;
  float4 border = float4(color.rgb * alpha, alpha);
  out_Color = blend(border, out_Color);

  offset_px += len;
}

void drawBorder(inout float4 out_Color, float2 p, float border_alpha, float2 outer_size, float2 inner_size, float2 inner_offset,
                float4 color_top, float4 color_right, float4 color_bottom, float4 color_left, float width) {
  outer_size *= 0.5;
  inner_size *= 0.5;
  
  // We shrink the inner size just a tiny bit so that we can calculate
  // the trapezoid even when the border width for a side is zero.
  inner_size.x -= (outer_size.x - inner_size.x) < 1.0 ? 0.5 : 0.0;
  inner_size.y -= (outer_size.y - inner_size.y) < 1.0 ? 0.5 : 0.0;

  float2 outerTopLeft = float2(-outer_size.x, -outer_size.y);
  float2 outerTopRight = float2(outer_size.x, -outer_size.y);
  float2 outerBottomRight = float2(outer_size.x, outer_size.y);
  float2 outerBottomLeft = float2(-outer_size.x, outer_size.y);

  float2 innerTopLeft = float2(-inner_size.x, -inner_size.y) + inner_offset;
  float2 innerTopRight = float2(inner_size.x, -inner_size.y) + inner_offset;
  float2 innerBottomRight = float2(inner_size.x, inner_size.y) + inner_offset;
  float2 innerBottomLeft = float2(-inner_size.x, inner_size.y) + inner_offset;

  float offset_px = 0.0f;

  // Top, Right, Bottom, Left
  drawBorderSide(out_Color, color_top, p, border_alpha, true, innerTopLeft, innerTopRight, outerTopLeft, outerTopRight, offset_px, width);
  drawBorderSide(out_Color, color_right, p, border_alpha, false, innerTopRight, innerBottomRight, outerTopRight, outerBottomRight, offset_px, width);
  drawBorderSide(out_Color, color_bottom, p, border_alpha, true, innerBottomRight, innerBottomLeft, outerBottomRight, outerBottomLeft, offset_px, width);
  drawBorderSide(out_Color, color_left, p, border_alpha, false, innerBottomLeft, innerTopLeft, outerBottomLeft, outerTopLeft, offset_px, width);
}

float sdRoundBox(in float2 p, in float2 b, in float r)
{
  b *= 0.5;
  r = min(min(b.x, b.y), r);
  b = b - r;
  float2 q = abs(p) - b;
  float2 m = float2(min(q.x, q.y), max(q.x, q.y));
  float d = (m.x > 0.0) ? length(q) : m.y;
  return d - r;
}

float4 fillBoxDecorations(VS_OUTPUT input) {
  float2 outer_size = input.Data0.zw;
  float2 inner_offset = input.Data1.xy;
  float2 inner_size = input.Data1.zw;

  float4 outer_radii_x, outer_radii_y;
  Unpack(input.Data2, outer_radii_x, outer_radii_y);

  float4 inner_radii_x, inner_radii_y;
  Unpack(input.Data3, inner_radii_x, inner_radii_y);

  float4 color_top, color_right;
  Unpack(input.Data4, color_top, color_right);
  color_top /= 255.0f;
  color_right /= 255.0f;

  float4 color_bottom, color_left;
  Unpack(input.Data5, color_bottom, color_left);
  color_bottom /= 255.0f;
  color_left /= 255.0f;

  float width = 0.3;

  float2 p = input.TexCoord * outer_size - (outer_size * 0.5);

  float d_outer = sdRoundRect(p, outer_size, outer_radii_x, outer_radii_y);
  float d_inner = sdRoundRect(p - inner_offset, inner_size, inner_radii_x, inner_radii_y);

  float outer_alpha = antialias(-d_outer, width, 0.0);
  float inner_alpha = antialias(-d_inner, width, 0.0);
  float border_alpha = min(outer_alpha, 1.0 - inner_alpha);

  // Draw background fill
  float alpha = antialias(-d_outer, width, 0.0) * input.Color.a;
  float4 outColor = float4(input.Color.rgb * alpha, alpha);

  // Draw border
  drawBorder(outColor, p, border_alpha, outer_size, inner_size, inner_offset, color_top, color_right, color_bottom, color_left, 0.5);

  return outColor;
}

float innerStroke(float stroke_width, float d) {
  return min(antialias(-d, 0.5, 0.0), 1.0 - antialias(-d, 0.5, stroke_width));
}

float4 fillRoundedRect(VS_OUTPUT input) {
  float2 p = input.TexCoord;
  float2 size = input.Data0.zw;
  p = (p - 0.5) * size;
  float d = sdRoundRect(p, size, input.Data1, input.Data2);

  // Fill background
  float alpha = antialias(-d, 0.5, 0.0) * input.Color.a;
  float4 outColor = float4(input.Color.rgb * alpha, alpha);

  // Draw stroke
  float stroke_width = input.Data3.x;
  float4 stroke_color = input.Data4;

  if (stroke_width > 0.0) {
    alpha = innerStroke(stroke_width, d);
    alpha *= stroke_color.a;
    float4 stroke = float4(stroke_color.rgb * alpha, alpha);
    outColor = blend(stroke, outColor);
  }

  return outColor;
}

float4 GetCol(in matrix m, uint i) { return float4(m[0][i], m[1][i], m[2][i], m[3][i]); }

#define VISUALIZE_CLIP 0

void applyClip(VS_OUTPUT input, inout float4 outColor) {
  for (uint i = 0; i < ClipSize; i++) {
    matrix data = Clip[i];
    float2 origin = GetCol(data, 0).xy;
    float2 size = GetCol(data, 0).zw;
    float4 radii_x, radii_y;
    Unpack(GetCol(data, 1), radii_x, radii_y);
    bool inverse = bool(GetCol(data, 3).z);

    float2 p = input.ObjectCoord;
    p = transformAffine(p, GetCol(data, 2).xy, GetCol(data, 2).zw, GetCol(data, 3).xy);
    p -= origin;
    float d_clip = sdRoundRect(p, size, radii_x, radii_y) * (inverse ? -1.0 : 1.0);

#if VISUALIZE_CLIP
    if (abs(d_clip) < 3.0)
      outColor = float4(0.9, 1.0, 0.0, 1.0);
#else
    float alpha = antialias2(-d_clip);
    outColor = float4(outColor.rgb * alpha, outColor.a * alpha);
#endif
  }
}

float4 PS(VS_OUTPUT input) : SV_Target
{
  const uint FillType_Solid = 0u;
  const uint FillType_Image = 1u;
  const uint FillType_Pattern_Image = 2u;
  const uint FillType_Pattern_Gradient = 3u;
  const uint FillType_Fill_SDF = 4u;
  const uint FillType_Stroke_SDF = 5u;
  const uint FillType_Box_Decorations = 6u;
  const uint FillType_Rounded_Rect = 7u;

  float4 outColor = input.Color;

  switch (FillType(input))
  {
  case FillType_Solid: outColor = fillSolid(input); break;
  case FillType_Image: outColor = fillImage(input); break;
  case FillType_Pattern_Image: outColor = fillPatternImage(input); break;
  case FillType_Pattern_Gradient: outColor = fillPatternGradient(input); break;
  case FillType_Fill_SDF: outColor = fillSDF(input); break;
  case FillType_Stroke_SDF: outColor = fillStrokeSDF(input); break;
  case FillType_Box_Decorations: outColor = fillBoxDecorations(input); break;
  case FillType_Rounded_Rect: outColor = fillRoundedRect(input); break;
  }

  applyClip(input, outColor);

  return outColor;
}
