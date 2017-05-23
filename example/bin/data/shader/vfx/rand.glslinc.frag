const float PI = 3.14159265358979323;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float srand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453) * 2.0 - 1.0;
}

float rand(float seed){
    vec2 co = vec2(seed, seed + 100);
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float srand(float seed){
    vec2 co = vec2(seed, seed + 100);
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453) * 2.0 - 1.0;
}

//

vec3 getRandomXYZ(vec2 co){
    vec3 v;
    v.y = srand(co);
    float r = sqrt(1.0 - v.y * v.y);
    float theta = srand(co * 1.11) * PI;

    v.x = cos(theta) * r;
    v.z = sin(theta) * r;

    return v;
}

vec3 getRandomVector(vec2 co){

    vec3 dir = vec3(srand(co+vec2(0.3,-0.3)), srand(co+vec2(-0.2, -0.1)), srand(co+vec2(-0.1, -0.4))) * 2.0 - vec3(1.0);
    float r = rand(co);
    vec3 v = r * normalize(dir);

    return v;

}
