#include "ObjectShader.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool ObjectShader::InitShaders(const std::string& filename, shaderType typeOfShader)
{
	std::string fragmentPath = std::string("./Shaders/" + filename + ".frag");
	std::string vertexPath = std::string("./Shaders/" + filename + ".vert");

	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try 
	{
		std::stringstream vShaderStream, fShaderStream;

		fShaderFile.open((const char*)fragmentPath.c_str());
		if (!fShaderFile.is_open()) {
		//	std::cerr << "ERROR::SHADER::VERTEX_FILE_NOT_SUCCESFULLY_READ: " << vertexPath << std::endl;
			return false;
		}
		fShaderStream << fShaderFile.rdbuf();
		fShaderFile.close();


		// open files
		vShaderFile.open((const char*)vertexPath.c_str());
		if (!vShaderFile.is_open()) {
			std::cerr << "ERROR::SHADER::VERTEX_FILE_NOT_SUCCESFULLY_READ: " << vertexPath << std::endl;
			return false;
		}
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		// convert stream into string

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	//printf("shader loaded from vertex \n %s\n", vShaderCode);
	const char* fShaderCode = fragmentCode.c_str();
	//printf("shader loaded from fragment \n %s\n", fShaderCode);

	// Load and compile the vertex/fragment shaders
	objectVertexShader = LoadShader(GL_VERTEX_SHADER, (const char*)vShaderCode);
	objectFragmentShader = LoadShader(GL_FRAGMENT_SHADER, (const char*)fShaderCode);

	// Create the program object	
	*currentProgramObject = glCreateProgram();
	if (*currentProgramObject == 0) {
		fprintf(stderr, "Error can't create shaderprogram");
		return 0;
	}
	// now we have the V and F shaders  attach them to the progam object
	glAttachShader(*currentProgramObject, objectVertexShader);
	glAttachShader(*currentProgramObject, objectFragmentShader);

	// Link the program
	glLinkProgram(*currentProgramObject);
	// Check the link status
	GLint AreTheylinked;
	glGetProgramiv(*currentProgramObject, GL_LINK_STATUS, &AreTheylinked);
	if (!AreTheylinked)
	{
		GLint RetinfoLen = 0;
		// check and report any errors
		glGetProgramiv(*currentProgramObject, GL_INFO_LOG_LENGTH, &RetinfoLen);
		if (RetinfoLen > 1)
		{
			GLchar* infoLog = (GLchar*)malloc(sizeof(char) * RetinfoLen);
			glGetProgramInfoLog(*currentProgramObject, RetinfoLen, NULL, infoLog);
			fprintf(stderr, "Error linking program:\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteProgram(*currentProgramObject);
		return FALSE;
	}


	switch (typeOfShader)
	{
	case ObjectShader::Object:
		objectProgramObject = *currentProgramObject;
		SetAttributeLocations();

		// Get the attribute locations
		positionLoc = glGetAttribLocation(objectProgramObject, "a_position");
		colourLoc = glGetAttribLocation(objectProgramObject, "a_colour");
		texCoordLoc = glGetAttribLocation(objectProgramObject, "a_texCoord");
		normalLoc = glGetAttribLocation(objectProgramObject, "a_normal");
		// Get the sampler location
		samplerLoc = glGetUniformLocation(objectProgramObject, "s_texture");
		if (glGetError() != GL_NO_ERROR) printf("Oh fiddlesticks, there seems to be a problem getting attribute data\n");

		break;
	case ObjectShader::Debug:
		debugProgramObject = *currentProgramObject;

		// Get the attribute locations
		debugPositionLoc = glGetAttribLocation(debugProgramObject, "a_position");
		debugColourLoc = glGetAttribLocation(debugProgramObject, "a_colour");
		debugTexCoordLoc = glGetAttribLocation(debugProgramObject, "a_texCoord");
		debugNormalLoc = glGetAttribLocation(debugProgramObject, "a_normal");
		// Get the sampler location
		debugSamplerLoc = glGetUniformLocation(debugProgramObject, "s_texture");
		if (glGetError() != GL_NO_ERROR) printf("Oh fiddlesticks, there seems to be a problem getting attribute data\n");
		break;
	case ObjectShader::Terrain: 
		terrainProgramObject = *currentProgramObject;

		// Get the attribute locations
		terrainPositionLoc = glGetAttribLocation(terrainProgramObject, "a_position");
		terrainColorLoc = glGetAttribLocation(terrainProgramObject, "a_color");
		terrainNormalLoc = glGetAttribLocation(terrainProgramObject, "a_normal");
		// Get the sampler location
		terrainSamplerLoc = glGetUniformLocation(terrainProgramObject, "s_texture");
		if (glGetError() != GL_NO_ERROR) printf("Oh fiddlesticks, there seems to be a problem getting attribute data terrain\n");
		break;
	default:
		printf("INCORRECT TYPE OF SHADER INIT");
		break;
	}
}

void ObjectShader::SetAttributeLocations() {

}


GLuint ObjectShader::LoadShader(GLenum type, const char* shaderSrc)
{
	// 1st create the shader object
	GLuint TheShader = glCreateShader(type);

	if (TheShader == 0) {
		fprintf(stderr, "Error can't allocate this shader");
		return FALSE; // can't allocate so stop.
	}
	// pass the shader source then compile it
	glShaderSource(TheShader, 1, &shaderSrc, NULL);
	glCompileShader(TheShader);

	GLint  IsItCompiled;
	// After the compile we need to check the status and report any errors
	glGetShaderiv(TheShader, GL_COMPILE_STATUS, &IsItCompiled);
	if (!IsItCompiled)
	{
		GLint RetinfoLen = 0;
		glGetShaderiv(TheShader, GL_INFO_LOG_LENGTH, &RetinfoLen);
		if (RetinfoLen > 1)
		{
			// standard output for errors
			char* infoLog = (char*)malloc(sizeof(char) * RetinfoLen);
			glGetShaderInfoLog(TheShader, RetinfoLen, NULL, infoLog);
			fprintf(stderr, "Error compiling this shader:\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteShader(TheShader);
		return FALSE;
	}
	return TheShader;
}



void ObjectShader::use(shaderType typeOfShader)
{
	switch (typeOfShader)
	{
	case ObjectShader::Object:
		glUseProgram(objectProgramObject);
		currentProgramObject = &objectProgramObject;
		break;
	case ObjectShader::Debug:
		glUseProgram(debugProgramObject);
		currentProgramObject = &debugProgramObject;
		break;
	case ObjectShader::Terrain:
		glUseProgram(terrainProgramObject);
		currentProgramObject = &terrainProgramObject;
		break;
	default:
		printf("INCORRECT TYPE OF SHADER");
		break;
	}
}

void ObjectShader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(*currentProgramObject, name.c_str()), (int)value);
}
void ObjectShader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(*currentProgramObject, name.c_str()), value);
}
void ObjectShader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(*currentProgramObject, name.c_str()), value);
}

void ObjectShader::setVec3(const std::string& name, glm::vec3 value) const
{
	float valueArray[3] = { value.x, value.y, value.z };
	glUniform3fv(glGetUniformLocation(*currentProgramObject, name.c_str()), 1, &valueArray[0]);
}
void ObjectShader::setVec4(const std::string& name, glm::vec4 value) const
{
	float valueArray[4] = { value.x, value.y, value.z, value.r };
	glUniform4fv(glGetUniformLocation(*currentProgramObject, name.c_str()), 1, &valueArray[0]);
}

void ObjectShader::setMat4(const std::string& name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(*currentProgramObject, name.c_str()), 1, GL_FALSE, &value[0][0]);
}
