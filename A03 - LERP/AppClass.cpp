#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "Michael Howlett - mah9882@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits
	float fRadius = 0.95f; //initial orbit radius

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		
		std::vector<vector3> m_stopsList; //make a list 
		float degrees = PI * 2.0f / i; //get degrees of arc
		for (uint j = 0; j < i; j++)
		{
			vector3 coord(0.0f, 0.0f, 0.0f);

			coord.x = cos(j * degrees) * fRadius;//get x coordinates
			coord.y = sin(j * degrees) * fRadius;//get y coordinates

			m_stopsList.push_back(coord);//add coordinates to the stops list
		}
		m_listOfLists.push_back(m_stopsList); //add a list to the list of lists of lists

		fSize += 0.5f; //increment the size for the next orbit
		fRadius += 0.5f; //increment radius for next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
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

	//get a timer
	static float fTimer = 0;
	static uint uClock = m_pSystem->GenClock(); //generate new clock for timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get deltatime for the clock

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);

	//set a timer between stops
	float fTimeBetweenStops = 2.0f;
	float fPercent = MapValue(fTimer, 0.0f, fTimeBetweenStops, 0.0f, 1.0f);

	//initialize last stop and next stop
	vector3 v3stopLast;
	vector3 v3stopNext;

	//set route
	static uint route = 0;

	//get routes per iteration
	std::vector<std::vector<uint>> routesPerIteration;
	for (uint i = 0; i < m_uOrbits; i++)
	{
		std::vector<uint> routes;
		for (uint j = 0; j < m_listOfLists[i].size(); j++)
		{
			routes.push_back(j);
		}
		routesPerIteration.push_back(routes);
	}
	
	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 90.0f, AXIS_X));
		
		//dynamically allocate numbers of routes
		for (uint j = 0; j < routesPerIteration[i].size(); j++)
		{
			route = routesPerIteration[i][j]; //set route

			//set stops for lerp
			v3stopLast = m_listOfLists[i][route];
			v3stopNext = m_listOfLists[i][(route + 1) % m_listOfLists[i].size()];

			//calculate the current position
			vector3 v3CurrentPos = glm::lerp(v3stopLast, v3stopNext, fPercent);
			matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

			//draw spheres
			m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
		}

		if (fPercent >= 1.0f)
		{
			//reset timer
			fTimer = m_pSystem->GetDeltaTime(uClock);
		}
	}

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
	//release GUI
	ShutdownGUI();
}