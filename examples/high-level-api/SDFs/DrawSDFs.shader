#vertex
#version 450

/*
Astral.Shaderc.exe "examples\high-level-api\SDFs\DrawSDFs.shader" "examples\high-level-api\SDFs\bin\Debug\DrawSDFs.shaderobj"
*/
layout(binding = 0) uniform ShaderGlobalData
{
    mat4 projection;
    mat4 view;
    mat4 projectionInverse;
    mat4 viewInverse;
    vec4 minExtents;
    vec4 maxExtents;
} globalData;

layout(location = 0) out vec2 fragTexCoord;

void main() 
{
    vec2 vertices[4] = vec2[4](
        vec2(-1.0, -1.0),
        vec2(1.0, -1.0),
        vec2(1.0, 1.0),
        vec2(-1.0, 1.0)
    );
    gl_Position = vec4(vertices[gl_VertexIndex], 0.0, 1.0);
    fragTexCoord = (vertices[gl_VertexIndex] + vec2(1.0)) * 0.5;
}

#fragment
#version 450
#line 32


layout(binding = 0) uniform ShaderGlobalData
{
    mat4 projection;
    mat4 view;
    mat4 projectionInverse;
    mat4 viewInverse;
    vec4 minExtents;
    vec4 maxExtents;
} globalData;
layout(binding = 1) uniform texture3D meshDistanceField;
layout(binding = 2) uniform sampler samplerState;
layout(binding = 3) uniform texture2D albedoMap;
layout(binding = 4) uniform texture2D heightMap;
layout(binding = 5) uniform sampler textureSamplerState;
// layout(binding = 2) uniform texture2D heightMap;
// layout(binding = 3) uniform texture3D meshDistanceField;
// layout(binding = 4) uniform sampler samplerState;

struct RayHit
{
    bool status;
    float hitDistance;
    float hitDistanceFar;
    vec3 hitPosition;
    vec3 hitNormal;
};

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

const vec3 lightdir = normalize(vec3(0.0, 1.0, 1.0));

vec3 GetRayDir(vec2 UV)
{
    vec2 pixelNDH = UV * 2.0 - 1.0;

    vec4 dir = globalData.viewInverse * globalData.projectionInverse * vec4(pixelNDH.x, pixelNDH.y, 1.0, 1.0);
    return normalize(dir.xyz);
}

float Merge(float a, float b, float radius){
    radius *= 4.0;
    float h = max( radius-abs(a-b), 0.0 )/radius;
    return min(a,b) - h*h*radius*(1.0/4.0);
}

vec4 triplanar( in texture2D tex, in vec3 p, in vec3 n, in float k, float res)
{
    //p -= globalData.minExtents.xyz;
    p *= 0.15;///= (globalData.maxExtents.xyz - globalData.minExtents.xyz) * 2.0;
    // project+fetch
    vec4 x = texture( sampler2D(tex, textureSamplerState), p.yz * res);
    vec4 y = texture( sampler2D(tex, textureSamplerState), p.zx * res);
    vec4 z = texture( sampler2D(tex, textureSamplerState), p.xy * res);
    
    // blend weights
    vec3 w = pow( abs(n), vec3(k) );
    // blend and return
    return (x*w.x + y*w.y + z*w.z) / (w.x + w.y + w.z);
}

//ray tracing
RayHit boxIntersect(in vec3 ro, in vec3 rd, vec3 boxSize)
{
    vec3 halfSize = boxSize;// * 2.0;
    if (ro.x > -halfSize.x && ro.y > -halfSize.y && ro.z > -halfSize.z && ro.x < halfSize.x && ro.y < halfSize.y && ro.z < halfSize.z)
    {
        return RayHit(true, 0.0, length(boxSize), ro, rd);
    }
    vec3 m = 1.0/rd; // can precompute if traversing a set of aligned boxes
    vec3 n = m*ro;   // can precompute if traversing a set of aligned boxes
    vec3 k = abs(m)*boxSize;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max( max( t1.x, t1.y ), t1.z );
    float tF = min( min( t2.x, t2.y ), t2.z );
    if( tN>tF || tF<0.0) return RayHit(false, -1.0, -1.0, vec3(0.0), vec3(0.0)); // no intersection
    vec3 outNormal = (tN>0.0) ? step(vec3(tN),t1) : // ro ouside the box
                           step(t2,vec3(tF));  // ro inside the box
    outNormal *= -sign(rd);
    return RayHit(true, tN, tF, ro + rd * tN, outNormal);//vec2( tN, tF );
}
RayHit planeIntersect(in vec3 ro, in vec3 rd, vec4 plane)
{
    RayHit result;
    result.hitDistance = -(dot(ro,plane.xyz)+plane.w)/dot(rd,plane.xyz);
    result.hitPosition = ro + rd * result.hitDistance;
    result.hitNormal = plane.xyz;
    result.hitDistanceFar = result.hitDistance;
    return result;
}

//ray marching
float sphSD( vec3 p, vec3 ce, float ra )
{
  return length(p - ce) - ra;
}

vec4 SDF(vec3 p)
{
    vec3 textureSize = (globalData.maxExtents.xyz - globalData.minExtents.xyz);
    vec3 UV = (p - globalData.minExtents.xyz) / (textureSize);

    vec4 res = texture(sampler3D(meshDistanceField, samplerState), UV, 0.0);

    float extrude = triplanar(heightMap, p, res.xyz, 8.0, 1.0).r;
    res.w -= abs(extrude * 0.3);

    return res;
}
vec3 SDFNormal( in vec3 p ) // for function f(p)
{
    const float h = 0.02; // replace by an appropriate value
    const vec2 k = vec2(1,-1);
    return normalize( k.xyy*SDF( p + k.xyy*h ).w + 
                      k.yyx*SDF( p + k.yyx*h ).w + 
                      k.yxy*SDF( p + k.yxy*h ).w + 
                      k.xxx*SDF( p + k.xxx*h ).w );
}

RayHit RayTrace(vec3 ro, vec3 rd, out bool castShadow)
{
    //castShadow = false;
    //const float skin = 0.2;
    vec3 center = (globalData.maxExtents.xyz + globalData.minExtents.xyz) * 0.5;
    RayHit hit = boxIntersect(ro - center, rd, (globalData.maxExtents.xyz - globalData.minExtents.xyz) * 0.5);

    if (hit.status)
    {
        //sphere trace
        int iterations = 60;//int(ceil((hit.hitDistanceFar - hit.hitDistance) / 0.05));
        vec3 p = hit.hitPosition + center;
        float totalDist = hit.hitDistance;
        for (int i = 0; i < iterations; i++)
        {
            vec4 trace = SDF(p);
            if (trace.w < 0.005)
            {
                return RayHit(true, totalDist, totalDist, p, SDFNormal(p));
            }
            //cap to prevent overshooting past potential boundary
            float advanceMax = trace.w;
            p += advanceMax * rd;
            totalDist += advanceMax;

            if (totalDist > hit.hitDistanceFar)
            {
                break;
            }
        }
    }
    
    // hit = planeIntersect(ro, rd, vec4(0.0, 1.0, 0.0, 1.0));
    // if (hit.hitDistance >= 0.0)
    // {
    //     castShadow = true;
    //     return hit;
    // }

    return RayHit(false, -1.0, 0.0, vec3(0.0), vec3(0.0));
}

void main()
{
    // premultiplied alpha
    vec3 color;
    vec2 UV = fragTexCoord;//(gl_FragCoord.xy - vec2(1920.0 * 0.5, 1080.0 * 0.5)) / vec2(1080.0);
    vec4 rayOrigin = globalData.viewInverse * vec4(0.0, 0.0, 0.0, 1.0);
    rayOrigin.xyz /= rayOrigin.w;
    vec3 rayDir = GetRayDir(UV);

    vec3 bgCol = mix(vec3(1.0, 1.0, 1.0), vec3(0.2, 0.8, 1.0), (rayDir.y + 1.0) * 0.5);

    bool castShadow = false;
    RayHit hit = RayTrace(rayOrigin.xyz, GetRayDir(UV), castShadow);
    if (hit.status)
    {

        vec3 normal = hit.hitNormal;

        //color = (normal + 1.0) * 0.5;

        vec3 viewDir = globalData.viewInverse[2].xyz;
        vec3 halfwayDir = reflect(-lightdir, normal);//normalize(lightdir + viewDir);
        float lightAmt = max(dot(normal, lightdir), 0.0);
        //float specular = pow(max(dot(viewDir, halfwayDir), 0.0), 128.0);
        color = vec3(lightAmt);// + specular);
        color *= triplanar(albedoMap, hit.hitPosition, normal, 8.0, 1.0).xyz * 2.0;
        // shadow feelers
        // if (castShadow)
        // {
        //     vec3 shadowDir = lightdir;
        //     shadowDir.z *= -1.0;
        //     bool discardBool;
        //     hit = RayTrace(hit.hitPosition + shadowDir * 0.01, shadowDir, discardBool);
        //     if (hit.hitDistance > 0.0)
        //     {
        //         color *= 0.25;
        //     }
        // }
    }
    else color = bgCol;

    outColor = vec4(color, 1.0);
}