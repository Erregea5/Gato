rem @echo off
SetLocal EnableDelayedExpansion
set nameOfFiles=Tests\Game
set directory=C:\Users\Sebastian\source\repos\Gato\src\
set output=%directory%%nameOfFiles%.cpp

set app=Object Application Scripting EditorGUI SaveSystem pch
set graphics=Camera FrameBuffer IndexBuffer Light Mesh Renderer Shader Texture VertexArray VertexBuffer
set physics=Transform Collision 
set vendor=stb_image\stb_image ImGuiFileDialog\ImGuiFileDialog sol2_ImGui_Bindings\sol_ImGui imgui\imgui_impl_opengl3 glm\detail\glm imgui\imgui_draw imgui\imgui_tables imgui\imgui_widgets imgui\imgui_demo imgui\imgui imgui\imgui_impl_glfw 
set audio=SoundBuffer Source Listener

set images=graycloud_bk.jpg graycloud_dn.jpg graycloud_ft.jpg graycloud_lf.jpg graycloud_rt.jpg graycloud_up.jpg LCACloud.png LCASparks.png
set shaders=color.shader cube.shader default.shader directLightDepth.shader light.shader pointLightDepth.shader redTriangle.shader screen.shader skybox.shader
set objs=baseballBat.obj capsule.obj cone.obj cube.obj cylinder.obj disk.obj FinalBaseMesh.obj

set shaders=color.shader

for %%a in (%app%) do set output=!output! %directory%Application\%%a.cpp
for %%a in (%graphics%) do set output=!output! %directory%Graphics\%%a.cpp
for %%a in (%physics%) do set output=!output! %directory%Physics\%%a.cpp
for %%a in (%vendor%) do set output=!output! %directory%vendor\%%a.cpp
for %%a in (%audio%) do set output=!output! %directory%Audio\%%a.cpp

set output=%output% -I%directory%vendor\ -I%directory%Audio\ -I%directory%Physics\ -I%directory%Graphics\ -I%directory%Application\ -IC:\Users\Sebastian\source\repos\Gato\Dependencies\glfw\include -IC:\Users\Sebastian\source\repos\Gato\Dependencies\glew\include -IC:\Users\Sebastian\source\repos\Gato\Dependencies\OpenAL\include -IC:\Users\Sebastian\source\repos\Gato\Dependencies\lua\include -o %directory%%nameOfFiles%.html -s USE_GLFW=3 

set directory=c:\Program Files\emsdk\res\
for %%a in (%images%) do set output=!output! --preload-file "%directory%textures\%%a"
for %%a in (%shaders%) do set output=!output! --preload-file "%directory%shaders\%%a"
for %%a in (%objs%) do set output=!output! --preload-file "%directory%meshes\%%a"

em++ %output%