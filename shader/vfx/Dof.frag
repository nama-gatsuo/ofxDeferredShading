/* modified vesion of http://pasteall.org/10779 */
#version 400

uniform sampler2DRect tex; // read fbo
uniform sampler2DRect normalAndDepthTex;

uniform float aperture;
uniform float focusDist;
uniform float maxBlur;

uniform vec2 size;

in vec2 vTexCoord;
out vec4 outputColor;

void main() {

    vec2 aspectcorrect = size;

    float depth = texture(normalAndDepthTex, vTexCoord).a;
    float factor = depth - focusDist;

    vec2 dofblur = vec2(clamp(factor * aperture, -maxBlur, maxBlur));

    vec2 dofblur9 = dofblur * 0.9;
                vec2 dofblur7 = dofblur * 0.7;
                vec2 dofblur4 = dofblur * 0.4;

                vec4 col = vec4( 0.0 );

                col += texture( tex, vTexCoord );
                col += texture( tex, vTexCoord + ( vec2(  0.0,   0.4  ) * aspectcorrect ) * dofblur );
                col += texture( tex, vTexCoord + ( vec2(  0.15,  0.37 ) * aspectcorrect ) * dofblur );
                col += texture( tex, vTexCoord + ( vec2(  0.29,  0.29 ) * aspectcorrect ) * dofblur );
                col += texture( tex, vTexCoord + ( vec2( -0.37,  0.15 ) * aspectcorrect ) * dofblur );
                col += texture( tex, vTexCoord + ( vec2(  0.40,  0.0  ) * aspectcorrect ) * dofblur );
                col += texture( tex, vTexCoord + ( vec2(  0.37, -0.15 ) * aspectcorrect ) * dofblur );
                col += texture( tex, vTexCoord + ( vec2(  0.29, -0.29 ) * aspectcorrect ) * dofblur );
                col += texture( tex, vTexCoord + ( vec2( -0.15, -0.37 ) * aspectcorrect ) * dofblur );
                col += texture( tex, vTexCoord + ( vec2(  0.0,  -0.4  ) * aspectcorrect ) * dofblur );
                col += texture( tex, vTexCoord + ( vec2( -0.15,  0.37 ) * aspectcorrect ) * dofblur );
                col += texture( tex, vTexCoord + ( vec2( -0.29,  0.29 ) * aspectcorrect ) * dofblur );
                col += texture( tex, vTexCoord + ( vec2(  0.37,  0.15 ) * aspectcorrect ) * dofblur );
                col += texture( tex, vTexCoord + ( vec2( -0.4,   0.0  ) * aspectcorrect ) * dofblur );
                col += texture( tex, vTexCoord + ( vec2( -0.37, -0.15 ) * aspectcorrect ) * dofblur );
                col += texture( tex, vTexCoord + ( vec2( -0.29, -0.29 ) * aspectcorrect ) * dofblur );
                col += texture( tex, vTexCoord + ( vec2(  0.15, -0.37 ) * aspectcorrect ) * dofblur );

                col += texture( tex, vTexCoord + ( vec2(  0.15,  0.37 ) * aspectcorrect ) * dofblur9 );
                col += texture( tex, vTexCoord + ( vec2( -0.37,  0.15 ) * aspectcorrect ) * dofblur9 );
                col += texture( tex, vTexCoord + ( vec2(  0.37, -0.15 ) * aspectcorrect ) * dofblur9 );
                col += texture( tex, vTexCoord + ( vec2( -0.15, -0.37 ) * aspectcorrect ) * dofblur9 );
                col += texture( tex, vTexCoord + ( vec2( -0.15,  0.37 ) * aspectcorrect ) * dofblur9 );
                col += texture( tex, vTexCoord + ( vec2(  0.37,  0.15 ) * aspectcorrect ) * dofblur9 );
                col += texture( tex, vTexCoord + ( vec2( -0.37, -0.15 ) * aspectcorrect ) * dofblur9 );
                col += texture( tex, vTexCoord + ( vec2(  0.15, -0.37 ) * aspectcorrect ) * dofblur9 );

                col += texture( tex, vTexCoord + ( vec2(  0.29,  0.29 ) * aspectcorrect ) * dofblur7 );
                col += texture( tex, vTexCoord + ( vec2(  0.40,  0.0  ) * aspectcorrect ) * dofblur7 );
                col += texture( tex, vTexCoord + ( vec2(  0.29, -0.29 ) * aspectcorrect ) * dofblur7 );
                col += texture( tex, vTexCoord + ( vec2(  0.0,  -0.4  ) * aspectcorrect ) * dofblur7 );
                col += texture( tex, vTexCoord + ( vec2( -0.29,  0.29 ) * aspectcorrect ) * dofblur7 );
                col += texture( tex, vTexCoord + ( vec2( -0.4,   0.0  ) * aspectcorrect ) * dofblur7 );
                col += texture( tex, vTexCoord + ( vec2( -0.29, -0.29 ) * aspectcorrect ) * dofblur7 );
                col += texture( tex, vTexCoord + ( vec2(  0.0,   0.4  ) * aspectcorrect ) * dofblur7 );

                col += texture( tex, vTexCoord + ( vec2(  0.29,  0.29 ) * aspectcorrect ) * dofblur4 );
                col += texture( tex, vTexCoord + ( vec2(  0.4,   0.0  ) * aspectcorrect ) * dofblur4 );
                col += texture( tex, vTexCoord + ( vec2(  0.29, -0.29 ) * aspectcorrect ) * dofblur4 );
                col += texture( tex, vTexCoord + ( vec2(  0.0,  -0.4  ) * aspectcorrect ) * dofblur4 );
                col += texture( tex, vTexCoord + ( vec2( -0.29,  0.29 ) * aspectcorrect ) * dofblur4 );
                col += texture( tex, vTexCoord + ( vec2( -0.4,   0.0  ) * aspectcorrect ) * dofblur4 );
                col += texture( tex, vTexCoord + ( vec2( -0.29, -0.29 ) * aspectcorrect ) * dofblur4 );
                col += texture( tex, vTexCoord + ( vec2(  0.0,   0.4  ) * aspectcorrect ) * dofblur4 );

                outputColor = col / 41.0;
                outputColor.a = 1.0;

}
