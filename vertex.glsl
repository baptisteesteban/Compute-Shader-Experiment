#version 450

in layout(location=0) vec3 pos;

void main(void)
{
	gl_Position = vec4(pos, 1.0);
}