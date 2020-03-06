uniform sampler2D Texture1;

void main(void) {
    vec4 sample =  texture2D(Texture1, gl_TexCoord[0]);
    vec3 grey = vec3(0.22 * sample.r + 0.71 * sample.g + 0.07 * sample.b);
    gl_FragColor = vec4(grey, sample.a);
}

/* Written by ArcticFqx, adapted from NotITG */
