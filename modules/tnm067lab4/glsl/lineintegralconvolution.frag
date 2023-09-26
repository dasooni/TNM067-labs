uniform sampler2D vfColor;
uniform sampler2D noiseColor;

uniform int nSteps;
uniform float stepSize;

in vec3 texCoord_;

/*
* Traverse the vector field and sample the noise image
* @param posF Starting position
* @param stepSize length of each step
* @param nSteps the number of steps to traverse
* @param accVal the accumulated value from sampling the noise image
* @param nSamples the number of samples used for v
*/
void traverse(vec2 posF, float stepSize, int nSteps, inout float accVal, inout int nSamples){
    // traverse the vectorfield staring at `posF` for `nSteps` 
	// using `stepSize` and sample the noiseColor texture for each position
    // store the accumulated value in `accVal` and the amount of samples in `nSamples`
	
	vec2 currentPos = posF;
	
	for (int i = 0; i < nSteps; i++) {
		vec2 v = normalize(texture(vfColor, currentPos).xy);
		
		
		currentPos += stepSize * v;
		vec3 noise = texture(noiseColor, currentPos).rgb;
		
		accVal += noise[0];
		nSamples++;
		
	}
	
}

void main(void) {
    float accVal = texture(noiseColor, texCoord_.xy).r;
    int nSamples = 1;
    
    //traverse the vector field both forward and backwards to calculate the output color
	traverse(texCoord_.xy, stepSize, nSteps/2, accVal, nSamples);
	traverse(texCoord_.xy, -1.0 * stepSize, nSteps / 2, accVal, nSamples);
	
	accVal /= nSamples;
    FragData0 = vec4(accVal, accVal, accVal, 1);
	
	
}
