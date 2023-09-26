#include "utils/structs.glsl"

uniform sampler2D vfColor;
uniform ImageParameters vfParameters;
in vec3 texCoord_;

float passThrough(vec2 coord){
    return texture(vfColor,coord).x;
}

float magnitude( vec2 coord ){
    //TASK 1: find the magnitude of the vectorfield at the position coords
	
	
    return length(coord);
}

float divergence(vec2 coord){
    //TASK 2: find the divergence of the vectorfield at the position coords
    vec2 pixelSize = vfParameters.reciprocalDimensions;
	
	vec2 center = texture(vfColor, coord).xy;
	vec2 left = texture(vfColor, coord - vec2(pixelSize.x, 0.0)).xy;
	vec2 right = texture(vfColor, coord + vec2(pixelSize.x, 0.0)).xy;
	vec2 up = texture(vfColor, coord + vec2(0.0, pixelSize.y)).xy;
	vec2 down = texture(vfColor, coord - vec2(0.0, pixelSize.y)).xy;
	
	vec2 gradX = (right - left) / (2 * pixelSize.x);
	vec2 gradY = (up - down) / (2 * pixelSize.y);
	
    return gradX.x + gradY.y;
}

float rotation(vec2 coord){
    //TASK 3: find the curl of the vectorfield at the position coords
    vec2 pixelSize = vfParameters.reciprocalDimensions;
	
    vec2 center = texture(vfColor, coord).xy;
	vec2 left = texture(vfColor, coord - vec2(pixelSize.x, 0.0)).xy;
	vec2 right = texture(vfColor, coord + vec2(pixelSize.x, 0.0)).xy;
	vec2 up = texture(vfColor, coord + vec2(0.0, pixelSize.y)).xy;
	vec2 down = texture(vfColor, coord - vec2(0.0, pixelSize.y)).xy;
	
	vec2 gradX = (right - left) / (2 * pixelSize.x);
	vec2 gradY = (up - down) / (2 * pixelSize.y);
	
    return gradY.x - gradX.y;
}

void main(void) {
    float v = OUTPUT(texCoord_.xy);
    FragData0 = vec4(v);
}
