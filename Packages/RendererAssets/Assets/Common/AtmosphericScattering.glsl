#ifndef ATMOSPHERIC_SCATTERING
#define ATMOSPHERIC_SCATTERING

#include "Math.glsl"

float MiePhaseFunction(float angleCos, float g)
{
	float g2 = g * g;
	float a = (1 + g2 - 2 * g * angleCos);
	return (3 * (1 - g2)) / (2 * (2 + g2)) * (1 + angleCos * angleCos) / sqrt(pow(a, 3.0f));
}

float RayleighPhaseFunction(float cosTheta)
{
	return 3.0f / (16.0f * PI) * (1 + cosTheta * cosTheta);
}

struct AtmosphereProperties
{
	float MieHeight;
	float RayleighHeight;

	float MieCoefficient;
	float MieAbsorbtion;
	float RayleighAbsobtion;
	vec3 OzoneAbsorbtion;
	vec3 RayleighCoefficient;
};

struct ScatteringCoefficients
{
	float Mie;
	vec3 Rayleigh;
	vec3 Extinction;
};

float FindSpehereRayIntersection(vec3 rayOrigin, vec3 rayDirection, float radius)
{
	vec3 d = rayOrigin;
	float p1 = -dot(rayDirection, d);
	float p2sqr = p1 * p1 - dot(d, d) + radius * radius;

	if (p2sqr < 0)
		return -1.0f;

	float p2 = sqrt(p2sqr);
	float t1 = p1 - p2;
	float t2 = p1 + p2;

	return max(t1, t2);
}


ScatteringCoefficients ComputeScatteringCoefficients(float height, in AtmosphereProperties properties)
{
	float rayleighDensity = exp(-height / properties.RayleighHeight);
	float mieDensity = exp(-height / properties.MieHeight);

	ScatteringCoefficients coefficients;
	coefficients.Mie = properties.MieCoefficient * mieDensity;
	coefficients.Rayleigh = properties.RayleighCoefficient * rayleighDensity;

	vec3 ozoneAbsorbtion = properties.OzoneAbsorbtion * max(0.0f, 1.0f - abs(height - 25000) / 15000);
	coefficients.Extinction = coefficients.Rayleigh
		+ vec3(properties.RayleighAbsobtion)
		+ properties.MieAbsorbtion
		+ coefficients.Mie
		+ ozoneAbsorbtion;

	return coefficients;
}

vec3 ComputeSunTransmittance(vec3 rayOrigin,
	int raySteps,
	float planetRadius,
	float atmosphereThickness,
	vec3 lightDirection,
	in AtmosphereProperties properties)
{
	float atmosphereDistance = FindSpehereRayIntersection(rayOrigin, -lightDirection, planetRadius + atmosphereThickness);
	if (atmosphereDistance < 0.0f)
		return vec3(0.0f);

	vec3 opticalDepth = vec3(0.0f);
	float stepLength = atmosphereDistance / max(1, raySteps - 1);
	vec3 rayStep = -lightDirection * stepLength;
	vec3 rayPoint = rayOrigin;

	for (int i = 0; i < raySteps; i++)
	{
		float height = length(rayPoint) - planetRadius;

		ScatteringCoefficients scatteringCoefficients = ComputeScatteringCoefficients(height, properties);
		opticalDepth += scatteringCoefficients.Extinction;
	}

	return exp(-opticalDepth / atmosphereThickness * stepLength);
}

#endif
