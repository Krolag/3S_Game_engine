#version 330 core
// input
layout (points) in;
layout (line_strip, max_vertices = 30) out;

// output
out vec3 fColor;

// uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 radius;
uniform bool collision;

// functions
void emitFaces();

void main() {
	if(collision) fColor = vec3(1.0, 0.0, 0.0);
	else fColor = vec3(0.0, 1.0, 0.0);
	emitFaces();
	EndPrimitive();
}

void emitFaces() {
	// WE DONT NEED TO SET ALL THE FACES CAUSE 4 FACES MADE BY STRIP LINE WILL COVER ALL THE EDGES 
	
	// FRONT
	vec4 offset = gl_in[0].gl_Position + vec4(-radius.x,  radius.y, radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
    offset = gl_in[0].gl_Position + vec4( radius.x,  radius.y, radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
    offset = gl_in[0].gl_Position + vec4( radius.x, -radius.y, radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
	offset = gl_in[0].gl_Position + vec4(-radius.x, -radius.y, radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
	offset = gl_in[0].gl_Position + vec4(-radius.x,  radius.y, radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
	// BACK
	offset = gl_in[0].gl_Position + vec4(-radius.x,  radius.y, -radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
    offset = gl_in[0].gl_Position + vec4( radius.x,  radius.y, -radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
    offset = gl_in[0].gl_Position + vec4( radius.x, -radius.y, -radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
	offset = gl_in[0].gl_Position + vec4(-radius.x, -radius.y, -radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
	offset = gl_in[0].gl_Position + vec4(-radius.x,  radius.y, -radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
	// TOP
	offset = gl_in[0].gl_Position + vec4(-radius.x, radius.y, -radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
    offset = gl_in[0].gl_Position + vec4( radius.x, radius.y, -radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
    offset = gl_in[0].gl_Position + vec4( radius.x, radius.y,  radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
	offset = gl_in[0].gl_Position + vec4(-radius.x, radius.y,  radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
	offset = gl_in[0].gl_Position + vec4(-radius.x, radius.y, -radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();

	// BOTTOM
	offset = gl_in[0].gl_Position + vec4(-radius.x, -radius.y, -radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
    offset = gl_in[0].gl_Position + vec4( radius.x, -radius.y, -radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
    offset = gl_in[0].gl_Position + vec4( radius.x, -radius.y,  radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
	offset = gl_in[0].gl_Position + vec4(-radius.x, -radius.y,  radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();
	
	offset = gl_in[0].gl_Position + vec4(-radius.x, -radius.y, -radius.z, 0.0);
	gl_Position = projection * view * model * offset;
    EmitVertex();

}