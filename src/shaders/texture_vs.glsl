uniform mat4 projection;
uniform mat4 modelView;
in vec3 aPosition;
in vec2 aTexCoords;
out vec2 vTexCoords;

void main()
{
	gl_Position = projection * modelView * vec4(aPosition, 1.0);
	vTexCoords = aTexCoords;
}
