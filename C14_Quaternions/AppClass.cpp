#include "AppClass.h"
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 10.0f, ZERO_V3, AXIS_Y);

	//initializing the model
	m_pModel = new Simplex::Model();

	//Load a model
	m_pModel->Load("Minecraft\\Steve.obj");

	m_pMesh = new MyMesh;
	m_pMesh->GenerateCube(2, C_BLUE_CORNFLOWER);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Get a timer
	static uint uClock = m_pSystem->GenClock();
	float fTimer = m_pSystem->GetTimeSinceStart(uClock);
	float fDeltaTime = m_pSystem->GetDeltaTime(uClock);

#pragma region SLERP
	if (false)
	{
		quaternion q1;
		quaternion q2 = glm::angleAxis(glm::radians(180.f), vector3(0.0f, 0.0f, 1.0f));
		float fPercentage = MapValue(fTimer, 0.0f, 2.0f, 0.0f, 1.0f);
		quaternion qSLERP = glm::mix(q1, q2, fPercentage);
		m_m4Steve = glm::toMat4(qSLERP);
	}
#pragma endregion
#pragma region translate vector orientation into a matrix
	if (false)
	{
		matrix4 m4OrientX = glm::rotate(IDENTITY_M4, glm::radians(m_v3Orientation.x), vector3(1.0f, 0.0f, 0.0f));
		matrix4 m4OrientY = glm::rotate(IDENTITY_M4, glm::radians(m_v3Orientation.y), vector3(0.0f, 1.0f, 0.0f));
		matrix4 m4OrientZ = glm::rotate(IDENTITY_M4, glm::radians(m_v3Orientation.z), vector3(0.0f, 0.0f, 1.0f));

		matrix4 m4Orientation = m4OrientX * m4OrientY * m4OrientZ;
		m_m4Steve = glm::toMat4(m_qOrientation);
	}
#pragma endregion
#pragma region orientation using quaternions
	if (true)
	{
		m_m4Steve = glm::toMat4(m_qOrientation);
	}
#pragma endregion
	
	//Attach the model matrix that takes me from the world coordinate system
	m_pModel->SetModelMatrix(m_m4Steve);

	//Send the model to render list
	m_pModel->AddToRenderList();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();
	
	matrix4 model = glm::translate(m_v3Orientation * 0.01f);
	matrix4 view = m_pCameraMngr->GetViewMatrix();
	matrix4 proj = m_pCameraMngr->GetProjectionMatrix();

	//Orthographic view
	//proj = glm::ortho(0.f, 2.f, 0.f, 2.f, 0.01f, 20.f);

	//Perspective view
	float fov = 45.f;//Field of view
	float aspect = static_cast<float>(m_pSystem->GetWindowWidth()) / static_cast<float>(m_pSystem->GetWindowHeight());//m_pSystem->GetWindowRatio();//Aspect ratio
	float nearPlane = 0.01f;//Distance to near clip
	float farPlane = 20.f;//Distance to far clip
	proj = glm::perspective(fov, aspect, nearPlane, farPlane);

	vector3 pos = vector3(0.f, 0.f , 10.f);
	vector3 target;
	vector3 up = vector3(0.f , 1.f, 0.f);

	view = glm::lookAt(pos, target, up);

	m_pMesh->Render(proj, view, model);

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//Use these for homework when you create camera
	m_pCameraMngr->SetProjectionMatrix(proj);
	m_pCameraMngr->SetViewMatrix(view);

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release model
	SafeDelete(m_pModel);
	SafeDelete(m_pMesh);
	//release GUI
	ShutdownGUI();
}
