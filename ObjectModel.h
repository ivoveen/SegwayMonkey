#pragma once
// theres and old bug in some GLM's that requires an undef of countof

//#include "ShaderManager.h"
#include "tiny_obj_loader.h"

#include<bullet/btBulletDynamicsCommon.h>

/*Parts of this file, specifically the routines, LoadandConvert and CalcNormal as well as the data structures used by them
 have been converted and modifed from Soyo Fujita's viewer example code in TinyOBJLoader which was distributed under the MIT
 license. In accordance with that license I acknowledge the original copyright and ownership of those routines and reprint
 the copyright notice here:

The MIT License (MIT)

Copyright (c) 2012-2017 Syoyo Fujita and many contributors.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/



class PhysicsObj;
class BulletManager;
#include "Model.h"





class ObjectModel : public Model
{
public:
	std::vector<btScalar> vb;// pos(3float), normal(3float), color(3float)


	/*********** This is important new content, we are adding the ability to work with OBJ's which consist of muliple shapes called DrawObjects
	 *********** These will mostly be used by OBJ's but we may use them for other types so putting them in the base class might be sensible
	 *********** however if only OBJ's use them, we should move them to the class that loads, but its probable we will create multiple types
	 *********** of derived ObjectModel derived classes that load OBJ's so this is just as availabe here, though a ModelManager/Loader class will later
	 *********** be disussed and used
	 *********** */




	ObjectModel();
	ObjectModel(char* FileName); /* this is a new constructor which can load an obj file, though this is probably temporary
											since there are many other kinds of model loader we will eventually depricate this
											and move the functionality to the classes that specifically need to load obj's
											in this project example we have a type of model called a ShipModel which is going to excusively use OBJ's
											*/

	float bmin[3] = { 0,0,0 };
	float bmax[3] = { 0,0,0 };

	bool  LoadandConvert(  // a complex routine which actually simplifies the loading of data using Tinyobj
		std::vector<DrawObject>* drawObjects,
		std::vector<tinyobj::material_t>& materials,
		std::map<std::string,
		GLuint>& textures,
		const char* filename);


	~ObjectModel();

	// a function used sometimes in model loading, so left at the base class for others to use, but mainly OBJ


	// passed and maybe removed once used
	std::vector<tinyobj::material_t> materials;
	std::vector<DrawObject> gdrawObjects;

	//temp these are to be relocated into the Model Manager	
	static std::map<std::string, GLuint> textures;


	//-------------------------------
	//physics
	BulletManager* mpBulletManager;

	typedef	enum
	{
		BOX,
		CAPSULE,
		SPHERE,
		CYLINDER,
		CONVEXHULL

	} ShapeTypes;

	btConvexShape* CreateMyShape(ShapeTypes);
	void CreatePhysicsObj(BulletManager* mBulletManager,
		btCollisionShape* pShape,
		const float& mass,
		int mask = 1); // default no rotation using w 1
	 
	PhysicsObj* MyPhysObj = nullptr;
	  
	GLuint texture1;  // a handle
	//bool		LoadModel(GLvoid* vertices);
};