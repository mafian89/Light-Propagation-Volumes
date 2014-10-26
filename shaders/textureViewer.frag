#version 430

in vec2 uv;

out vec4 final_color;

uniform sampler2D texture;
uniform bool depth;

void main()
{
	//rrra because depth textures are not usual textures, they have only one channel
	final_color = (depth) ? texture2D(texture, uv).rrra : texture2D(texture, uv).rgba;
    //final_color = texture2D(texture, uv).rrra;
	//final_color = vec4(1.0,0.0,0.0,1.0);
}