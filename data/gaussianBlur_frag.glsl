uniform vec2 resolution;
uniform float time;
uniform sampler2D tex0;

uniform mat4 previous_mvp;

varying vec2 texture_coordinate; 
varying vec3 normal;
varying vec3 vertex_to_light_vector;
 
void main()
{
    // Defining The Material Colors
    const vec4 AmbientColor = vec4(0.1, 0.1, 0.1, 1.0);
    const vec4 DiffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
 
    // Scaling The Input Vector To Length 1
    vec3 normalized_normal = normalize(normal);
    vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);
 
    // Calculating The Diffuse Term And Clamping It To [0;1]
    float DiffuseTerm = clamp(dot(normal, vertex_to_light_vector), 0.0, 1.0);
 
    // Calculating The Final Color
    gl_FragColor = (AmbientColor + DiffuseColor * DiffuseTerm) * texture2D(tex0, texture_coordinate);
}