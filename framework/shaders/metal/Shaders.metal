#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

// Include header shared between this Metal shader code and C code executing Metal API commands
#import "ShaderTypes.h"

// Vertex shader outputs and fragment shader inputs
typedef struct
{
    float4 Position [[position]];
    float4 Color;
    float2 TexCoord;
    float4 Data0;
    float4 Data1;
    float4 Data2;
    float4 Data3;
    float4 Data4;
    float4 Data5;
    float4 Data6;
    float2 ObjectCoord;
    float2 ScreenCoord;
} FragmentInput;

// Uniform Accessor Functions
float Time(constant Uniforms& u) { return u.State[0]; }
float ScreenWidth(constant Uniforms& u) { return u.State[1]; }
float ScreenHeight(constant Uniforms& u) { return u.State[2]; }
float ScreenScale(constant Uniforms& u) { return u.State[3]; }
float Scalar(constant Uniforms& u, uint i) { if (i < 4u) return u.Scalar4[0][i]; else return u.Scalar4[1][i - 4u]; }

float2 ScreenToDeviceCoords(constant Uniforms &u, float2 screen_coord)
{
    screen_coord *= 2.0 / float2(ScreenWidth(u), -ScreenHeight(u));
    screen_coord += float2(-1.0, 1.0);
    return screen_coord;
}

// Vertex function
vertex FragmentInput
vertexShader(uint vertexID [[vertex_id]],
             constant Vertex *vertices [[buffer(VertexIndex_Vertices)]],
             constant Uniforms &uniforms [[buffer(VertexIndex_Uniforms)]])
{
    FragmentInput out;
    
    out.ObjectCoord = vertices[vertexID].ObjCoord;
    out.ScreenCoord = (uniforms.Transform * float4(vertices[vertexID].Position.xy, 1.0, 1.0)).xy;
    //out.ScreenCoord = vertices[vertexID].Position.xy;
    out.Position = float4(ScreenToDeviceCoords(uniforms, out.ScreenCoord), 1.0, 1.0);
    out.Color = float4(vertices[vertexID].Color) / 255.0f;
    out.TexCoord = vertices[vertexID].TexCoord;
    out.Data0 = vertices[vertexID].Data0;
    out.Data1 = vertices[vertexID].Data1;
    out.Data2 = vertices[vertexID].Data2;
    out.Data3 = vertices[vertexID].Data3;
    out.Data4 = vertices[vertexID].Data4;
    out.Data5 = vertices[vertexID].Data5;
    out.Data6 = vertices[vertexID].Data6;
    
    return out;
}

constexpr sampler texSampler(mag_filter::linear, min_filter::linear);

uint FillType(thread FragmentInput& input) { return uint(input.Data0.x); }
float4 TileRectUV(constant Uniforms& u) { return u.Vector[0]; }
float2 TileSize(constant Uniforms& u) { return u.Vector[1].zw; }
float2 PatternTransformA(constant Uniforms& u) { return u.Vector[2].xy; }
float2 PatternTransformB(constant Uniforms& u) { return u.Vector[2].zw; }
float2 PatternTransformC(constant Uniforms& u) { return u.Vector[3].xy; }
uint Gradient_NumStops(thread FragmentInput& input) { return uint(input.Data0.y); }
bool Gradient_IsRadial(thread FragmentInput& input) { return bool(input.Data0.z); }
float Gradient_R0(thread FragmentInput& input) { return input.Data1.x; }
float Gradient_R1(thread FragmentInput& input) { return input.Data1.y; }
float2 Gradient_P0(thread FragmentInput& input) { return input.Data1.xy; }
float2 Gradient_P1(thread FragmentInput& input) { return input.Data1.zw; }
float SDFMaxDistance(thread FragmentInput& input) { return input.Data0.y; }

struct GradientStop { float percent; float4 color; };

GradientStop GetGradientStop(thread FragmentInput& input, constant Uniforms& u, uint offset)
{
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
        result.percent = Scalar(u, offset - 4);
        result.color = u.Vector[offset - 4];
    }
    return result;
}

float antialias(float d, float width, float median) {
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
    if (rx.x * ry.x > 0.0 && p.x < corner.x && p.y <= corner.y)
        return sdEllipse(local, float2(rx.x, ry.x)) * SOFTEN_ELLIPSE;
    
    corner = float2(size.x - rx.y, -size.y + ry.y);   // Top-Right
    local = p - corner;
    if (rx.y * ry.y > 0.0 && p.x >= corner.x && p.y <= corner.y)
        return sdEllipse(local, float2(rx.y, ry.y)) * SOFTEN_ELLIPSE;
    
    corner = float2(size.x - rx.z, size.y - ry.z);  // Bottom-Right
    local = p - corner;
    if (rx.z * ry.z > 0.0 && p.x >= corner.x && p.y >= corner.y)
        return sdEllipse(local, float2(rx.z, ry.z)) * SOFTEN_ELLIPSE;
    
    corner = float2(-size.x + rx.w, size.y - ry.w); // Bottom-Left
    local = p - corner;
    if (rx.w * ry.w > 0.0 && p.x < corner.x && p.y > corner.y)
        return sdEllipse(local, float2(rx.w, ry.w)) * SOFTEN_ELLIPSE;
    
    return sdRect(p, size);
}

float4 fillSolid(thread FragmentInput& input) {
    float2 size = input.Data1.xy;
    float2 p = input.TexCoord * size;
    float aa_width = 0.5;
    float alpha_x = min(antialias(p.x, aa_width, 1.0), 1.0 - antialias(p.x, aa_width, size.x - 1));
    float alpha_y = min(antialias(p.y, aa_width, 1.0), 1.0 - antialias(p.y, aa_width, size.y - 1));
    float alpha = min(alpha_x, alpha_y) * input.Color.a;
    return float4(input.Color.rgb * alpha, alpha);
}

float4 fillImage(thread FragmentInput& input, thread texture2d<half>& tex) {
    float4 col = float4(input.Color.rgb * input.Color.a, input.Color.a);
    return float4(tex.sample(texSampler, input.TexCoord)) * col;
}

float2 transformAffine(float2 val, float2 a, float2 b, float2 c) {
    return val.x * a + val.y * b + c;
}

float4 fillPatternImage(thread FragmentInput& input, constant Uniforms& u, thread texture2d<half>& tex) {
    float4 tile_rect_uv = TileRectUV(u);
    float2 tile_size = TileSize(u);
    
    float2 p = input.ObjectCoord;
    
    // Apply the affine matrix
    float2 transformed_coords = transformAffine(p, PatternTransformA(u), PatternTransformB(u), PatternTransformC(u));
    
    // Convert back to uv coordinate space
    transformed_coords /= tile_size;
    
    // Wrap UVs to [0.0, 1.0] so texture repeats properly
    float2 uv = fract(transformed_coords);
    
    // Clip to tile-rect UV
    uv *= tile_rect_uv.zw - tile_rect_uv.xy;
    uv += tile_rect_uv.xy;
    
    return float4(tex.sample(texSampler, uv)) * input.Color;
}

float4 fillPatternGradient(thread FragmentInput& input, constant Uniforms& u) {
    float num_stops = Gradient_NumStops(input);
    bool is_radial = Gradient_IsRadial(input);
    float r0 = Gradient_R0(input);
    float r1 = Gradient_R1(input);
    float2 p0 = Gradient_P0(input);
    float2 p1 = Gradient_P1(input);
    
    if (!is_radial) {
        GradientStop stop0 = GetGradientStop(input, u, 0u);
        GradientStop stop1 = GetGradientStop(input, u, 1u);
        
        float2 V = p1 - p0;
        float t = dot(input.TexCoord - p0, V) / dot(V, V);
        t = saturate(t);
        return mix(stop0.color, stop1.color, t);
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

float samp(thread texture2d<half>& tex, float2 uv, float width, float median) {
    return antialias(tex.sample(texSampler, uv).a, width, median);
}

#define SHARPENING 0.7 // 0 = No sharpening, 0.9 = Max sharpening.
#define SHARPEN_MORE 1

float supersample(thread texture2d<half>& tex, float2 uv) {
    float dist = tex.sample(texSampler, uv).a;
    float width = fwidth(dist) * (1.0 - SHARPENING);
    const float median = 0.5;
    float alpha = antialias(dist, width, median);
    float2 dscale = float2(0.354, 0.354); // half of 1/sqrt2
    float2 duv = (dfdx(uv) + dfdy(uv)) * dscale;
    float4 box = float4(uv - duv, uv + duv);
    
    float asum = samp(tex, box.xy, width, median)
    + samp(tex, box.zw, width, median)
    + samp(tex, box.xw, width, median)
    + samp(tex, box.zy, width, median);
#if SHARPEN_MORE
    alpha = (alpha + 0.4 * asum) * 0.39;
#else
    alpha = (alpha + 0.5 * asum) / 3.0;
#endif
    return alpha;
}

float4 fillSDF(thread FragmentInput& input, thread texture2d<half>& tex) {
    float a = supersample(tex, input.TexCoord);
    return float4(input.Color.rgb * a, a);
}

float samp_stroke(thread texture2d<half>& tex, float2 uv, float w, float m, float max_d) {
    float d = abs(((tex.sample(texSampler, uv).r * 2.0) - 1.0) * max_d);
    return antialias(d, w, m);
}

float4 fillStrokeSDF(thread FragmentInput& input, thread texture2d<half>& tex) {
    float max_distance = SDFMaxDistance(input);
    float2 uv = input.TexCoord;
    float dist = abs(((tex.sample(texSampler, uv).r * 2.0) - 1.0) * max_distance);
    float stroke_width = (max_distance - 2.0) * 0.5;
    float width = fwidth(dist) * (1.0 - SHARPENING);
    float alpha = antialias(dist, width, stroke_width);
    
#if SUPERSAMPLE_SDF
    float2 dscale = float2(0.354, 0.354); // half of 1/sqrt2
    float2 duv = (dfdx(uv) + dfdy(uv)) * dscale;
    float4 box = float4(uv - duv, uv + duv);
    
    float asum = samp_stroke(tex, box.xy, width, stroke_width, max_distance)
    + samp_stroke(tex, box.zw, width, stroke_width, max_distance)
    + samp_stroke(tex, box.xw, width, stroke_width, max_distance)
    + samp_stroke(tex, box.zy, width, stroke_width, max_distance);
    
    alpha = (alpha + 0.5 * asum) / 3.;
#endif
    
    alpha = 1.0 - alpha;
    return float4(input.Color.rgb * alpha, alpha);
}

void Unpack(float4 x, thread float4& a, thread float4& b) {
    const float s = 65536.0;
    a = floor(x / s);
    b = floor(x - a * s);
}

constant float epsilon = 0.75;

float antialias2(float d) {
    return smoothstep(-epsilon, +epsilon, d);
}

// Returns two values:
// [0] = distance of p to line segment.
// [1] = closest t on line segment, clamped to [0, 1]
float2 sdSegment(float2 p, float2 a, float2 b)
{
    float2 pa = p - a, ba = b - a;
    float t = dot(pa, ba) / dot(ba, ba);
    return float2(length(pa - ba * t), t);
}

float testCross(float2 a, float2 b, float2 p) {
    return (b.y - a.y) * (p.x - a.x) - (b.x - a.x) * (p.y - a.y);
}

float sdLine(float2 a, float2 b, float2 p)
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

void drawBorderSide(thread float4& out_Color, float4 color, float2 p, float border_alpha, bool is_horiz,
                    float2 a, float2 b, float2 outer_a, float2 outer_b, thread float& offset_px, float width) {
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

void drawBorder(thread float4& out_Color, float2 p, float border_alpha, float2 outer_size, float2 inner_size, float2 inner_offset, float4 color_top, float4 color_right, float4 color_bottom, float4 color_left, float width) {
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

float sdRoundBox(float2 p, float2 b, float r)
{
    b *= 0.5;
    r = min(min(b.x, b.y), r);
    b = b - r;
    float2 q = abs(p) - b;
    float2 m = float2(min(q.x, q.y), max(q.x, q.y));
    float d = (m.x > 0.0) ? length(q) : m.y;
    return d - r;
}

float4 fillBoxDecorations(thread FragmentInput& input) {
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

float4 fillRoundedRect(thread FragmentInput& input) {
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

//float4 GetCol(float4x4 m, uint i) { return float4(m[0][i], m[1][i], m[2][i], m[3][i]); }

#define VISUALIZE_CLIP 0

void applyClip(thread FragmentInput& input, constant Uniforms& u, thread float4& outColor) {
    for (uint i = 0; i < u.ClipSize; i++) {
        float4x4 data = u.Clip[i];
        float2 origin = data[0].xy;
        float2 size = data[0].zw;
        float4 radii_x, radii_y;
        Unpack(data[1], radii_x, radii_y);
        bool inverse = bool(data[3].z);
        
        float2 p = input.ObjectCoord;
        p = transformAffine(p, data[2].xy, data[2].zw, data[3].xy);
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

// Fragment function
fragment float4 fragmentShader(FragmentInput input [[stage_in]],
                               constant Uniforms &uniforms [[buffer(FragmentIndex_Uniforms)]],
                               texture2d<half> tex [[texture(0)]])
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
        case FillType_Image: outColor = fillImage(input, tex); break;
        case FillType_Pattern_Image: outColor = fillPatternImage(input, uniforms, tex); break;
        case FillType_Pattern_Gradient: outColor = fillPatternGradient(input, uniforms); break;
        case FillType_Fill_SDF: outColor = fillSDF(input, tex); break;
        case FillType_Stroke_SDF: outColor = fillStrokeSDF(input, tex); break;
        case FillType_Box_Decorations: outColor = fillBoxDecorations(input); break;
        case FillType_Rounded_Rect: outColor = fillRoundedRect(input); break;
    }
    
    applyClip(input, uniforms, outColor);
    
    return outColor;
}
