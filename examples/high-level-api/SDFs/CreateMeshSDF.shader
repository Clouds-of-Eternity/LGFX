#compute
#version 460

/*
Astral.Shaderc.exe "examples\high-level-api\SDFs\CreateMeshSDF.shader" "examples\high-level-api\SDFs\bin\Debug\CreateMeshSDF.shaderobj"
*/

struct Vertex
{
    vec4 position;
    vec4 normal;
};

layout(std430, binding = 0) readonly buffer Vertices {
    Vertex vertices[ ];
};
layout(std430, binding = 1) readonly buffer Indices {
    uint indices[ ];
};
layout(rgba16f, binding = 2) uniform image3D writeTo;

// layout(r32f, binding = 2) uniform image3D writeTo; 
layout(binding = 3) uniform MeshData {
    uvec4 dimensions;
    vec4 minExtents;
    vec4 maxExtents;
} meshData;

float dot2(vec3 vec)
{
    return dot(vec, vec);
}

vec3 intersectTriangle( in vec3 ro, in vec3 rd, in vec3 v0, in vec3 v1, in vec3 v2 )
{
    vec3 v1v0 = v1 - v0;
    vec3 v2v0 = v2 - v0;
    vec3 rov0 = ro - v0;
    vec3  n = cross( v1v0, v2v0 );
    vec3  q = cross( rov0, rd );
    float d = 1.0/dot( rd, n );
    float u = d*dot( -q, v2v0 );
    float v = d*dot(  q, v1v0 );
    float t = d*dot( -n, rov0 );
    if( u<0.0 || v<0.0 || (u+v)>1.0 ) t = -1.0;
    return vec3( t, u, v );
}
float udTriangle( vec3 p, vec3 a, vec3 b, vec3 c )
{
  vec3 ba = b - a; vec3 pa = p - a;
  vec3 cb = c - b; vec3 pb = p - b;
  vec3 ac = a - c; vec3 pc = p - c;
  vec3 nor = cross( ba, ac );

  return sqrt(
    (sign(dot(cross(ba,nor),pa)) +
     sign(dot(cross(cb,nor),pb)) +
     sign(dot(cross(ac,nor),pc))<2.0)
     ?
     min( min(
     dot2(ba*clamp(dot(ba,pa)/dot2(ba),0.0,1.0)-pa),
     dot2(cb*clamp(dot(cb,pb)/dot2(cb),0.0,1.0)-pb) ),
     dot2(ac*clamp(dot(ac,pc)/dot2(ac),0.0,1.0)-pc) )
     :
     dot(nor,pa)*dot(nor,pa)/dot2(nor) );
}
// quadratic polynomial
float smin( float a, float b, float k )
{
    k *= 4.0;
    float h = max( k-abs(a-b), 0.0 )/k;
    return min(a,b) - h*h*k*(1.0/4.0);
}
vec3 Barycentric(vec3 value1, vec3 value2, vec3 value3, float amount1, float amount2)
{
    return value1 + (value2 - value1) * amount1 + (value3 - value1) * amount2;
}

layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
void main()
{
    uvec3 index = gl_GlobalInvocationID;
    if (index.x < meshData.dimensions.x && index.y < meshData.dimensions.y && index.z < meshData.dimensions.z)
    {
        vec3 p = meshData.minExtents.xyz + (vec3(index) / vec3(meshData.dimensions.xyz)) * (meshData.maxExtents.xyz - meshData.minExtents.xyz);
        vec4 minSDF = vec4(10000.0);
        int intersectionCount = 0;
        uint finalTrangle = 0;
        for (uint i = 0; i < meshData.dimensions.w / 3; i++)
        {
            vec3 v0 = vertices[indices[i * 3 + 0]].position.xyz;
            vec3 v1 = vertices[indices[i * 3 + 1]].position.xyz;
            vec3 v2 = vertices[indices[i * 3 + 2]].position.xyz;

            float nearest = udTriangle(p, v0, v1, v2);
            if (nearest < minSDF.w)
            {
                finalTrangle = i;
                //minSDF.xyz = normalize((n0 + n1 + n2) / 3.0);
                minSDF.w = nearest;
            }
            if (intersectTriangle(p, vec3(0.0, 0.0, 1.0), v0, v1, v2).x > 0.0)
            {
                intersectionCount++;
            }
        }
        vec3 n0 = vertices[indices[finalTrangle * 3 + 0]].normal.xyz;
        vec3 n1 = vertices[indices[finalTrangle * 3 + 1]].normal.xyz;
        vec3 n2 = vertices[indices[finalTrangle * 3 + 2]].normal.xyz;
        minSDF.xyz = normalize((n0 + n1 + n2) / 3.0);  
        if (intersectionCount % 2 == 1)
        {
            minSDF.w *= -1.0;
        }
        imageStore(writeTo, ivec3(index), vec4(normalize(minSDF.xyz), minSDF.w));
    }
}