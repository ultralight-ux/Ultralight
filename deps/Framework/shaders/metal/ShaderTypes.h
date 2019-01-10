#ifndef ShaderTypes_h
#define ShaderTypes_h

#include <simd/simd.h>

typedef enum VertexIndex
{
    VertexIndex_Vertices = 0,
    VertexIndex_Uniforms = 1,
} VertexInputIndex;

typedef enum FragmentIndex
{
    FragmentIndex_Uniforms = 0,
} FragmentIndex;

typedef struct
{
    simd::float4 State;
    simd::float4x4 Transform;
    simd::float4 Scalar4[2];
    simd::float4 Vector[8];
    unsigned int ClipSize;
    simd::float4x4 Clip[8];
} Uniforms;

#pragma pack(push, 1)
typedef struct
{
    simd::float2 Position;
    simd::uchar4 Color;
    simd::float2 TexCoord;
    simd::float2 ObjCoord;
    simd::float4 Data0;
    simd::float4 Data1;
    simd::float4 Data2;
    simd::float4 Data3;
    simd::float4 Data4;
    simd::float4 Data5;
    simd::float4 Data6;
} Vertex;

typedef struct
{
    simd::float2 Position;
    simd::uchar4 Color;
    simd::float2 ObjCoord;
} PathVertex;
#pragma pack(pop)


#endif /* ShaderTypes_h */
