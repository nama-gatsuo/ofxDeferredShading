// https://www.shadertoy.com/view/ltSBzw

#version 400

uniform vec2 res;

uniform mat4 modelMatrix;
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

uniform sampler2DRect depth;
uniform sampler2DRect tex;
in vec2 vTexCoord;
out vec4 outputColor;

#pragma include "./libs/noise3d.frag"
//#pragma include "./libs/random.frag"

float fbm(vec3 p) {
	float t = 0.f;
    for (int i = 0; i < 4; i++) {
        float f = pow(2., float(i));
        t += pow(f, -1.0) * cnoise(f * p);
    }
	return t;
}

void main() {
	vec2 uv = vTexCoord.xy / res - 0.5 + vec2(lensOffset.x, - lensOffset.y);
    uv.y = - uv.y;
    vec3 ray;
    ray.x = uv.x * (res.x / res.y);
    ray.y = uv.y;
    ray.z = .5 / tan(fovy * 0.5);

	vec3 rd = normalize(modelMatrix * vec4(ray, 0)).xyz;
    vec3 ro = (inverse(modelMatrix) * vec4(vec3(0), 1)).xyz;

	float t = 0.0;
	float depth = texture(depth, vTexCoord).a;

	if (depth == 0.) depth = 1.f;
	float d = depth / lds;

	const int fogStep = 6;
	vec3 p = ro + rd * fogRayStart; // initial fog depth
	float depthStep = (farClip - fogRayStart) / float(fogStep);
	vec3 o = fogOffset * time;
	int c = 0;
	for (int i = 0; i < fogStep; ++i) {
		if (depthStep * float(i) + fogRayStart > d) break;
		t += fbm(p * fogFreq + o) * fogAmp * float(c);
		p += depthStep * rd;
		c++;
	}

	float density = fogCenter + t;
	density *= noiseFogStrength;
	//density += srand(vTexCoord.xy / res) * 0.02; // Add dither to avoid banding

	vec3 read = texture(tex, vTexCoord).rgb;
	outputColor.rgb = mix(read, fogColor.rgb, depth * depthFogStrength);
	outputColor.rgb = mix(outputColor.rgb, fogColor.rgb, clamp(density * depth, 0.f, 1.f));
	//outputColor.rgb = vec3(float(c)/float(fogStep));
    outputColor.a = 1.f;

}
