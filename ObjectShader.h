#pragma once
#include "Graphics.h"




class Graphics;
using namespace glm;

class ObjectShader
{
public:
	enum shaderType {
		Object,
		Debug,
		Terrain
	};

	GLuint* currentProgramObject = new GLuint;
	GLuint debugProgramObject = -1;
	GLuint objectProgramObject = -1;
	GLuint terrainProgramObject = -1;


	bool InitShaders(const std::string& filename, shaderType typeOfShader);
	GLuint LoadShader(GLenum type, const char* shaderSrc);

	// use/activate the shader
	void use(shaderType typeOfShader);
	// uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setVec4(const std::string& name, glm::vec4 value) const;
	void setMat4(const std::string& name, glm::mat4 value) const;

	GLint	positionLoc;  // index handles to important concepts in texture 
	GLint	colourLoc;  // index handles to important concepts in texture 
	GLint	texCoordLoc;
	GLint	normalLoc;
	GLint	samplerLoc;

	GLint	debugPositionLoc;  // index handles to important concepts in texture 
	GLint	debugColourLoc;  // index handles to important concepts in texture 
	GLint	debugTexCoordLoc;
	GLint	debugNormalLoc;
	GLint	debugSamplerLoc;

	GLint	terrainPositionLoc;  // index handles to important concepts in texture 
	GLint	terrainColorLoc;  // index handles to important concepts in texture 
	GLint	terrainNormalLoc;
	GLint	terrainSamplerLoc;

private:
	void SetAttributeLocations();
	GLuint objectVertexShader, objectFragmentShader;  // programObject used for draw. vertex and fragment used to create programObject
	GLuint debugVertexShader, debugFragmentShader;  // programObject used for draw. vertex and fragment used to create programObject
	GLuint terrainVertexShader, terrainFragmentShader;  // programObject used for draw. vertex and fragment used to create programObject

	// Ambiant light is universal in all model types, so we should keep this in the base class	
	float Amb[4];  // = { Rand(1.0f), Rand(1.0f), Rand(1.0f), 1.0f };

	Graphics* TheGraphics;

};

