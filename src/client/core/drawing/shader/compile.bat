glslangValidator.exe -V -S vert vertex_shader.glsl -o vertex.spv
glslangValidator.exe -V -S frag fragment_shader.glsl -o fragment.spv
glslangValidator.exe -V -S vert vertex2d_shader.glsl -o vertex2d.spv
glslangValidator.exe -V -S frag fragment2d_shader.glsl -o fragment2d.spv

pause