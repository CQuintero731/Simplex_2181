#include "AppClass.h"
void Application::InitVariables(void)
{
	//Make MyMesh object

	for (int i = 0; i < 46; i++)
	{
		MyMesh* mesh = new MyMesh();
		mesh->GenerateCube(1.0f, C_PURPLE);

		cubes.push_back(mesh);
	}

}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//Used to keep track of Indices
	int index = 0;

	//Drawing out the space invader row by row. i value in for loops used as a translation amount
	//Row 1
	for (int i = -13; i < -6; i+=6)
	{
		cubes[index]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(i + translation, 7, 0)));
		index++;
	}
	//Row 2
	for (int i = -12; i < -7; i += 4)
	{
		cubes[index]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(i + translation, 6, 0)));
		index++;
	}
	//Row 3
	for (int i = -13; i < -6; i++)
	{
		cubes[index]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(i + translation, 5, 0)));
		index++;
	}
	//Row 4
	for (int i = -14; i < -5; i ++)
	{
		if (i == -12 || i == -8)
		{ i++; }

		cubes[index]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(i + translation, 4, 0)));
		index++;
	}
	//Row 5
	for (int i = -15; i < -4; i++)
	{
		cubes[index]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(i + translation, 3, 0)));
		index++;
	}
	//Row 6
	for (int i = -15; i < -4; i++)
	{
		if (i == -14 || i == -6)
		{ i++; }

		cubes[index]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(i + translation, 2, 0)));
		index++;
	}
	//Row 7
	for (int i = -15; i < -4; i += 2)
	{
		if(i == -11)
		{ i += 4; }
		
		cubes[index]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(i + translation, 1, 0)));
		index++;
	}
	//Row 8
	for (int i = -12; i < -7; i++)
	{
		if(i == -10)
		{ i++; }
		cubes[index]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(i + translation, 0, 0)));
		index++;
	}

	//Increase the translation so the space invader moves across the screen
	translation += .1f;
		
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
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
	for (int i = 0; i < 46; i++)
	{
		SafeDelete(cubes[i]);
	}
	
	//release GUI
	ShutdownGUI();
}