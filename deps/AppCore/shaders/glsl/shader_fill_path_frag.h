#include <string>

static std::string shader_fill_path_frag() {
  return R"(#version 150

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
in vec4 ex_Color;
in vec2 ex_ObjectCoord;
in vec2 ex_ScreenCoord;

// Out Params
out vec4 out_Color;

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

vec2 transformAffine(vec2 val, vec2 a, vec2 b, vec2 c) {
  return val.x * a + val.y * b + c;
}

void Unpack(vec4 x, out vec4 a, out vec4 b) {
  const float s = 65536.0;
  a = floor(x / s);
  b = floor(x - a * s);
}

#define AA_WIDTH 0.354

float antialias(in float d, in float width, in float median) {
  return smoothstep(median - width, median + width, d);
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
    float alpha = antialias(-d_clip, AA_WIDTH, -AA_WIDTH);
    out_Color = vec4(out_Color.rgb * alpha, out_Color.a * alpha);
    
    //if (abs(d_clip) < 2.0)
    // out_Color = vec4(0.9, 1.0, 0.0, 1.0);
  }
}

void main(void) {
  out_Color = ex_Color;
  applyClip();
}

)";
}
