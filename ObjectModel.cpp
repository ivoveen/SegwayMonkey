#include<bullet/btBulletDynamicsCommon.h>

#include "ObjectModel.h"
#include "Graphics.h"
#include "PhysicsObj.h"
#include "BulletManager.h"
#include "../Headers/stb_image.h"

using namespace glm;
//******************************************************************************************
// A base class, containing information needed to position/rotate and create a model.
//******************************************************************************************
//testing a theory

//temp these are to be relocated into the Model Manager	
std::map<std::string, GLuint> ObjectModel::textures;



ObjectModel::ObjectModel()
{
	// really don't want these to do anything
}



ObjectModel::~ObjectModel()
{
	mpBulletManager->DynamicPhysicsWorld->removeRigidBody(MyPhysObj->GetRigidBody());  // group 1, mask 1;
	delete MyPhysObj;

	glDeleteBuffers(1, &gdrawObjects[0].vb);
}


// many types of derived object will want to use OBJ's as their model format of choice, so its good to have the loading systems here
// some model types may not use OBJ though, and it might happen your game never uses OBJ's, in which case you can choose to leave it in or remove it.
ObjectModel::ObjectModel(char* FileName)
{
	bool ret = LoadandConvert(&gdrawObjects, materials, textures, FileName);

	if (ret == false) printf("This load for %s failed, check file name and try again!\n", FileName);
}

// this loads a standard OBJ file, since many types of object are likely to use OBJ's we allow the base model class (ObjectModel) to be able to load and store a standard OBJ model
bool  ObjectModel::LoadandConvert(
	std::vector<DrawObject>* drawObjects,
	std::vector<tinyobj::material_t>& materials,
	std::map<std::string,
	GLuint>& textures,
	const char* filename) {

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;

	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, "./Resources/Models/");

	if (!err.empty())
	{
		char* cstr = &err[0u];   // printf needs chars
		printf("oh Bugger can't load this obj file %s file returns error %s \n", filename, cstr);
	}

	if (!ret)
	{
		printf("The %s.obj seems to be corrupt or incorrectly formated\n", filename);
		return false;
	}


	// output the data, but wrap in a debug when not used
	/*printf("Attrib data for %s is :-\n", filename);
	printf("# vertices  : %d\n", (int)(attrib.vertices.size()) / 3);
	printf("# normals   : %d\n", (int)(attrib.normals.size()) / 3);
	printf("# texcoords : %d\n", (int)(attrib.texcoords.size()) / 2);
	printf("Other data  :-\n");

	printf("# materials : %d\n", (int)materials.size());
	printf("# shapes    : %d\n", (int)shapes.size());*/

	// Append `default` material
	materials.push_back(tinyobj::material_t());

	char* base_dir = (char*)"./Resources/Models/";   //<< this pre-supposes you store your material files with your obj and texture files
	// Parse the materials and Load and create textures
	{

		for (size_t m = 0; m < materials.size(); m++) {
			tinyobj::material_t* mp = &materials[m];

			if (mp->diffuse_texname.length() > 0) {
				// Only load the texture if it is not already loaded
				if (textures.find(mp->diffuse_texname) == textures.end()) {
					GLuint texture_id;
					int w, h;
					int comp;

					std::string texture_filename = mp->diffuse_texname;
					texture_filename = base_dir + mp->diffuse_texname;

					char* cstr = &texture_filename[0u];    // embarrasing side effect of the decision to use char.....
					char* image = (char*)stbi_load(cstr, &w, &h, &comp, 4);

					if (!image) {
						printf("Unable to load texture:%s \n", cstr);
						exit(1);
					}

					/*printf("Texture info for %s :-\n", cstr);

					printf("# width   : %d\n", w);
					printf("# Height  : %d\n", h);
					printf("# comp    : %d\n", comp);*/


					glGenTextures(1, &texture_id);
					glBindTexture(GL_TEXTURE_2D, texture_id);

					// set up some paramaters
					// Set our texture parameters
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   	// Set texture wrapping to GL_REPEAT
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					// Set texture filtering
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

					if (comp == 4)	// check if we had an alpha texture or a plane (strictly speaking any value not 3 is wrong)	
					{
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
					}
					else
					{
						if (comp != 3) printf("Made a default non alpha texture, comp was :%d\n", comp);
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
					}

					if (glGetError() != GL_NO_ERROR)
					{
						printf("Oh bugger:- Model texture creation error, but attempting to carry on.\n");

					}

					glBindTexture(GL_TEXTURE_2D, 0);
					free(image);    // release the cpu memory once its been put in the GPU
					// store the resulting texture ID in a map, associated with its filename
					textures.insert(std::make_pair(mp->diffuse_texname, texture_id));
				}
			}
		}
	}


	{
		// now lets put lots of data out of the vertext list, 	
		for (size_t s = 0; s < shapes.size(); s++) {
			DrawObject o;
			//std::vector<float> vb;     // pos(3float), normal(3float), color(3float)
			for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++) {
				tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
				tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
				tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

				int current_material_id = shapes[s].mesh.material_ids[f];

				if ((current_material_id < 0) || (current_material_id >= static_cast<int>(materials.size()))) {
					// Invaid material ID. Use default material.
					current_material_id = materials.size() - 1;    // Default material is added to the last item in `materials`.
				}

				float diffuse[3];
				for (size_t i = 0; i < 3; i++) {
					diffuse[i] = materials[current_material_id].diffuse[i];
				}
				float tc[3][2];
				if (attrib.texcoords.size() > 0) {
					assert(attrib.texcoords.size() > 2 * idx0.texcoord_index + 1);
					assert(attrib.texcoords.size() > 2 * idx1.texcoord_index + 1);
					assert(attrib.texcoords.size() > 2 * idx2.texcoord_index + 1);
					tc[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
					tc[0][1] = 1.0f - attrib.texcoords[2 * idx0.texcoord_index + 1];
					tc[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
					tc[1][1] = 1.0f - attrib.texcoords[2 * idx1.texcoord_index + 1];
					tc[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
					tc[2][1] = 1.0f - attrib.texcoords[2 * idx2.texcoord_index + 1];
				}
				else {
					tc[0][0] = 0.0f;
					tc[0][1] = 0.0f;
					tc[1][0] = 0.0f;
					tc[1][1] = 0.0f;
					tc[2][0] = 0.0f;
					tc[2][1] = 0.0f;
				}

				float v[3][3];
				for (int k = 0; k < 3; k++) {
					int f0 = idx0.vertex_index;
					int f1 = idx1.vertex_index;
					int f2 = idx2.vertex_index;
					assert(f0 >= 0);
					assert(f1 >= 0);
					assert(f2 >= 0);

					v[0][k] = attrib.vertices[3 * f0 + k];
					v[1][k] = attrib.vertices[3 * f1 + k];
					v[2][k] = attrib.vertices[3 * f2 + k];

					bmin[k] = std::min(v[0][k], bmin[k]);
					bmin[k] = std::min(v[1][k], bmin[k]);
					bmin[k] = std::min(v[2][k], bmin[k]);
					bmax[k] = std::max(v[0][k], bmax[k]);
					bmax[k] = std::max(v[1][k], bmax[k]);
					bmax[k] = std::max(v[2][k], bmax[k]);
				}

				float n[3][3];
				if (attrib.normals.size() > 0) {
					int f0 = idx0.normal_index;
					int f1 = idx1.normal_index;
					int f2 = idx2.normal_index;
					assert(f0 >= 0);
					assert(f1 >= 0);
					assert(f2 >= 0);
					for (int k = 0; k < 3; k++) {
						n[0][k] = attrib.normals[3 * f0 + k];
						n[1][k] = attrib.normals[3 * f1 + k];
						n[2][k] = attrib.normals[3 * f2 + k];
					}
				}
				else {
					// compute geometric normal
					CalcNormal(n[0], v[0], v[1], v[2]);
					n[1][0] = n[0][0];
					n[1][1] = n[0][1];
					n[1][2] = n[0][2];
					n[2][0] = n[0][0];
					n[2][1] = n[0][1];
					n[2][2] = n[0][2];
				}

				for (int k = 0; k < 3; k++) {
					vb.push_back(v[k][0]);
					vb.push_back(v[k][1]);
					vb.push_back(v[k][2]);

					//	printf("v1 v2 v3 =%f  %f  %f\n", v[k][0], v[k][1], v[k][2]);

					vb.push_back(n[k][0]);
					vb.push_back(n[k][1]);
					vb.push_back(n[k][2]);
					// Combine normal and diffuse to get color.
					float normal_factor = 0.2;
					float diffuse_factor = 1 - normal_factor;
					float c[3] = {
						n[k][0] * normal_factor + diffuse[0] * diffuse_factor,
						n[k][1] * normal_factor + diffuse[1] * diffuse_factor,
						n[k][2] * normal_factor + diffuse[2] * diffuse_factor
					};
					float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
					if (len2 > 0.0f) {
						float len = sqrtf(len2);

						c[0] /= len;
						c[1] /= len;
						c[2] /= len;
					}
					vb.push_back(c[0] * 0.5 + 0.5);
					vb.push_back(c[1] * 0.5 + 0.5);
					vb.push_back(c[2] * 0.5 + 0.5);

					vb.push_back(tc[k][0]);
					vb.push_back(tc[k][1]);
				}
			}

			o.vb = 0;
			o.numTriangles = 0;


			if (shapes[s].mesh.material_ids.size() > 0 && shapes[s].mesh.material_ids.size() > s)
			{
				// Base case
				o.material_id = shapes[s].mesh.material_ids[s];
			}
			else
			{
				o.material_id = materials.size() - 1;    // = ID for default material.
			}


			if (vb.size() > 0)
			{
				glGenBuffers(1, &o.vb);
				glBindBuffer(GL_ARRAY_BUFFER, o.vb);
				glBufferData(GL_ARRAY_BUFFER,
					vb.size() * sizeof(float),
					&vb.at(0),
					GL_STATIC_DRAW);

				o.numTriangles = vb.size() / (3 + 3 + 3 + 2);    // note we have vertices, normals, colour, and uv, we proably won't use normals and colour yet

				/*printf("shape[%d] # of triangles = %d\n",
					static_cast<int>(s),
					o.numTriangles);*/

				GLsizei size;
				glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
				//printf("VB is reported as %d \n", size);
			}

			drawObjects->push_back(o);
		}
	}
	return true;
}



void ObjectModel::CreatePhysicsObj(BulletManager* bulletManager,
	btCollisionShape* pShape,
	const float& mass,
	int mask) // mask default sto 1
{
	mpBulletManager = bulletManager;
	btVector3 position = btVector3((int)worldPosition.x, (int)worldPosition.y, (int)worldPosition.z);
	// create a new game object
	btQuaternion rotation;
	rotation.setEulerZYX(rotations.z, rotations.y, rotations.x);


	MyPhysObj = new PhysicsObj(pShape, mass, position, rotation);
	float currentm = pShape->getMargin();

	pShape->setMargin(0.09f);

	if (mpBulletManager->DynamicPhysicsWorld) // if not nullptr
	{
		mpBulletManager->DynamicPhysicsWorld->addRigidBody(MyPhysObj->GetRigidBody(), 1, 1);  // group 1, mask 1;
		// the group and mask are accessable
		//		pPhysicsObject->GetRigidBody()->getBroadphaseHandle()->m_collisionFilterGroup;
		//		pPhysicsObject->GetRigidBody()->getBroadphaseHandle()->m_collisionFilterMask;

	}
	else
	{
		printf("Attempting to add to a Dynamic world that does not exist\n");
	}


}




// only used if Bullet is present
//#include <bullet/btBulletDynamicsCommon.h>

btConvexShape* ObjectModel::CreateMyShape(ShapeTypes shape)
{
	offsetY = ((bmax[1] - bmin[1]) / 2.0f) * scales.y;
	//btVector3 vBox = btVector3(10, 10, 10); 
	btVector3 vBox = btVector3(((bmax[0] - bmin[0]) / 2.0f) * scales.x, ((bmax[1] - bmin[1]) / 2.0f) * scales.y, ((bmax[2] - bmin[2]) / 2.0f) * scales.z);
	//	Volume = ((bmax[0] - bmin[0]) * (bmax[1] - bmin[1]) * (bmax[2] - bmin[2])); 

	switch (shape)
	{
	case	 BOX:
	{
		btBoxShape* Box = new btBoxShape(vBox);
		return Box;
	}
	break;
	case	CAPSULE:
	{
		btCapsuleShape* Capsule = new btCapsuleShape(vBox.x(), vBox.y());
		return Capsule;

	}
	break;

	case	SPHERE:
	{
		btSphereShape* Sphere = new btSphereShape(vBox.y());
		return Sphere;
	}
	break;

	case	CYLINDER:
	{
		btCylinderShape* Cylinder = new btCylinderShape(vBox);
		return Cylinder;
	}
	break;
	case	CONVEXHULL:
	{
		btConvexHullShape* ConvexHull = new btConvexHullShape();
		int stride = (3 + 3 + 3 + 2);
		btVector3 point;
		btScalar* pFront = &vb.front();
		for (int i = 0; i < (vb.size() / stride); i++) {
			//
			point = btVector3(*(pFront)*scales.x, *(pFront + 1) * scales.y - offsetY, *(pFront + 2) * scales.z);
			ConvexHull->addPoint(point);
			pFront += stride;
		}
		ConvexHull->setMargin(btScalar(0.0001f));
		return ConvexHull;

	}

	break;

	default:
	{
		exit(1);  // this will cause a crash so stop
	}
	break;
	}
}