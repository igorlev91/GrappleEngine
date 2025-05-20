Properties =
{
	u_Sky.PlanetRadius = {}
	u_Sky.AtmosphereThickness = {}
	u_Sky.RaySteps = {}
	u_Sky.ViewRaySteps = {}

	u_Sky.ObserverHeight = {}

	u_Sky.MieHeight = {}
	u_Sky.RayleighHeight = {}
	u_Sky.RayleighCoefficient = {}
	u_Sky.MieCoefficient = {}
	u_Sky.MieAbsorbtion = {}
	u_Sky.RayleighAbsobtion = {}
	u_Sky.OzoneAbsorbtion = {}

	u_Sky.GroundColor = { Type = Color }
}

#begin vertex
#version 450

#include "Packages/RendererAssets/Assets/Common/Camera.glsl"

layout(location = 0) in vec2 i_Position;

void main()
{
	gl_Position = vec4(i_Position, 0.9999999f, 1.0f);
}

#end

#begin pixel
#version 450

#include "Packages/RendererAssets/Assets/Common/BRDF.glsl"
#include "Packages/RendererAssets/Assets/Common/Light.glsl"
#include "Packages/RendererAssets/Assets/Common/Camera.glsl"

layout(std140, push_constant) uniform Sky
{
	float PlanetRadius;
	float AtmosphereThickness;
	int RaySteps;
	int ViewRaySteps;

	float ObserverHeight;

	float MieHeight;
	float RayleighHeight;

	float MieCoefficient;
	float MieAbsorbtion;
	float RayleighAbsobtion;
	vec3 OzoneAbsorbtion;
	vec3 RayleighCoefficient;

	vec3 GroundColor;

} u_Sky;

layout(location = 0) out vec4 o_Color;

float MiePhaseFunction(float angleCos, float g)
{
	float g2 = g * g;
	float a = (1 + g2 - 2 * g * angleCos);
	return (3 * (1 - g2)) / (2 * (2 + g2)) * (1 + angleCos * angleCos) / sqrt(pow(a, 3.0f));
}

float RayleighPhaseFunction(float cosTheta)
{
	return 3.0f / (16.0f * pi) * (1 + cosTheta * cosTheta);
}

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

vec3 CalculateViewDirection()
{
	vec2 uv = vec2(gl_FragCoord.xy) / vec2(u_Camera.ViewportSize);
	vec4 projected = u_Camera.InverseViewProjection * vec4(uv * 2.0f - vec2(1.0f), 1.0f, 1.0f);
	projected.xyz /= projected.w;
	return normalize(projected.xyz - u_Camera.Position);
}

struct ScatteringCoefficients
{
	float Mie;
	vec3 Rayleigh;
	vec3 Extinction;
};

ScatteringCoefficients ComputeScatteringCoefficients(float height)
{
	float rayleighDensity = exp(-height / u_Sky.RayleighHeight);
	float mieDensity = exp(-height / u_Sky.MieHeight);

	ScatteringCoefficients coefficients;
	coefficients.Mie = u_Sky.MieCoefficient * mieDensity;
	coefficients.Rayleigh = u_Sky.RayleighCoefficient * rayleighDensity;

	vec3 ozoneAbsorbtion = u_Sky.OzoneAbsorbtion * max(0.0f, 1.0f - abs(height - 25000) / 15000);
	coefficients.Extinction = coefficients.Rayleigh + vec3(u_Sky.RayleighAbsobtion) + u_Sky.MieAbsorbtion + coefficients.Mie + ozoneAbsorbtion;

	return coefficients;
}

vec3 ComputeSunTransmittance(vec3 rayOrigin)
{
	float atmosphereDistance = FindSpehereRayIntersection(rayOrigin, -u_LightDirection, u_Sky.PlanetRadius + u_Sky.AtmosphereThickness);
	if (atmosphereDistance < 0.0f)
		return vec3(0.0f);

	vec3 opticalDepth = vec3(0.0f);
	float stepLength = atmosphereDistance / max(1, u_Sky.RaySteps - 1);
	vec3 rayStep = -u_LightDirection * stepLength;
	vec3 rayPoint = rayOrigin;

	for (int i = 0; i < u_Sky.RaySteps; i++)
	{
		float height = length(rayPoint) - u_Sky.PlanetRadius;

		ScatteringCoefficients scatteringCoefficients = ComputeScatteringCoefficients(height);

		opticalDepth += scatteringCoefficients.Extinction;
	}

	return exp(-opticalDepth / u_Sky.AtmosphereThickness * stepLength);
}

void main()
{
	vec3 viewDirection = CalculateViewDirection();
	vec3 viewRayPoint = vec3(0.0f, u_Sky.PlanetRadius + u_Sky.ObserverHeight, 0.0f);

	float groudDistance = FindSpehereRayIntersection(viewRayPoint, viewDirection, u_Sky.PlanetRadius);
	if (groudDistance > 0.0f)
	{
		o_Color = vec4(u_Sky.GroundColor, 1.0f);
		return;
	}

	float distanceThroughAtmosphere = FindSpehereRayIntersection(viewRayPoint,
		viewDirection, u_Sky.PlanetRadius + u_Sky.AtmosphereThickness);

	if (distanceThroughAtmosphere < 0.0f)
		discard;

	vec3 viewRayStep = viewDirection * distanceThroughAtmosphere / max(1, u_Sky.ViewRaySteps - 1);
	float viewRayStepLength = length(viewRayStep);
	float scaledViewRayStepLength = viewRayStepLength / u_Sky.AtmosphereThickness;

	float rayleighPhase = RayleighPhaseFunction(-dot(viewDirection, -u_LightDirection));
	float miePhase = MiePhaseFunction(dot(viewDirection, -u_LightDirection), 0.84f);

	vec3 luminance = vec3(0.0f);
	vec3 transmittance = vec3(1.0f);
	for (int i = 0; i < u_Sky.ViewRaySteps; i++)
	{
		float height = length(viewRayPoint) - u_Sky.PlanetRadius;

		ScatteringCoefficients scatteringCoefficients = ComputeScatteringCoefficients(height);
		vec3 sampleTransmittance = exp(-scatteringCoefficients.Extinction * scaledViewRayStepLength);
		vec3 sunTransmittance = ComputeSunTransmittance(viewRayPoint);

		vec3 rayleighInScatter = scatteringCoefficients.Rayleigh * rayleighPhase * sunTransmittance;
		vec3 mieInScatter = scatteringCoefficients.Mie * miePhase * sunTransmittance;
		vec3 inScatter = rayleighInScatter + mieInScatter;

		vec3 scatteringIntegral = (inScatter - inScatter * sampleTransmittance) / scatteringCoefficients.Extinction;
		luminance += scatteringIntegral * transmittance;
		transmittance *= sampleTransmittance;

		viewRayPoint += viewRayStep;
	}

	o_Color = vec4(u_LightColor.w * luminance * 10, 1.0f);
}

#end
