#version 430

layout(points )in;
layout(points ,max_vertices = 1 )out;

out vec4 color;


void main(){
	/*for (int i = 0; i < gl_in.length(); i++)
	{
		gl_Position=gl_in[i].gl_Position;
		EmitVertex();
		EndPrimitive();
	}*/

		gl_Position=gl_in[0].gl_Position;
		color = vec4(0.0,1.0,0.0,1.0);
		gl_Layer = 1;
		EmitVertex();
		EndPrimitive();

}