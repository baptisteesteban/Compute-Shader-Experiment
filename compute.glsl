#version 450 core

layout (local_size_x=1024) in;

layout(std430, binding=0) buffer pos_buffer
{
  vec4 pos[];
};

void update(int i)
{
  if (pos[i].x >= 1)
    pos[i].x = 0;
  if (pos[i].y >= 1)
    pos[i].y = 0;
  pos[i].x += 0.01;
  pos[i].y += 0.01;
}

void main()
{
  update(int(gl_GlobalInvocationID.x));
}
