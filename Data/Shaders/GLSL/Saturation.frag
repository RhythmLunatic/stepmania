#version 130

uniform sampler2D Texture1;
uniform int ViewportHeight;

#define SCALE 40

void main(void) {
    vec4 pixel_color = texture2D(Texture1, gl_TexCoord[0].xy);
    int fadeStart = ViewportHeight/2-100;
    int fadeEnd = ViewportHeight/2+100;
    
    //scale from y pos bottom to y pos top if it's in the range of smoothstep
    //If it's outside the range it would either return 0.0 or 1.0
    //min() - Keep the transparent parts of the sprite
    float clr = min(smoothstep(gl_FragCoord.y-SCALE,gl_FragCoord.y+SCALE,fadeStart),pixel_color.a);
    
    //We don't want to overwrite faded notes and set their transparency back to 1.0
    if (fadeEnd > fadeStart && gl_FragCoord.y-SCALE > fadeStart)
        clr = min(smoothstep(gl_FragCoord.y+SCALE, gl_FragCoord.y-SCALE,fadeEnd),pixel_color.a);
    
    gl_FragColor = vec4(pixel_color.r, pixel_color.g, pixel_color.b, clr);
    
}
