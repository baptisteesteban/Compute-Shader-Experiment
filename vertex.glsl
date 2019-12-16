#version 450

in layout(location=0) vec4 pos;

void main(void)
{
	gl_Position = pos;
}
