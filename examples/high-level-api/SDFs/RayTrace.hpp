#pragma once
#include "Maths/Vec3.hpp"
#include "Maths/Util.hpp"
#include <math.h>

struct Ray
{
    Maths::Vec3 origin;
    Maths::Vec3 dir;
};

struct RayHitResult
{
    bool status;
    Maths::Vec3 hitPosition;
    float length;
};
inline RayHitResult RayTriangle(Ray ray, Maths::Vec3 v0, Maths::Vec3 v1, Maths::Vec3 v2)
{
    // Compute the plane's normal
    Maths::Vec3 v0v1 = v1 - v0;
    Maths::Vec3 v0v2 = v2 - v0;
    // No need to normalize
    Maths::Vec3 N = Maths::Vec3::Cross(v0v1, v0v2); // N
 
    // Step 1: Finding P
    
    // Check if the ray and plane are parallel
    float NdotRayDirection = Maths::Vec3::Dot(N, ray.dir); // N.dotProduct(dir);
    if (fabs(NdotRayDirection) < 0.001f) // Almost 0
    {
        return {}; // They are parallel, so they don't intersect!
    }

    // Compute d parameter using equation 2
    float d = -Maths::Vec3::Dot(N, v0); //-N.dotProduct(v0);

    RayHitResult result = {};
    // Compute t (equation 3)
    result.length = -(Maths::Vec3::Dot(N, ray.origin) + d) / NdotRayDirection;
    
    // Check if the triangle is behind the ray
    if (result.length < 0) return {}; // The triangle is behind
 
    // Compute the intersection point using equation 1
    Maths::Vec3 P = ray.origin + ray.dir * result.length;
 
    // Step 2: Inside-Outside Test
    Maths::Vec3 Ne; // Vector perpendicular to triangle's plane
 
    // Test sidedness of P w.r.t. edge v0v1
    Maths::Vec3 v0p = P - v0;
    Ne = Maths::Vec3::Cross(v0v1, v0p);//v0v1.crossProduct(v0p);
    if (Maths::Vec3::Dot(N, Ne) < 0.0f)
    {
        return {};
    }
    //if (N.dotProduct(Ne) < 0) return false; // P is on the right side
 
    // Test sidedness of P w.r.t. edge v2v1
	Maths::Vec3 v2v1 = v2 - v1;
    Maths::Vec3 v1p = P - v1;
    Ne = Maths::Vec3::Cross(v2v1, v1p);//v2v1.crossProduct(v1p);
    if (Maths::Vec3::Dot(N, Ne) < 0.0f) 
    {
        return {}; // P is on the right side
    }
    // Test sidedness of P w.r.t. edge v2v0
	Maths::Vec3 v2v0 = v0 - v2;
    Maths::Vec3 v2p = P - v2;
    Ne = Maths::Vec3::Cross(v2v0, v2p);
    if (Maths::Vec3::Dot(N, Ne) < 0.0f)
    {
        return {}; // P is on the right side
    };
    result.hitPosition = P;
    result.status = true;

    return result; // The ray hits the triangle
}