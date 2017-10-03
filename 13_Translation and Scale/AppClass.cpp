#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	//m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));
	
	//init the mesh
	m_pMesh = new MyMesh[46];
	//m_pMesh->GenerateSphere(1.0f, 5, C_WHITE);

	for (uint i = 0; i < 46; i++)
	{
		m_pMesh[i].GenerateCube(1.0f, C_BLACK);
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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
		
	/*matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(2.0f,2.0f,2.0f));
	static float value = 0.0f;
	matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(value, 2.0f, 3.0f));
	value += 0.01f;*/

	//matrix4 m4Model = m4Translate * m4Scale;
	static matrix4 m4Model;
	m4Model *= glm::translate(IDENTITY_M4, vector3(0.01f, 0.0f, 0.0f));

	m_pMesh[0].Render(m4Projection, m4View, m4Model * glm::translate(m4Model,vector3(3,0,0)));
	m_pMesh[1].Render(m4Projection, m4View, m4Model * glm::translate(m4Model,vector3(4,0,0)));
	m_pMesh[2].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(6, 0, 0)));
	m_pMesh[3].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(7, 0, 0)));
	m_pMesh[4].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(0, 1, 0)));
	m_pMesh[5].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(2, 1, 0)));
	m_pMesh[6].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(8, 1, 0)));
	m_pMesh[7].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(10, 1, 0)));
	m_pMesh[8].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(0, 2, 0)));
	m_pMesh[9].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(2, 2, 0)));
	m_pMesh[10].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(3, 2, 0)));
	m_pMesh[11].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(4, 2, 0)));
	m_pMesh[12].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(5, 2, 0)));
	m_pMesh[13].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(6, 2, 0)));
	m_pMesh[14].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(7, 2, 0)));
	m_pMesh[15].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(8, 2, 0)));
	m_pMesh[16].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(10, 2, 0)));
	m_pMesh[17].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(0, 3, 0)));
	m_pMesh[18].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(1, 3, 0)));
	m_pMesh[19].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(2, 3, 0)));
	m_pMesh[20].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(3, 3, 0)));
	m_pMesh[21].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(4, 3, 0)));
	m_pMesh[22].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(5, 3, 0)));
	m_pMesh[23].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(6, 3, 0)));
	m_pMesh[24].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(7, 3, 0)));
	m_pMesh[25].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(8, 3, 0)));
	m_pMesh[26].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(9, 3, 0)));
	m_pMesh[27].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(10, 3, 0)));
	m_pMesh[28].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(1, 4, 0)));
	m_pMesh[29].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(2, 4, 0)));
	m_pMesh[30].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(4, 4, 0)));
	m_pMesh[31].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(5, 4, 0)));
	m_pMesh[32].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(6, 4, 0)));
	m_pMesh[33].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(8, 4, 0)));
	m_pMesh[34].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(9, 4, 0)));
	m_pMesh[35].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(2, 5, 0)));
	m_pMesh[36].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(3, 5, 0)));
	m_pMesh[37].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(4, 5, 0)));
	m_pMesh[38].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(5, 5, 0)));
	m_pMesh[39].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(6, 5, 0)));
	m_pMesh[40].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(7, 5, 0)));
	m_pMesh[41].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(8, 5, 0)));
	m_pMesh[42].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(3, 6, 0)));
	m_pMesh[43].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(7, 6, 0)));
	m_pMesh[44].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(2, 7, 0)));
	m_pMesh[45].Render(m4Projection, m4View, m4Model * glm::translate(m4Model, vector3(8, 7, 0)));
	
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
	delete[] m_pMesh;

	//release GUI
	ShutdownGUI();
}