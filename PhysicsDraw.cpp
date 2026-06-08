#include "PhysicsDraw.h"
#include <GLES2/gl2.h>
#include<bullet/btBulletDynamicsCommon.h>
#include "ObjectShader.h"


void PhysicsDraw::drawLine(const btVector3& from,
	const btVector3& to,
	const btVector3& color)
{
	// draws a simple line of pixels between points but stores them for later draw
	LineValues Line;
	//btVector3 scolor = btVector3(1, 0, 1);
	Line.p1 = from;
	Line.Colour1 = color;
	Line.p2 = to;
	Line.Colour2 = color;

	TheLines.push_back(Line);

}

void PhysicsDraw::DoDebugDraw()
{// set up the line shader and then draw the buffer
			//load the vertex data info


	if (TheLines.size() == 0) return;
	int howmany = TheLines.size();
	GLuint LineError = glGetError();


	int stride = 8 * sizeof(float); // 4 floats for each btvec3
	glVertexAttribPointer(mpObjectShader->debugPositionLoc,
		3,	// there are 3 values xyz
		GL_FLOAT, // they a float
		GL_FALSE, // don't need to be normalised
		stride,  // how many floats to the next one(be aware btVector3 uses 4 floats)
		(GLfloat*)&TheLines[0]  // where do they start as an index); // use 3 values, but add stride each time to get to the next
	);
	glVertexAttribPointer(mpObjectShader->debugColourLoc,
		3,	// there are 3 values xyz
		GL_FLOAT, // they a float
		GL_FALSE, // don't need to be normalised
		stride, // how many floats to the next one(be aware btVector3 uses 4 floats)
		(GLfloat*)&TheLines[0].Colour1  // where do they start as an index); // use 3 values, but add stride each time to get to the next
	);


	glEnableVertexAttribArray(mpObjectShader->debugPositionLoc);
	glEnableVertexAttribArray(mpObjectShader->debugColourLoc);

	glDrawArrays(GL_LINES, 0, (TheLines.size() * 2)); // there are 2 componants to send

	glDisableVertexAttribArray(mpObjectShader->debugPositionLoc);
	glDisableVertexAttribArray(mpObjectShader->debugColourLoc);

	LineError = glGetError();
	if (LineError != GL_NO_ERROR)
	{
		printf("Oh bugger Line Draw error\n");

	}

	TheLines.clear();
}
void PhysicsDraw::drawContactPoint(const btVector3& pointOnB,
	const btVector3& normalOnB,
	btScalar distance,
	int lifeTime,
	const btVector3& color)
{
	drawLine(pointOnB, pointOnB + (normalOnB * 2), color);
}


void PhysicsDraw::ToggleDebugFlag(int flag)
{
	// checks if a flag is set and enables/
	// disables it
	if (m_debugMode & flag)
		// flag is enabled, so disable it
		m_debugMode = m_debugMode & (~flag);
	else
		// flag is disabled, so enable it
		m_debugMode |= flag;
}
