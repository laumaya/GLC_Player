
struct light
{
	vec4 position;
	vec4 ambient_color;
	vec4 diffuse_color;
	vec4 specular_color;
	vec3 spot_direction;
	vec3 attenuation_factors;
	float spot_exponent;
	float spot_cutoff_angle;
	bool compute_distance_attenuation;
};

uniform mat4 modelview_matrix;
uniform mat4 mvp_matrix;
uniform mat3 inv_modelview_matrix;
uniform bool enable_lighting;

// Light
uniform bool light_enable_state[8];
uniform light light_state[8];

varying vec3 normal, lightDir,I;
varying vec4 Cs;

void main()
{	
	vec4 P = modelview_matrix * gl_Vertex;
	gl_ClipVertex= P;
	lightDir = normalize(vec3(gl_LightSource[0].position));
	I = P.xyz;
	normal = normalize(inv_modelview_matrix * gl_Normal);

	Cs= vec4(0.5,0.8,1.0,0.2);
		
	gl_Position = mvp_matrix * gl_Vertex;
}