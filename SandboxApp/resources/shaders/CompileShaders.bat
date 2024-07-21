@echo off

echo Compiling shaders...

echo:
for %%f in (*.frag) do (
	echo Compiling %%f...
	glslc.exe %%f -o %%f.spv
)

echo:
for %%f in (*.vert) do (
	echo Compiling %%f...
	glslc.exe %%f -o %%f.spv
)

echo:
echo Shaders compiled successfully
pause