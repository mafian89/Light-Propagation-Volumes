#version 430

in vec2 uv;

out vec4 final_color;

uniform sampler2D texture;

void main()
{
    final_color = texture2D(texture, uv);
	//final_color = vec4(1.0,0.0,0.0,1.0);
}