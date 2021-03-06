#include "MyRigidBody.h"
using namespace Simplex;
//Accessors
bool MyRigidBody::GetVisible(void) { return m_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColor(void) { return m_v3Color; }
void MyRigidBody::SetColor(vector3 a_v3Color) { m_v3Color = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix) { m_m4ToWorld = a_m4ModelMatrix; }
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisible = true;

	m_fRadius = 0.0f;

	m_v3Color = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& other)
{
	std::swap(m_pMeshMngr , other.m_pMeshMngr);
	std::swap(m_bVisible , other.m_bVisible);

	std::swap(m_fRadius, other.m_fRadius);

	std::swap(m_v3Color , other.m_v3Color);

	std::swap(m_v3Center , other.m_v3Center);
	std::swap(m_v3MinL , other.m_v3MinL);
	std::swap(m_v3MaxL , other.m_v3MaxL);

	std::swap(m_v3MinG , other.m_v3MinG);
	std::swap(m_v3MaxG , other.m_v3MaxG);

	std::swap(m_v3HalfWidth , other.m_v3HalfWidth);

	std::swap(m_m4ToWorld , other.m_m4ToWorld);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();

	if (a_pointList.size() == 0)
		return;

	m_v3MinL.x = a_pointList[0].x;
	m_v3MinL.y = a_pointList[0].y;
	m_v3MinL.z = a_pointList[0].z;

	m_v3MaxL.x = a_pointList[0].x;
	m_v3MaxL.y = a_pointList[0].y;
	m_v3MaxL.z = a_pointList[0].z;

	for (uint i = 1; i < a_pointList.size(); i++)
	{
		if (a_pointList[i].x < m_v3MinL.x)
			m_v3MinL.x = a_pointList[i].x;
		else if (a_pointList[i].x > m_v3MaxL.x)
			m_v3MaxL.x = a_pointList[i].x;

		if (a_pointList[i].y < m_v3MinL.y)
			m_v3MinL.y = a_pointList[i].y;
		else if (a_pointList[i].y > m_v3MaxL.y)
			m_v3MaxL.y = a_pointList[i].y;
		
		if (a_pointList[i].z < m_v3MinL.z)
			m_v3MinL.z = a_pointList[i].z;
		else if (a_pointList[i].z > m_v3MaxL.z)
			m_v3MaxL.z = a_pointList[i].z;
	}

	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.f;

	m_fRadius = glm::distance(m_v3Center, m_v3MaxL);

	//vector3 v3Size;
	//v3Size.x = glm::distance(vector3(m_v3MaxL.x, 0.f, 0.f), vector3(m_v3MinL.x, 0.f, 0.f));
	//v3Size.y = glm::distance(vector3(0.f, m_v3MaxL.y, 0.f), vector3(0.f, m_v3MinL.y, 0.f));
	//v3Size.z = glm::distance(vector3(0.f, 0.f, m_v3MaxL.z), vector3(0.f, 0.f, m_v3MinL.z));

	//Does the same as the three distances
	//v3Size = m_v3MaxL - m_v3MinL;

	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.f;
}
MyRigidBody::MyRigidBody(MyRigidBody const& other)
{
	m_pMeshMngr = m_pMeshMngr;
	m_bVisible = m_bVisible;

	m_fRadius = m_fRadius;

	m_v3Color = m_v3Color;

	m_v3Center = m_v3Center;
	m_v3MinL = m_v3MinL;
	m_v3MaxL = m_v3MaxL;

	m_v3MinG = m_v3MinG;
	m_v3MaxG = m_v3MaxG;

	m_v3HalfWidth = m_v3HalfWidth;

	m_m4ToWorld = m_m4ToWorld;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		MyRigidBody temp(other);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody(){Release();};

//--- Non Standard Singleton Methods
void MyRigidBody::AddToRenderList(void)
{
	if (!m_bVisible)
		return;

	//m_pMeshMngr->AddWireSphereToRenderList(m_m4ToWorld * glm::translate(m_v3Center) * glm::scale(vector3(m_fRadius)), m_v3Color, RENDER_SOLID);
	m_pMeshMngr->AddWireCubeToRenderList(m_m4ToWorld * glm::translate(m_v3Center) * glm::scale(m_v3HalfWidth * 2), m_v3Color, RENDER_WIRE);
}
bool MyRigidBody::IsColliding(MyRigidBody* const other)
{
	//return glm::distance(m_v3Center, other->m_v3Center) < (m_fRadius + other->m_fRadius);

	vector3 center1 = m_m4ToWorld * vector4(m_v3Center, 1.f);
	vector3 center2 = other->m_m4ToWorld * vector4(other->m_v3Center, 1.f);

	//return glm::distance(center1, center2) < (m_fRadius + other->m_fRadius)

	vector3 max1 = m_m4ToWorld * vector4(m_v3MaxL, 1.f);
	vector3 min1 = m_m4ToWorld * vector4(m_v3MinL, 1.f);

	vector3 max2 = other->m_m4ToWorld * vector4(other->m_v3MaxL, 1.f);
	vector3 min2 = other->m_m4ToWorld * vector4(other->m_v3MinL, 1.f);

	if (max1.x < min2.x || min1.x > max2.x)
		return false;
	if (max1.y < min2.y || min1.y > max2.y)
		return false;
	if (max1.z < min2.z || min1.z > max2.z)
		return false;

	return true;
}