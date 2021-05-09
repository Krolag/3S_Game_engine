#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
 vec4 gpass_clipSpaceReal;
 vec4 gpass_clipSpaceGrid;
 vec3 gpass_toCameraVector;
 vec3 gpass_normal;
} gs_in[];

out vec4 pass_clipSpaceReal;
out vec4 pass_clipSpaceGrid;
out vec3 pass_toCameraVector;
out vec3 pass_normal;
out vec3 normalG;


vec3 GetNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}  
	
void main()
{
	
	normalG = GetNormal();
    pass_normal = gs_in[0].gpass_normal;	
	pass_clipSpaceGrid = gs_in[0].gpass_clipSpaceGrid;
	pass_clipSpaceReal = gs_in[0].gpass_clipSpaceReal;
	pass_toCameraVector = gs_in[0].gpass_toCameraVector;
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	
	pass_normal = gs_in[1].gpass_normal;
	pass_clipSpaceGrid = gs_in[1].gpass_clipSpaceGrid;
	pass_clipSpaceReal = gs_in[1].gpass_clipSpaceReal;
	pass_toCameraVector = gs_in[1].gpass_toCameraVector;
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	
	pass_normal = gs_in[2].gpass_normal;
	pass_clipSpaceGrid = gs_in[2].gpass_clipSpaceGrid;
	pass_clipSpaceReal = gs_in[2].gpass_clipSpaceReal;
	pass_toCameraVector = gs_in[2].gpass_toCameraVector;
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	}