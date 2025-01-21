// https://www.shadertoy.com/view/ltSBzw

#version 400

uniform vec2 res;

uniform mat4 camModelMatrix;
uniform float fovy;
uniform vec2 lensOffset;
uniform float farClip;
uniform float nearClip;
uniform float lds;
uniform float time;

uniform float noiseFogStrength;
uniform float depthFogStrength;

uniform vec4 fogColor;
uniform float fogAmp;
uniform vec3 fogFreq;
uniform vec3 fogOffset;
uniform float fogCenter;
uniform float fogRayStart;
uniform float fogPower;

uniform sampler2DRect depth;
uniform sampler2DRect tex;
in vec2 vTexCoord;
out vec4 outputColor;

#pragma include "./libs/noise3d.frag"
//#pragma include "./libs/random.frag"

float fbm(vec3 p) {
	float t = 0.f;
    for (int i = 0; i < 4; i++) {
        float f = pow(3., float(i));
        t += pow(f, -1.0) * max(cnoise(f * p), 0);
    }
	return t;
}

void main() {
	vec2 uv = vTexCoord.xy / res - 0.5;
    uv.y = - uv.y;

    vec3 ray;
    ray.x = uv.x * (res.x / res.y);
    ray.y = uv.y;
    ray.z = .5 / tan(fovy * 0.5);
	ray /= ray.z;

	vec3 ro = ((camModelMatrix) * vec4(vec3(0), 1)).xyz;
	vec3 rd = (transpose(inverse(camModelMatrix)) * vec4(ray, 0)).xyz;

	float t = 0.0;
	float depth = texture(depth, vTexCoord).a;

	if (depth == 0.) depth = 1.f;
	float d = - depth / lds;

	const int fogStep = 6;
	float depthStep = farClip / float(fogStep);
	vec3 p = ro + depthStep * rd; // initial fog depth
	vec3 o = fogOffset * time;

	int s = 0;
	for (int i = 1; i < fogStep; ++i) {
		float l = depthStep * float(i);
		if (l > abs(d)) break;
		t += fbm(p * fogFreq * lds + o) * fogAmp * smoothstep(0., fogRayStart, distance(ro, p));
		p += depthStep * rd;
		s++;
	}

	float density = pow(fogCenter + t, fogPower);
	density *= noiseFogStrength;
	//density += srand(vTexCoord.xy / res) * 0.02; // Add dither to avoid banding

	vec3 read = texture(tex, vTexCoord).rgb;
	outputColor.rgb = mix(read, fogColor.rgb, depth * depthFogStrength);
	outputColor.rgb = mix(outputColor.rgb, fogColor.rgb, clamp(density * depth, 0.f, 1.f));
	
    outputColor.a = 1.f;

}
