#include "MyOctant.h"
using namespace Simplex;

//Setting static variables
uint MyOctant::mOctantCount = 0;
uint MyOctant::mMaxLevel = 3;
uint MyOctant::mIdealEntityCount = 5;

//Properties
float Simplex::MyOctant::GetSize(void) { return mSize; }
uint Simplex::MyOctant::GetOctantCount(void) { return mOctantCount; }

//Constructors and Destructor
Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	Init();
	mOctantCount = 0;
	mID = mOctantCount;//Setting the ID to the current octant count
	mMaxLevel = a_nMaxLevel;
	mIdealEntityCount = a_nIdealEntityCount;

	mRoot = this;
	mChildList.clear();

	//Get a vertex list in order to make a RigidBody
	std::vector<vector3> maxMinList;
	for (uint i = 0; i < mEntityMngr->GetEntityCount(); i++)
	{
		MyRigidBody* rigidBody = mEntityMngr->GetEntity(i)->GetRigidBody();

		maxMinList.push_back(rigidBody->GetMinGlobal());
		maxMinList.push_back(rigidBody->GetMaxGlobal());
	}

	MyRigidBody* rigidBody = new MyRigidBody(maxMinList);

	//Use the rigidBody to get the size of our cube
	float maxVal = rigidBody->GetHalfWidth().x;
	for (int i = 0; i < 3; i++)
	{
		if (maxVal < rigidBody->GetHalfWidth()[i])
			maxVal = rigidBody->GetHalfWidth()[i];
	}
	mCenter = rigidBody->GetCenterLocal();//Setting our center value
	SafeDelete(rigidBody);//The rigidBody is no longer needed so we can remove it

	//Setting the size of our cube
	mSize = maxVal * 2.0f;
	mMin = mCenter - vector3(maxVal);
	mMax = mCenter + vector3(maxVal);
	mOctantCount++;
	ConstructTree(mMaxLevel);
}
Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();

	//Setting the size of our cube
	mCenter = a_v3Center;
	mSize = a_fSize;
	mMax = mCenter + (vector3(mSize) / 2.0f);
	mMin = mCenter - (vector3(mSize) / 2.0f);

	//Increase the octant count and set the ID
	mOctantCount++;
	mID = mOctantCount;
}
Simplex::MyOctant::~MyOctant(void)
{
	Release();
}

//Public Methods
bool Simplex::MyOctant::IsColliding(uint index)
{
	bool colliding = true;
	if (index >= mEntityMngr->GetEntityCount() || index < 0)
		return false;

	//Get the rigid body from the entity to get its bounds
	MyRigidBody* rigidBody = mEntityMngr->GetEntity(index)->GetRigidBody();
	vector3 otherMin = rigidBody->GetCenterGlobal() - rigidBody->GetHalfWidth();
	vector3 otherMax = rigidBody->GetCenterGlobal() + rigidBody->GetHalfWidth();

	//Used the MyRigidBody.cpp as a guideline. Use the min and max bounds to check for collision
	if (mMax.x < otherMin.x)
		colliding = false;
	if (mMin.x > otherMax.x)
		colliding = false;

	if (mMax.y < otherMin.y)
		colliding = false;
	if (mMin.y > otherMax.y)
		colliding = false;

	if (mMax.z < otherMin.z)
		colliding = false;
	if (mMin.z > otherMax.z)
		colliding = false;

	return colliding;
}
void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	if (mID == a_nIndex)//If the ID is equal to the index, only display that section
	{
		mMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, mCenter) * glm::scale(vector3(mSize)), a_v3Color, RENDER_WIRE);
		return;
	}

	for (uint i = 0; i < mChildren; i++)//Otherwise loop until you have the ID to draw
	{
		mChildArray[i]->Display(a_nIndex);
	}
}	
void Simplex::MyOctant::Display(vector3 a_v3Color)
{
	//Displays the entire octree
	for (uint i = 0; i < mChildren; i++)
	{
		mChildArray[i]->Display(a_v3Color);
	}

	mMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, mCenter) * glm::scale(vector3(mSize)), a_v3Color, RENDER_WIRE);
}
void Simplex::MyOctant::ClearEntityList(void)
{
	//Clearing every entity list from every child
	for (uint i = 0; i < mChildren; i++)
	{
		mChildArray[i]->ClearEntityList();
	}

	mEntityList.clear();//Clearing this entity list
}
void Simplex::MyOctant::Subdivide(void)
{
	//Return if were at the max level or if the children does not equal 0
	if (mLevel >= mMaxLevel || mChildren != 0)
		return;

	mChildren = 8;
	float fSize = mSize / 2.0f;

	vector3 cubeCenters = mCenter - (fSize / 2.f);

	//Back cubes
	mChildArray[0] = new MyOctant(cubeCenters, fSize);
	mChildArray[1] = new MyOctant(vector3(cubeCenters.x + fSize, cubeCenters.y,			cubeCenters.z), fSize);
	mChildArray[2] = new MyOctant(vector3(cubeCenters.x + fSize, cubeCenters.y + fSize, cubeCenters.z), fSize);
	mChildArray[3] = new MyOctant(vector3(cubeCenters.x,		 cubeCenters.y + fSize, cubeCenters.z), fSize);

	//Front cubes
	mChildArray[4] = new MyOctant(vector3(cubeCenters.x,		 cubeCenters.y,			cubeCenters.z + fSize), fSize);
	mChildArray[5] = new MyOctant(vector3(cubeCenters.x + fSize, cubeCenters.y,			cubeCenters.z + fSize), fSize);
	mChildArray[6] = new MyOctant(vector3(cubeCenters.x + fSize, cubeCenters.y + fSize, cubeCenters.z + fSize), fSize);
	mChildArray[7] = new MyOctant(vector3(cubeCenters.x,		 cubeCenters.y + fSize, cubeCenters.z + fSize), fSize);


	for (uint i = 0; i < mChildren; i++)
	{
		//Setting the parents, levels, and roots of these subdivisions
		mChildArray[i]->mParent = this;
		mChildArray[i]->mLevel = mLevel + 1;
		mChildArray[i]->mRoot = mRoot;

		//If there are enough entities, then continue to subdivide
		if (mChildArray[i]->ContainsMoreThan(mIdealEntityCount))
			mChildArray[i]->Subdivide();
	}
}
bool Simplex::MyOctant::IsLeaf(void)
{
	if (mChildren == 0)//If it has no children, it's a leaf
		return true;

	return false;
}
bool Simplex::MyOctant::ContainsMoreThan(uint numEntities)
{
	//Check if a section contains more than the specified number of entities
	uint numInSection = 0;
	for (uint i = 0; i < mEntityMngr->GetEntityCount(); i++)
	{
		if (IsColliding(i))
			numInSection++;
	}

	if (numInSection > numEntities)//If so, return true
		return true;

	return false;
}
void Simplex::MyOctant::KillBranches(void)
{
	if (IsLeaf())//If this is a leaf there are no branches to kill
		return;

	//Remove all branches from children that have branches
	for (uint i = 0; i < mChildren; i++)
	{
		mChildArray[i]->KillBranches();
		SafeDelete(mChildArray[i]);
	}

	mChildren = 0;
}
void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{
	//If this is not the base level then return
	if (mLevel != 0)
		return;

	mMaxLevel = a_nMaxLevel;//Reset the max
	mOctantCount = 1;//Set the octant count to one octant
	mEntityList.clear();//Remove all entites from the list

	////Remove all the branches in the tree and clear the child list to reset
	KillBranches();
	mChildList.clear();

	//Start subdividing the tree
	if (ContainsMoreThan(mIdealEntityCount))
		Subdivide();

	AssignIDtoEntity();
	ConstructList();
}
void Simplex::MyOctant::AssignIDtoEntity(void)
{
	for (uint i = 0; i < mChildren; i++)
	{
		mChildArray[i]->AssignIDtoEntity();
	}

	//If this is a leaf, check to see if there are any colliding entites and push them back to this dimension
	if (IsLeaf())
	{
		for (uint i = 0; i < mEntityMngr->GetEntityCount(); i++)
		{
			if (IsColliding(i))
			{
				mEntityList.push_back(i);
				mEntityMngr->AddDimension(i, mID);
			}
		}
	}
}

//Private Methods
void Simplex::MyOctant::Release(void)
{
	//If this is the root, destroy all the branches
	if (this == mRoot)
		KillBranches();

	mEntityList.clear();
	mChildList.clear();
	mSize = 0;
	mChildren = 0;
}
void Simplex::MyOctant::Init(void)
{
	//Getting instances of managers
	mMeshMngr = MeshManager::GetInstance();
	mEntityMngr = MyEntityManager::GetInstance();

	mChildren = 0;
	mSize = 0.f;
	mLevel = 0;

	//Setting all octree elements to nullptr
	mRoot = nullptr;
	mParent = nullptr;
	for (int i = 0; i < 8; i++)
	{
		mChildArray[i] = nullptr;
	}
}
void Simplex::MyOctant::ConstructList(void)
{
	for (uint i = 0; i < mChildren; i++)
		mChildArray[i]->ConstructList();
	
	if (mEntityList.size() > 0)
		mRoot->mChildList.push_back(this);

}