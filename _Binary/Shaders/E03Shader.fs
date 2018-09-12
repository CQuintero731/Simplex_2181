#version 330

in vec3 Color;

uniform vec3 SolidColor = vec3(-1,-1,-1);
uniform bool Complimentary;

out vec4 Fragment;

void main()
{
	vec3 outputColor = Color;
	
	if(SolidColor.r != -1.0 && SolidColor.g != -1.0 && SolidColor.b != -1.0)
	{
		outputColor = SolidColor;
	}
	
	if(Complimentary == true)
	{
		outputColor = vec3(1.0f, 1.0f, 1.0f) - outputColor;
	}
	
	Fragment = vec4(outputColor,1);
		
	return;
}