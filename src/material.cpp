#include "common.h"
#include "random.h"
#include "shape.h"
#include "texture.h"
#include "material.h"

static Vector reflect(const Vector& v, const Vector& normal) {
    return v - 2.0f*dot_product(v, normal)*normal;
}

static bool refract(const Vector& v, const Vector& normal, float niOverNt, Vector& refracted) {
    Vector uv = v.normalized();
    float dt = dot_product(uv, normal);

    float discriminant = 1.0f - niOverNt*niOverNt*(1.0f - dt*dt);
    if (discriminant <= 0.0f)
        return false;

    refracted = niOverNt*(uv - normal*dt) - normal*sqrt(discriminant);
    return true;
}

static float schlick(float cosine, float refractionIndex) {
    float r0 = (1 - refractionIndex) / (1 + refractionIndex);
    r0 = r0*r0;
    return r0 + (1 - r0)*std::pow(1 - cosine, 5);
}

bool Lambertian::scatter(RNG& rng, const Ray& ray, const Intersection& hit, Scatter_Info& scatter_info) const {
    scatter_info.is_specular = false;
    scatter_info.attenuation = albedo->value(hit.u, hit.v, hit.p);
    scatter_info.pdf = new Cosine_Pdf(hit.normal);
    return true;
}

float Lambertian::scattering_pdf(const Ray& ray_in, const Intersection& isect, const Ray& scattered_ray) const {
    float cosine = std::max(0.f, dot_product(isect.normal, scattered_ray.direction));
    return cosine / PI;
}

bool Metal::scatter(RNG& rng, const Ray& ray, const Intersection& hit, Scatter_Info& scatter_info) const {
    Vector reflected = reflect(ray.direction, hit.normal);
    scatter_info.specular_ray = Ray(hit.p, reflected + fuzz * random_point_in_unit_sphere(rng));
    scatter_info.attenuation = albedo;
    scatter_info.is_specular = true;
    scatter_info.pdf = nullptr;
    return true;
}

//bool Dielectric::scatter(RNG& rng, const Ray& ray, const Intersection& hitRecord, Vector& attenuation, Ray& scatteredRay) const {
//    Vector outwardNormal;
//    Vector reflected = reflect(ray.direction, hitRecord.normal);
//
//    float n;
//    attenuation = Vector(1.0, 1.0, 1.0);
//    Vector refracted;
//    float reflectProb;
//    float cosine;
//
//    if (dot_product(ray.direction, hitRecord.normal) > 0.0f) {
//        outwardNormal = -hitRecord.normal;
//        n = refraction_index;
//        cosine = dot_product(ray.direction, hitRecord.normal);
//        cosine = sqrt(1 - refraction_index * refraction_index * (1 - cosine * cosine));
//    } else {
//        outwardNormal = hitRecord.normal;
//        n = 1.0f / refraction_index;
//        cosine = -dot_product(ray.direction, hitRecord.normal);
//    }
//
//    if (refract(ray.direction, outwardNormal, n, refracted)) {
//        reflectProb = schlick(cosine, refraction_index);
//    } else {
//        reflectProb = 1.0f;
//    }
//
//    if (rng.random_float() < reflectProb) {
//        scatteredRay = Ray(hitRecord.p, reflected, ray.time);
//    } else {
//        scatteredRay = Ray(hitRecord.p, refracted, ray.time);
//    }
//
//    return true;
//}

Vector Diffuse_Light::emitted(const Ray& ray_in, const Intersection& isect, float u, float v, const Vector& p) const {
    if (dot_product(ray_in.direction, isect.normal) < 0.f)
        return emit->value(u, v, p);
    else
        return Vector(0);
}

//bool Isotropic::scatter(RNG& rng, const Ray& ray, const Intersection& hit, Vector& attenuation, Ray& scattered_ray) const {
//    scattered_ray = Ray(hit.p, random_point_in_unit_sphere(rng));
//    attenuation = albedo->value(hit.u, hit.v, hit.p);
//    return true;
//}
