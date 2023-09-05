#version 130

#define MAX_STEP 200
#define SCALE 0.5
#define MAX_t 50

in vec4 vs_out_pos;
out vec4 fs_out_col;

//uniforms to describe the 
uniform vec4 balls[5];	//xyz -> position, w -> r
uniform float tr;		//threshold

//for no disortion
uniform vec2 resolution;

/* 
display mode
0 -> depth 
1 -> normal vector as color
2 -> light
*/
uniform int mode;

//camera variables
uniform vec3 eye;
uniform vec3 at;
uniform vec3 up;

//lightning variables
uniform vec3 lightSources[3];

// Polynomial function for calc metaballs
float PolynomialFunction(vec3 pos, vec4 ball) {
    float dist = length(pos - ball.xyz);
    if (dist < ball.w)
        return pow(ball.w - dist, 2) * pow(dist / ball.w, 0.5);
    else
        return 0.0;
}

// Thin-Plate Spline function for calculating metaballs
float ThinPlateSpline(vec3 pos, vec4 ball) {
	float dist = length(pos - ball.xyz);
	if (dist < ball.w) {
		return pow(ball.w * ball.w - dist * dist, 2);
	} else {
		return 0.0;
	}
}

// Power function for calc metaballs
float PowerFunction(vec3 point, vec4 ball) {
    float dist = length(point - ball.xyz);
    return pow(ball.w / dist, 2);
}

float InverseSquareFunction(vec3 pos, vec4 ball) {
    float dist = length(pos - ball.xyz);
    return ball.w * ball.w / (dist * dist);
}

// F volume function 
float f(vec3 pos, vec4 ball){
	float d = length(ball.xyz - pos) / ball.w;

	if( d < 0){
		return 1.0;
	} else if (d > 1) {
		return 0.0;
	} else {
		return 2*pow(d,3) - 3*pow(d,2) + 1;
	}
}

float F(vec3 pos){
	float res = 0;
	for(int i=0; i<5;i++){
		res += f(pos, balls[i]);
		// res += ThinPlateSpline(pos, balls[i]);
		// res += PolynomialFunction(pos, balls[i]);
		// res += PowerFunction(pos, balls[i]);
		// res += InverseSquareFunction(pos, balls[i]);
	}
	return res - 0.5;   // f, Thin-Plate Spline
	// return res - 0.2;   // PolynomialFunction
	// return res - 8;       // PowerFunction. InverseSquareFunction
}


//distane function, distance field of metaballs, implemented for fun
float smin(float a, float b, float threshold) {
	float h = clamp(.5 + .5*(b-a)  / threshold, 0., 1.);
	return mix(b, a, h) - threshold*h*(1.-h);
}

float DistanceFromMetaballs(vec3 pos){
	    //l -> balls[0]
	float dist = length(pos - balls[0].xyz) - balls[0].w/2.0;
	for(int i=1; i<5; i++){
		float metaballDistance = length(pos - balls[i].xyz) - balls[i].w/2.0;    //l -> balls[0]
		dist = smin(dist, metaballDistance, 0.55);
	}
	return dist;
}


//calculate the normal of the surface in a given point, with gradient method;
vec3 calcNormals(vec3 p, float eps){
	float x = F(vec3(p.x + eps, p.y, p.z)) - F(vec3(p.x - eps, p.y, p.z)); 
	float y = F(vec3(p.x, p.y + eps, p.z)) - F(vec3(p.x, p.y - eps, p.z)) ; 
	float z = F(vec3(p.x, p.y, p.z + eps)) - F(vec3(p.x, p.y, p.z - eps)); 
	return -1 * normalize(vec3(x,y,z)/(2*eps));
}


//TODO:
vec3 calcLightning(vec3 normal, vec3 ro){
	vec3 lightColor = vec3(0.9);
	vec3 lightning = vec3(0.0);
	int nrOfLightSources = 1; 
	
	if(mode == 2){
		nrOfLightSources = 1;
	} else if(mode == 3) {
		nrOfLightSources = 2;
	} else if(mode == 4) {
		nrOfLightSources = 3;
	} else {
		nrOfLightSources = 3;
	}

	float constant = 1;
	float linear = 0.1;
	float quadratic = 0.06;

	for(int i=0; i<nrOfLightSources; i++){
		vec3 lightSource = normalize(lightSources[i]); 
		float diffuseStrength = max(0.0, dot(normalize(lightSource),normal));
		vec3 diffuse = lightColor * diffuseStrength;

		//blinn-Phong
		vec3 viewDir = normalize(-ro);
		vec3 lightDir = normalize(lightSource);
		vec3 halfDir = normalize(lightDir + viewDir);
		float specStrength = max(dot(halfDir, normal), 0.0);
		vec3 specular = lightColor * pow(specStrength, 64.0);

		float dist = distance(ro, lightSources[i]);
		float attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));

		lightning += diffuse * attenuation  + specular * attenuation;
	}
	return lightning;
}

//simple ray marching from a given ray origin with a given ray direction, t += scale
vec2 RayMarching(vec3 ro, vec3 rd, float MAX_DISTANCE){
    float t = 0;
    float hit = 0.0;
    for(int i=0; i<=MAX_STEP; i++){
        vec3 p = ro + t * rd;
        vec3 prev_p = ro + (t-SCALE) * rd; 
        if(F(p)  * F(prev_p) < 0){
            hit = 1.0;
            float left = t - SCALE;
            float right = t;
            while(right - left > 0.00001){
                float mid = (right + left)/2;
                vec3 p_mid = ro + mid * rd;
                if(F(p_mid) == 0){
                    break;
                } else if(F(p_mid) < 0){
                    left = mid;
                } else {
                    right = mid;
                }
            }
            t = (left + right) /2;
            break;
        }
        t += SCALE;
        if(t > MAX_DISTANCE){
            break; 
        }
    }
    return vec2(hit, t);
}


float getShadow(vec3 p, vec3 norm){
	float shadow = 1.0;
	vec3 lightPosition =  lightSources[0];
	float distanceToLightSource = distance(lightPosition,p); 
	vec3 ro = p + norm * 0.1;
	vec3 rd = normalize(lightPosition - p);
	vec2 res = RayMarching(ro, rd, distanceToLightSource );
	if(res.y < distanceToLightSource){
		shadow -= 0.25;
	}
	return shadow;
}

//Rendering the scene
void Render(vec3 frag_coord){
	// vec3 ro = vec3(0,0.3,5);
	vec3 ro = eye;
	vec3 rd = normalize(frag_coord.xyz - ro);
	vec2 res = RayMarching(ro, rd, MAX_t);
	float hit = res.x; 
	float t = res.y;

	if(hit == 1.0){
		//if part of the scene
		vec3 p = ro + t * rd;
		vec3 norm =  calcNormals(p, 0.001);
		if(mode == 0) 
		{	
			float alpha = 19.0;
			fs_out_col =  vec4(1/t * alpha ,1/t * alpha, 1/t* alpha,1);
		} else if(mode == 1){
			fs_out_col = vec4(norm * 0.5 + 0.5, 1);
		} else if(mode == 2 || mode ==3 || mode == 4){
			fs_out_col = vec4(pow(calcLightning(norm,ro),vec3(1.0/2.2)),1);
		}
	} else {
		if(mode == 2){
			vec3 p = ro + t * rd;
			fs_out_col = vec4(vec3(0.8,0.8,0.8) * getShadow(p, normalize(vec3(0,1,0))),1);
		} else {
			fs_out_col = vec4(0.8,0.8,0.8,1);
		}
	
	}
}

void main()
{
	vec3 frag_coord = vs_out_pos.xyz; 
	frag_coord.x *= resolution.x/ resolution.y;

	Render(frag_coord);
}
