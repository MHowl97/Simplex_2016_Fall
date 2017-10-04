#include "MyMesh.h"
#include <math.h>
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	vector3 origin(0.0f, 0.0f, 0.0f);
	vector3 tip(0.0f, a_fHeight * 0.5f, 0.0f);
	vector3 base(0.0f, -a_fHeight * 0.5f, 0.0f);

	float degrees = PI * 2.0f / a_nSubdivisions;

	std::vector<vector3> coordinates;

	for (uint i = 0; i <= a_nSubdivisions; i++)
	{
		vector3 coord(0.0f, base.y, 0.0f);

		coord.x = cos(i * degrees) * a_fRadius;
		coord.z = sin(i * degrees) * a_fRadius;

		coordinates.push_back(coord);
	}

	for (uint i = 0; i <= a_nSubdivisions; i++)
	{
		if (i + 1 == coordinates.size())
		{
			break;
		}

		AddTri(coordinates[i], coordinates[i + 1], tip);
	}

	for (uint i = 0; i <= a_nSubdivisions; i++)
	{
		if (i + 1 == coordinates.size())
		{
			break;
		}

		AddTri(coordinates[i], base, coordinates[i+1]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	vector3 origin(0.0f, 0.0f, 0.0f);
	vector3 top(0.0f, a_fHeight * 0.5f, 0.0f);
	vector3 base(0.0f, -a_fHeight * 0.5f, 0.0f);

	float degrees = PI * 2.0f / a_nSubdivisions;

	std::vector<vector3> coordinates;
	
	//get coordinates on circle
	for (uint i = 0; i <= a_nSubdivisions; i++)
	{
		vector3 coord(0.0f, 0.0f, 0.0f);

		coord.x = cos(i * degrees) * a_fRadius;
		coord.z = sin(i * degrees) * a_fRadius;

		coordinates.push_back(coord);
	}

	//create quads for sides
	for (uint i = 0; i <= a_nSubdivisions; i++)
	{
		if (i + 1 == coordinates.size())
		{
			break;
		}

		vector3 topLeft = coordinates[i];
		topLeft.y = top.y;
		vector3 topRight = coordinates[i + 1];
		topRight.y = top.y;
		vector3 bottomLeft = coordinates[i];
		bottomLeft.y = base.y;
		vector3 bottomRight = coordinates[i + 1];
		bottomRight.y = base.y;

		AddQuad(bottomRight, bottomLeft, topRight, topLeft);
	}

	//bottom
	for (uint i = 0; i <= a_nSubdivisions; i++)
	{
		if (i + 1 == coordinates.size())
		{
			break;
		}

		coordinates[i].y = base.y;
		coordinates[i + 1].y = base.y;
		AddTri(coordinates[i], coordinates[i + 1], base);
	}

	//top
	for (uint i = 0; i <= a_nSubdivisions; i++)
	{
		if (i + 1 == coordinates.size())
		{
			break;
		}

		coordinates[i].y = top.y;
		coordinates[i + 1].y = top.y;
		AddTri(coordinates[i], top, coordinates[i + 1]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	vector3 origin(0.0f, 0.0f, 0.0f);
	vector3 top(0.0f, a_fHeight * 0.5f, 0.0f);
	vector3 base(0.0f, -a_fHeight * 0.5f, 0.0f);

	float degrees = PI * 2.0f / a_nSubdivisions;

	std::vector<vector3> coordinatesInner;
	std::vector<vector3> coordinatesOuter;

	//get coordinates on inner circle
	for (uint i = 0; i <= a_nSubdivisions; i++)
	{
		vector3 coord(0.0f, 0.0f, 0.0f);

		coord.x = cos(i * degrees) * a_fInnerRadius;
		coord.z = sin(i * degrees) * a_fInnerRadius;

		coordinatesInner.push_back(coord);

		coord.x = cos(i * degrees) * a_fOuterRadius;
		coord.z = sin(i * degrees) * a_fOuterRadius;

		coordinatesOuter.push_back(coord);
	}

	//create quads for outer sides
	for (uint i = 0; i <= a_nSubdivisions; i++)
	{
		if (i + 1 == coordinatesOuter.size())
		{
			break;
		}

		vector3 topLeft = coordinatesOuter[i];
		topLeft.y = top.y;
		vector3 topRight = coordinatesOuter[i + 1];
		topRight.y = top.y;
		vector3 bottomLeft = coordinatesOuter[i];
		bottomLeft.y = base.y;
		vector3 bottomRight = coordinatesOuter[i + 1];
		bottomRight.y = base.y;

		AddQuad(bottomRight, bottomLeft, topRight, topLeft);
	}

	//create quads for inner sides
	for (uint i = 0; i <= a_nSubdivisions; i++)
	{
		if (i + 1 == coordinatesInner.size())
		{
			break;
		}

		vector3 topLeft = coordinatesInner[i];
		topLeft.y = top.y;
		vector3 topRight = coordinatesInner[i + 1];
		topRight.y = top.y;
		vector3 bottomLeft = coordinatesInner[i];
		bottomLeft.y = base.y;
		vector3 bottomRight = coordinatesInner[i + 1];
		bottomRight.y = base.y;

		AddQuad(bottomLeft, bottomRight, topLeft, topRight);
	}

	
	//bottom
	for (uint i = 0; i <= a_nSubdivisions; i++)
	{
		if (i + 1 == coordinatesOuter.size())
		{
			break;
		}

		coordinatesOuter[i].y = base.y;
		coordinatesOuter[i + 1].y = base.y;
		coordinatesInner[i].y = base.y;
		coordinatesInner[i + 1].y = base.y;
		
		AddQuad(coordinatesOuter[i], coordinatesOuter[i + 1], coordinatesInner[i], coordinatesInner[i + 1]);
	}

	//top
	for (uint i = 0; i <= a_nSubdivisions; i++)
	{
		if (i + 1 == coordinatesOuter.size())
		{
			break;
		}

		coordinatesOuter[i].y = top.y;
		coordinatesOuter[i + 1].y = top.y;
		coordinatesInner[i].y = top.y;
		coordinatesInner[i + 1].y = top.y;

		AddQuad(coordinatesOuter[i + 1], coordinatesOuter[i], coordinatesInner[i + 1], coordinatesInner[i]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	vector3 origin(0.0f, 0.0f, 0.0f);

	

	 
	std::vector<vector3> coordinatesV;
	std::vector<vector3> coordinatesN;

	float dpsi = 2.0f * PI / a_nSubdivisionsA;
	float dphi = 2.0f * PI / a_nSubdivisionsB;
	float psi = 0.0f;

	for (uint i = 0; i <= a_nSubdivisionsB; i++)
	{
		float cpsi = cos(psi);
		float spsi = sin(psi);
		float phi = 0.0f;

		for (uint j = 0; j <= a_nSubdivisionsA; j++)
		{
			uint offset = 3 * (j * a_nSubdivisionsA + i);
			float cphi = cos(phi);
			float sphi = sin(phi);

			vector3 coordV(0.0f, 0.0f, 0.0f);
			vector3 coordN(0.0f, 0.0f, 0.0f);

			coordV.x = cpsi * (a_fOuterRadius + cphi * a_fInnerRadius);
			coordV.y = spsi * (a_fOuterRadius + cphi * a_fInnerRadius);
			coordV.z = sphi * a_fInnerRadius;
			coordN.x = cpsi * cphi;
			coordN.y = spsi * cphi;
			coordN.z = sphi;

			coordinatesV.push_back(coordV);
			coordinatesN.push_back(coordN);

			phi += dphi;
		}
		psi += dpsi;
	}

	for (uint i = 0; i <= a_nSubdivisionsA; i++)
	{
		for (uint j = 0; i <= a_nSubdivisionsB; j++)
		{
			if (j + 1 == coordinatesN.size())
			{
				break;
			}

			AddQuad(coordinatesV[j], coordinatesV[j+1], coordinatesN[j], coordinatesN[j+1]);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code
	vector3 origin(0.0f, 0.0f, 0.0f);
	vector3 tip(0.0f, a_fRadius, 0.0f);
	vector3 base(0.0f, -a_fRadius, 0.0f);

	float degrees = PI * 2.0f / 4;

	std::vector<vector3> coordinates;

	for (uint i = 0; i <= 4; i++)
	{
		vector3 coord(0.0f, 0.0f, 0.0f);

		coord.x = cos(i * degrees) * a_fRadius;
		coord.z = sin(i * degrees) * a_fRadius;

		coordinates.push_back(coord);
	}

	for (uint i = 0; i <= 4; i++)
	{
		if (i + 1 == coordinates.size())
		{
			break;
		}

		subdivide(coordinates[i], tip, coordinates[i + 1], a_nSubdivisions);
	}

	for (uint i = 0; i <= 4; i++)
	{
		if (i + 1 == coordinates.size())
		{
			break;
		}

		subdivide(coordinates[i], coordinates[i + 1], base, a_nSubdivisions);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}

void MyMesh::subdivide(vector3 v1, vector3 v2, vector3 v3, int level)
{
	if (level == 0)
	{
		AddTri(v1, v2, v3);
	}
	else
	{
		//first edge
		vector3 mid1(0.5f * (v1.x + v2.x), 0.5f * (v1.y + v2.y), 0.5f * (v1.z + v2.z));
		float s = 1.0f / sqrt(mid1.x * mid1.x + mid1.y * mid1.y + mid1.z * mid1.z);
		mid1 *= s;

		//second edge
		vector3 mid2(0.5f * (v1.x + v3.x), 0.5f * (v1.y + v3.y), 0.5f * (v1.z + v3.z));
		s = 1.0f / sqrt(mid2.x * mid2.x + mid2.y * mid2.y + mid2.z * mid2.z);
		mid2 *= s;

		//third edge
		vector3 mid3(0.5f * (v2.x + v3.x), 0.5f * (v2.y + v3.y), 0.5f * (v2.z + v3.z));
		s = 1.0f / sqrt(mid3.x * mid3.x + mid3.y * mid3.y + mid3.z * mid3.z);
		mid3 *= s;

		subdivide(v1, mid1, mid2, level - 1);
		subdivide(mid1, v2, mid3, level - 1);
		subdivide(mid2, mid3, v3, level - 1);
		subdivide(mid1, mid3, mid2, level - 1);
	}
}