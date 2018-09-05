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

// Texture Units
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform sampler2D Texture3;

// Vertex Attributes
in vec4 ex_Color;
in vec2 ex_TexCoord;
in vec2 ex_ObjectCoord;
in vec2 ex_ScreenCoord;
in vec4 ex_Data0;
in vec4 ex_Data1;
in vec4 ex_Data2;
in vec4 ex_Data3;
in vec4 ex_Data4;
in vec4 ex_Data5;
in vec4 ex_Data6;

// Out Params
out vec4 out_Color;

uint FillType() { return uint(ex_Data0.x); }
vec4 TileRectUV() { return Vector[0]; }
vec2 TileSize() { return Vector[1].zw; }
vec2 PatternTransformA() { return Vector[2].xy; }
vec2 PatternTransformB() { return Vector[2].zw; }
vec2 PatternTransformC() { return Vector[3].xy; }
uint Gradient_NumStops() { return uint(ex_Data0.y); }
bool Gradient_IsRadial() { return bool(ex_Data0.z); }
float Gradient_R0() { return ex_Data1.x; }
float Gradient_R1() { return ex_Data1.y; }
vec2 Gradient_P0() { return ex_Data1.xy; }
vec2 Gradient_P1() { return ex_Data1.zw; }
float SDFMaxDistance() { return ex_Data0.y; }

struct GradientStop { float percent; vec4 color; };

GradientStop GetGradientStop(uint offset) {
  GradientStop result;
  if (offset < 4u) {
    result.percent = ex_Data2[offset];
    if (offset == 0u)
      result.color = ex_Data3;
    else if (offset == 1u)
      result.color = ex_Data4;
    else if (offset == 2u)
      result.color = ex_Data5;
    else if (offset == 3u)
      result.color = ex_Data6;
  } else {
    result.percent = Scalar(offset - 4u);
    result.color = Vector[offset - 4u];
  }
  return result;
}

float antialias(in float d, in float width, in float median) {
  return smoothstep(median - width, median + width, d);
}

float sdRect(vec2 p, vec2 size) {
    vec2 d = abs(p) - size;
    return min(max(d.x,d.y),0.0) + length(max(d,0.0));
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

float sdEllipse( vec2 p, in vec2 ab ) {
  if (abs(ab.x - ab.y) < 0.1)
    return length(p) - ab.x;

    p = abs(p); if (p.x > p.y) { p=p.yx; ab=ab.yx; }
    
    float l = ab.y*ab.y - ab.x*ab.x;
    
  float m = ab.x*p.x/l; 
    float n = ab.y*p.y/l; 
    float m2 = m*m;
    float n2 = n*n;
    
  float c = (m2 + n2 - 1.0)/3.0; 
    float c3 = c*c*c;

  float q = c3 + m2*n2*2.0;
  float d = c3 + m2*n2;
  float g = m + m*n2;

  float co;

  if (d < 0.0)
  {
    float p = acos(q/c3)/3.0;
    float s = cos(p);
    float t = sin(p)*sqrt(3.0);
    float rx = sqrt( -c*(s + t + 2.0) + m2 );
    float ry = sqrt( -c*(s - t + 2.0) + m2 );
    co = ( ry + sign(l)*rx + abs(g)/(rx*ry) - m)/2.0;
  } else  {
    float h = 2.0*m*n*sqrt( d );
    float s = sign(q+h)*pow( abs(q+h), 1.0/3.0 );
    float u = sign(q-h)*pow( abs(q-h), 1.0/3.0 );
    float rx = -s - u - c*4.0 + 2.0*m2;
    float ry = (s - u)*sqrt(3.0);
    float rm = sqrt( rx*rx + ry*ry );
    float p = ry/sqrt(rm-rx);
    co = (p + 2.0*g/rm - m)/2.0;
  }

  float si = sqrt(1.0 - co*co);
 
  vec2 r = vec2(ab.x*co, ab.y*si);
    
  return length(r - p) * sign(p.y-r.y);
}

float sdRoundRect(vec2 p, vec2 size, vec4 rx, vec4 ry) {
  size *= 0.5;
  vec2 corner;

  corner = vec2(-size.x+rx.x, -size.y+ry.x);  // Top-Left
  vec2 local = p - corner;
  if (dot(rx.x, ry.x) > 0.0 && p.x < corner.x && p.y <= corner.y)
    return sdEllipse(local, vec2(rx.x, ry.x));

  corner = vec2(size.x-rx.y, -size.y+ry.y);   // Top-Right
  local = p - corner;
  if (dot(rx.y, ry.y) > 0.0 && p.x >= corner.x && p.y <= corner.y)
    return sdEllipse(local, vec2(rx.y, ry.y));

  corner = vec2(size.x-rx.z, size.y-ry.z);  // Bottom-Right
  local = p - corner;
  if (dot(rx.z, ry.z) > 0.0 && p.x >= corner.x && p.y >= corner.y)
    return sdEllipse(local, vec2(rx.z, ry.z));

  corner = vec2(-size.x+rx.w, size.y-ry.w); // Bottom-Left
  local = p - corner;
  if (dot(rx.w, ry.w) > 0.0 && p.x < corner.x && p.y > corner.y) 
    return sdEllipse(local, vec2(rx.w, ry.w));

  return sdRect(p, size);
}

void fillSolid() {
  vec2 size = ex_Data1.xy;
  vec2 p = ex_TexCoord * size;
  float aa_width = 0.5;
  float alpha_x = min(antialias(p.x, aa_width, 1.0), 1.0 - antialias(p.x, aa_width, size.x - 1));
  float alpha_y = min(antialias(p.y, aa_width, 1.0), 1.0 - antialias(p.y, aa_width, size.y - 1));
  float alpha = min(alpha_x, alpha_y) * ex_Color.a;
  out_Color = vec4(ex_Color.rgb * alpha, alpha);
  //out_Color = ex_Color;
  //out_Color = vec4(1.0, 0.0, 0.0, 1.0);
}

void fillImage(vec2 uv) {
  vec4 col = vec4(ex_Color.rgb * ex_Color.a, ex_Color.a);
  out_Color = texture(Texture1, uv) * col;
}

vec2 transformAffine(vec2 val, vec2 a, vec2 b, vec2 c) {
  return val.x * a + val.y * b + c;
}

void fillPatternImage() {
  vec4 tile_rect_uv = TileRectUV();
  vec2 tile_size = TileSize();

  vec2 p = ex_ObjectCoord;

  // Apply the affine matrix
  vec2 transformed_coords = transformAffine(p,
    PatternTransformA(), PatternTransformB(), PatternTransformC());

  // Convert back to uv coordinate space
  transformed_coords /= tile_size;

  // Wrap UVs to [0.0, 1.0] so texture repeats properly
  vec2 uv = fract(transformed_coords);

  // Clip to tile-rect UV
  uv *= tile_rect_uv.zw - tile_rect_uv.xy;
  uv += tile_rect_uv.xy;

  fillImage(uv);
}

void fillPatternGradient() {
  float num_stops = Gradient_NumStops();
  bool is_radial = Gradient_IsRadial();
  float r0 = Gradient_R0();
  float r1 = Gradient_R1();
  vec2 p0 = Gradient_P0();
  vec2 p1 = Gradient_P1();

  if (!is_radial) {
    GradientStop stop0 = GetGradientStop(0u);
    GradientStop stop1 = GetGradientStop(1u);

    vec2 V = p1 - p0;
    float t = dot(ex_TexCoord - p0, V) / dot(V, V);
    t = clamp(t, 0.0, 1.0);
    out_Color = mix(stop0.color, stop1.color, t);
  }
}

float stroke(float d, float s, float a) {
  if (d <= s)
    return 1.0;
  return 1.0 - smoothstep(s, s + a, d);
}

float samp(in vec2 uv, float width, float median) {
  return antialias(texture(Texture1, uv).r, width, median);
}

#define SHARPENING 0.7 // 0 = No sharpening, 0.9 = Max sharpening.
#define SHARPEN_MORE 1
#define SUPERSAMPLE_SDF 1

float supersample(in vec2 uv) {
  float dist = texture(Texture1, uv).a;
  float width = fwidth(dist) * (1.0 - SHARPENING);
  const float median = 0.5;
  float alpha = antialias(dist, width, median);
  vec2 dscale = vec2(0.354, 0.354); // half of 1/sqrt2
  vec2 duv = (dFdx(uv) + dFdy(uv)) * dscale;
  vec4 box = vec4(uv - duv, uv + duv);

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

void fillSDF() {
  float a = supersample(ex_TexCoord);
  out_Color = vec4(ex_Color.rgb * a, a);
}

float samp_stroke(in vec2 uv, float w, float m, float max_d) {
  float d = abs(((texture(Texture1, uv).r  * 2.0) - 1.0) * max_d);
  return antialias(d, w, m);
}

void fillStrokeSDF() {
  float max_distance = SDFMaxDistance();
  vec2 uv = ex_TexCoord;
  float dist = abs(((texture(Texture1, uv).r * 2.0) - 1.0) * max_distance);
  float stroke_width = (max_distance - 2.0) * 0.5;
  float width = fwidth(dist) * (1.0 - SHARPENING);
  float alpha = antialias(dist, width, stroke_width);

#if SUPERSAMPLE_SDF
  float dscale = 0.354; // half of 1/sqrt2; you can play with this
  vec2 duv = dscale * (dFdx(uv) + dFdy(uv));
  vec4 box = vec4(uv - duv, uv + duv);

  float asum = samp_stroke(box.xy, width, stroke_width, max_distance)
    + samp_stroke(box.zw, width, stroke_width, max_distance)
    + samp_stroke(box.xw, width, stroke_width, max_distance)
    + samp_stroke(box.zy, width, stroke_width, max_distance);

  alpha = (alpha + 0.5 * asum) / 3.;
#endif

  alpha = 1.0 - alpha;
  out_Color = vec4(ex_Color.rgb * alpha, alpha);
}

void Unpack(vec4 x, out vec4 a, out vec4 b) {
  const float s = 65536.0;
  a = floor(x / s);
  b = floor(x - a * s);
}

const float epsilon = 0.75;

float antialias2 (float d) {
  return smoothstep (-epsilon, +epsilon, d);
}

// Returns two values:
// [0] = distance of p to line segment.
// [1] = closest t on line segment, clamped to [0, 1]
vec2 sdSegment(in vec2 p, in vec2 a, in vec2 b)
{
  vec2 pa = p - a, ba = b - a;
  float t = dot(pa, ba) / dot(ba, ba);
  return vec2(length(pa - ba * t), t);
}

float testCross(vec2 a, vec2 b, vec2 p) {
  return (b.y - a.y) * (p.x - a.x) - (b.x - a.x) * (p.y - a.y);
}

float sdLine(in vec2 a, in vec2 b, in vec2 p)
{
  vec2 pa = p - a, ba = b - a;
  float t = dot(pa, ba) / dot(ba, ba);
  return length(pa - ba*t) * sign(testCross(a, b, p));
}

vec4 blend(vec4 src, vec4 dest) {
  vec4 result;
  result.rgb = src.rgb + dest.rgb * (1.0 - src.a);
  result.a = src.a + dest.a * (1.0 - src.a);
  return result;
}

float minBorderLine(float d_border, float d_line, float width) {
  return (d_border < width) ? d_line : ((d_line > 0.0) ? 1e9 : d_border);
}

void drawBorderSide(inout vec4 out_Color, vec4 color, vec2 p, float border_alpha, bool is_horiz,
                    vec2 a, vec2 b, vec2 outer_a, vec2 outer_b, inout float offset_px, float width) {
  vec2 seg = sdSegment(p, a, b);
  float len = distance(a, b);

  vec2 middle_a = is_horiz ? vec2(a.x, 0.0) : vec2(0.0, a.y);
  vec2 middle_b = is_horiz ? vec2(b.x, 0.0) : vec2(0.0, b.y);

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
  vec4 border = vec4(color.rgb * alpha, alpha);
  out_Color = blend(border, out_Color);

  offset_px += len;
}

void drawBorder(inout vec4 out_Color, vec2 p, float border_alpha, vec2 outer_size, vec2 inner_size, vec2 inner_offset,
                vec4 color_top, vec4 color_right, vec4 color_bottom, vec4 color_left, float width) {
  outer_size *= 0.5;
  inner_size *= 0.5;
  
  // We shrink the inner size just a tiny bit so that we can calculate
  // the trapezoid even when the border width for a side is zero.
  inner_size.x -= (outer_size.x - inner_size.x) < 1.0 ? 0.5 : 0.0;
  inner_size.y -= (outer_size.y - inner_size.y) < 1.0 ? 0.5 : 0.0;

  vec2 outerTopLeft = vec2(-outer_size.x, -outer_size.y);
  vec2 outerTopRight = vec2(outer_size.x, -outer_size.y);
  vec2 outerBottomRight = vec2(outer_size.x, outer_size.y);
  vec2 outerBottomLeft = vec2(-outer_size.x, outer_size.y);

  vec2 innerTopLeft = vec2(-inner_size.x, -inner_size.y) + inner_offset;
  vec2 innerTopRight = vec2(inner_size.x, -inner_size.y) + inner_offset;
  vec2 innerBottomRight = vec2(inner_size.x, inner_size.y) + inner_offset;
  vec2 innerBottomLeft = vec2(-inner_size.x, inner_size.y) + inner_offset;

  float offset_px = 0.0f;

  // Top, Right, Bottom, Left
  drawBorderSide(out_Color, color_top, p, border_alpha, true, innerTopLeft, innerTopRight, outerTopLeft, outerTopRight, offset_px, width);
  drawBorderSide(out_Color, color_right, p, border_alpha, false, innerTopRight, innerBottomRight, outerTopRight, outerBottomRight, offset_px, width);
  drawBorderSide(out_Color, color_bottom, p, border_alpha, true, innerBottomRight, innerBottomLeft, outerBottomRight, outerBottomLeft, offset_px, width);
  drawBorderSide(out_Color, color_left, p, border_alpha, false, innerBottomLeft, innerTopLeft, outerBottomLeft, outerTopLeft, offset_px, width);
}

float sdRoundBox(in vec2 p, in vec2 b, in float r)
{
  b *= 0.5;
  r = min(min(b.x, b.y), r);
  b = b - r;
  vec2 q = abs(p) - b;
  vec2 m = vec2(min(q.x, q.y), max(q.x, q.y));
  float d = (m.x > 0.0) ? length(q) : m.y;
  return d - r;
}

void fillBoxDecorations() {
  vec2 outer_size = ex_Data0.zw;
  vec2 inner_offset = ex_Data1.xy;
  vec2 inner_size = ex_Data1.zw;

  vec4 outer_radii_x, outer_radii_y;
  Unpack(ex_Data2, outer_radii_x, outer_radii_y);

  vec4 inner_radii_x, inner_radii_y;
  Unpack(ex_Data3, inner_radii_x, inner_radii_y);

  vec4 color_top, color_right;
  Unpack(ex_Data4, color_top, color_right);
  color_top /= 255.0f;
  color_right /= 255.0f;

  vec4 color_bottom, color_left;
  Unpack(ex_Data5, color_bottom, color_left);
  color_bottom /= 255.0f;
  color_left /= 255.0f;

  float width = 0.3;

  vec2 p = ex_TexCoord * outer_size - (outer_size * 0.5);

  float d_outer = sdRoundRect(p, outer_size, outer_radii_x, outer_radii_y);
  float d_inner = sdRoundRect(p - inner_offset, inner_size, inner_radii_x, inner_radii_y);

  float outer_alpha = antialias(-d_outer, width, 0.0);
  float inner_alpha = antialias(-d_inner, width, 0.0);
  float border_alpha = min(outer_alpha, 1.0 - inner_alpha);

  // Draw background fill
  float alpha = antialias(-d_outer, width, 0.0) * ex_Color.a;
  out_Color = vec4(ex_Color.rgb * alpha, alpha);

  // Draw border
  drawBorder(out_Color, p, border_alpha, outer_size, inner_size, inner_offset, color_top, color_right, color_bottom, color_left, 0.5);
}

float innerStroke(float stroke_width, float d) {
  return min(antialias(-d, 0.5, 0.0), 1.0 - antialias(-d, 0.5, stroke_width));
}

void fillRoundedRect() {
  vec2 p = ex_TexCoord;
  vec2 size = ex_Data0.zw;
  p = (p - 0.5) * size;
  float d = sdRoundRect(p, size, ex_Data1, ex_Data2);

  // Fill background
  float alpha = antialias(-d, 0.5, 0.0) * ex_Color.a;
  out_Color = vec4(ex_Color.rgb * alpha, alpha);

  // Draw stroke
  float stroke_width = ex_Data3.x;
  vec4 stroke_color = ex_Data4;

  if (stroke_width > 0.0) {
    alpha = innerStroke(stroke_width, d);
    alpha *= stroke_color.a;
    vec4 stroke = vec4(stroke_color.rgb * alpha, alpha);
    out_Color = blend(stroke, out_Color);
  }
}

void applyClip() {
  for (uint i = 0u; i < ClipSize; i++) {
    mat4 data = Clip[i];
    vec2 origin = data[0].xy;
    vec2 size = data[0].zw;
    vec4 radii_x, radii_y;
    Unpack(data[1], radii_x, radii_y);
    bool inverse = bool(data[3].z);
    
    vec2 p = ex_ObjectCoord;
    p = transformAffine(p, data[2].xy, data[2].zw, data[3].xy);
    p -= origin;
        
    float d_clip = sdRoundRect(p, size, radii_x, radii_y) * (inverse? -1.0 : 1.0);
    float alpha = antialias2(-d_clip);
    out_Color = vec4(out_Color.rgb * alpha, out_Color.a * alpha);
    
    //if (abs(d_clip) < 2.0)
    // out_Color = vec4(0.9, 1.0, 0.0, 1.0);
  }
}

void main(void) {
  const uint FillType_Solid = 0u;
  const uint FillType_Image = 1u;
  const uint FillType_Pattern_Image = 2u;
  const uint FillType_Pattern_Gradient = 3u;
  const uint FillType_Fill_SDF = 4u;
  const uint FillType_Stroke_SDF = 5u;
  const uint FillType_Box_Decorations = 6u;
  const uint FillType_Rounded_Rect = 7u;

  switch (FillType())
  {
  case FillType_Solid: fillSolid(); break;
  case FillType_Image: fillImage(ex_TexCoord); break;
  case FillType_Pattern_Image: fillPatternImage(); break;
  case FillType_Pattern_Gradient: fillPatternGradient(); break;
  case FillType_Fill_SDF: fillSDF(); break;
  case FillType_Stroke_SDF: fillStrokeSDF(); break;
  case FillType_Box_Decorations: fillBoxDecorations(); break;
  case FillType_Rounded_Rect: fillRoundedRect(); break;
  }

  applyClip();
}
