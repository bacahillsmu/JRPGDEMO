#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/VertexLit.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

CPUMesh::CPUMesh()
{

}

CPUMesh::~CPUMesh()
{

}

void CPUMesh::SetColor( const Rgba& color )
{
	m_stamp.color = color;
}

void CPUMesh::SetUV( const Vec2& uv )
{
	m_stamp.uv = uv;
}

void CPUMesh::SetNormal( const Vec3& normal )
{
	m_stamp.normal = normal;
}

void CPUMesh::SetTangent( const Vec3& tangent )
{
	m_stamp.tangent = tangent;
}

void CPUMesh::SetBiTangent( const Vec3& biTangent )
{
	m_stamp.bitangent = biTangent;
}

uint CPUMesh::AddVertex( const VertexMaster& vertexMaster )
{
	m_stamp = vertexMaster;
	m_vertices.push_back(m_stamp);

	return (uint)m_vertices.size() - 1;
}

uint CPUMesh::AddVertex( const Vec3& position )
{
	m_stamp.position = position;
	m_vertices.push_back(m_stamp);

	return (uint)m_vertices.size() - 1;
}

void CPUMesh::AddIndexedTriangle( uint index0, uint index1, uint index2 )
{
	ASSERT_OR_DIE( index0 < m_vertices.size(), "Index0 is more than the size of the vertices." );
	ASSERT_OR_DIE( index1 < m_vertices.size(), "Index1 is more than the size of the vertices." );
	ASSERT_OR_DIE( index2 < m_vertices.size(), "Index2 is more than the size of the vertices." );

	m_indices.push_back(index0);
	m_indices.push_back(index1);
	m_indices.push_back(index2);
}

void CPUMesh::AddIndexedQuad( uint topLeft, uint topRight, uint bottomLeft, uint bottomRight )
{
	m_indices.push_back(topLeft);
	m_indices.push_back(bottomLeft);
	m_indices.push_back(topRight);

	m_indices.push_back(bottomLeft);
	m_indices.push_back(bottomRight);
	m_indices.push_back(topRight);
}

uint CPUMesh::GetVertexCount() const
{
	return (uint)m_vertices.size();
}

uint CPUMesh::GetIndexCount() const
{
	return (uint)m_indices.size();
}

const BufferLayout* CPUMesh::GetLayout() const
{
	return m_layout;
}

const VertexMaster* CPUMesh::GetVertices() const
{
	return m_vertices.data();
}

void CPUMesh::SetLayout( const BufferLayout* bufferLayout )
{
	m_layout = bufferLayout;
}

void CPUMesh::Clear()
{
	m_vertices.clear();
	m_indices.clear();
}

void CPUMeshAddQuad( CPUMesh* out, AABB2 quad )
{
	out->Clear();

	out->SetColor(Rgba::WHITE);

	// 0 ----- 2
	// |    /  |
	// |   /   |
	// |  /    |
	// 1 ----- 3

	out->SetNormal(Vec3::FORWARD);
	out->SetTangent(Vec3::RIGHT);
	out->SetBiTangent(Vec3::UP);
	
	out->SetUV(Vec2(0.0f, 0.0f));
	out->AddVertex(Vec3(quad.GetTopLeft().x, quad.GetTopLeft().y, 0.0f));

	out->SetUV(Vec2(0.0f, 1.0f));
	out->AddVertex(Vec3(quad.GetBottomLeft().x, quad.GetBottomLeft().y, 0.0f));

	out->SetUV(Vec2(1.0f, 0.0f));
	out->AddVertex(Vec3(quad.GetTopRight().x, quad.GetTopRight().y, 0.0f));

	out->SetUV(Vec2(1.0f, 1.0f));
	out->AddVertex(Vec3(quad.GetBottomRight().x, quad.GetBottomRight().y, 0.0f));

	out->AddIndexedQuad(0, 2, 1, 3);
}

void CPUMeshAddCube( CPUMesh* out, AABB3 box )
{
	out->Clear();
	out->SetColor(Rgba::WHITE);

	Vec3 mins = box.mins;
	Vec3 maxs = box.maxs;

	// Front
	out->SetNormal(Vec3::FORWARD);
	out->SetTangent(Vec3::RIGHT);
	out->SetBiTangent(Vec3::UP);

	out->SetUV(Vec2(0.0f, 0.0f));
	out->AddVertex(Vec3(mins.x, maxs.y, mins.z));

	out->SetUV(Vec2(0.0f, 1.0f));
	out->AddVertex(Vec3(mins.x, mins.y, mins.z));

	out->SetUV(Vec2(1.0f, 0.0f));
	out->AddVertex(Vec3(maxs.x, maxs.y, mins.z));

	out->SetUV(Vec2(1.0f, 1.0f));
	out->AddVertex(Vec3(maxs.x, mins.y, mins.z));

	out->AddIndexedQuad(0, 2, 1, 3);

	// Back
	out->SetNormal(Vec3::BACK);
	out->SetTangent(Vec3::LEFT);
	out->SetBiTangent(Vec3::UP);

	out->SetUV(Vec2(0.0f, 0.0f));
	out->AddVertex(Vec3(maxs.x, maxs.y, maxs.z));

	out->SetUV(Vec2(0.0f, 1.0f));
	out->AddVertex(Vec3(maxs.x, mins.y, maxs.z));

	out->SetUV(Vec2(1.0f, 0.0f));
	out->AddVertex(Vec3(mins.x, maxs.y, maxs.z));

	out->SetUV(Vec2(1.0f, 1.0f));
	out->AddVertex(Vec3(mins.x, mins.y, maxs.z));

	out->AddIndexedQuad(4, 6, 5, 7);

	// Right
	out->SetNormal(Vec3::RIGHT);
	out->SetTangent(Vec3::FORWARD);
	out->SetBiTangent(Vec3::UP);

	out->SetUV(Vec2(0.0f, 0.0f));
	out->AddVertex(Vec3(maxs.x, maxs.y, mins.z));

	out->SetUV(Vec2(0.0f, 1.0f));
	out->AddVertex(Vec3(maxs.x, mins.y, mins.z));

	out->SetUV(Vec2(1.0f, 0.0f));
	out->AddVertex(Vec3(maxs.x, maxs.y, maxs.z));

	out->SetUV(Vec2(1.0f, 1.0f));
	out->AddVertex(Vec3(maxs.x, mins.y, maxs.z));

	out->AddIndexedQuad(8, 10, 9, 11);

	// Left
	out->SetNormal(Vec3::LEFT);
	out->SetTangent(Vec3::FORWARD);
	out->SetBiTangent(Vec3::UP);

	out->SetUV(Vec2(0.0f, 0.0f));
	out->AddVertex(Vec3(mins.x, maxs.y, maxs.z));

	out->SetUV(Vec2(0.0f, 1.0f));
	out->AddVertex(Vec3(mins.x, mins.y, maxs.z));

	out->SetUV(Vec2(1.0f, 0.0f));
	out->AddVertex(Vec3(mins.x, maxs.y, mins.z));

	out->SetUV(Vec2(1.0f, 1.0f));
	out->AddVertex(Vec3(mins.x, mins.y, mins.z));

	out->AddIndexedQuad(12, 14, 13, 15);

	// Top
	out->SetNormal(Vec3::UP);
	out->SetTangent(Vec3::RIGHT);
	out->SetBiTangent(Vec3::FORWARD);

	out->SetUV(Vec2(0.0f, 0.0f));
	out->AddVertex(Vec3(mins.x, maxs.y, maxs.z));

	out->SetUV(Vec2(0.0f, 1.0f));
	out->AddVertex(Vec3(mins.x, maxs.y, mins.z));

	out->SetUV(Vec2(1.0f, 0.0f));
	out->AddVertex(Vec3(maxs.x, maxs.y, maxs.z));

	out->SetUV(Vec2(1.0f, 1.0f));
	out->AddVertex(Vec3(maxs.x, maxs.y, mins.z));

	out->AddIndexedQuad(16, 18, 17, 19);

	// Bottom
	out->SetNormal(Vec3::DOWN);
	out->SetTangent(Vec3::RIGHT);
	out->SetBiTangent(Vec3::FORWARD);

	out->SetUV(Vec2(0.0f, 0.0f));
	out->AddVertex(Vec3(mins.x, mins.y, mins.z));

	out->SetUV(Vec2(0.0f, 1.0f));
	out->AddVertex(Vec3(mins.x, mins.y, maxs.z));

	out->SetUV(Vec2(1.0f, 0.0f));
	out->AddVertex(Vec3(maxs.x, mins.y, mins.z));

	out->SetUV(Vec2(1.0f, 1.0f));
	out->AddVertex(Vec3(maxs.x, mins.y, maxs.z));

	out->AddIndexedQuad(20, 22, 21, 23);
}

void CPUMeshAddUVSphere( CPUMesh* out, Vec3 center, float radius, uint wedges /*= 32*/, uint slices /*= 16 */ )
{
	uint uStep = wedges + 1;
	uint vStep = slices + 1;

	for(uint vIndex = 0; vIndex < vStep; vIndex++)
	{
		float v = (float)vIndex / (float)slices;
		float phi = RangeMap(v, 0, 1, 90.0f, -90.0f);

		for(uint uIndex = 0; uIndex < uStep; uIndex++)
		{
			float u = (float)uIndex / (float)wedges;
			float theta = u * 360.0f;

			Vec3 position = GetSphericalToCoord(radius, theta, phi) + center;
			out->SetUV(Vec2(u, v));

			Vec3 displacement = position - center;

			// Get normal.
			Vec3 normal = displacement.GetNormalized();
			out->SetNormal(normal);

			// Get tangent.
			Vec3 tangent = GetTangentFromSphereCoords(theta, phi);
			out->SetTangent(tangent);

			// Get bitangent.
			Vec3 bitangent = CrossProduct(tangent, normal);
			out->SetBiTangent(bitangent);

			out->AddVertex(position);			
		}		
	}

	for(uint y = 0; y < slices; y++)
	{
		for(uint x = 0; x < wedges; x++)
		{
			uint TLIndex = y * uStep + x;
			uint TRIndex = TLIndex + 1;
			uint BLIndex = TLIndex + uStep;
			uint BRIndex = BLIndex + 1;

			out->AddIndexedQuad(TLIndex, TRIndex, BLIndex, BRIndex);
		}
	}
}

void CPUMeshAddUVCapsule( CPUMesh* out, Vec3 start, Vec3 end, float radius, uint wedges /*= 32*/, uint slices /*= 16 */ )
{
	out->SetColor(Rgba::WHITE);

	uint uStep = wedges + 1;
	uint vStep = slices + 1;
	uint wedgeStart = 0;
	uint sliceStart = 0;
	uint wedgeEnd = wedges;
	uint sliceEnd = slices;

	enum CapsuleSections
	{
		START,
		END,
		DONE
	};

	CapsuleSections capsuleSection = START;
	while(capsuleSection != DONE)
	{
		// Figure which point is being made;
		for(uint vIndex = 0; vIndex < vStep; vIndex++)
		{
			float v = (float)vIndex / (float)slices;
			float phi = -1.0f;
			Vec3 point = Vec3(0.0f, 0.0f, 0.0f);
			switch(capsuleSection)
			{
				case START:
				{
					point = start;
					phi = RangeMap(v, 0, 1, 90.0f, 0.0f);
					break;
				}
				case END:
				{
					point = end;
					phi = RangeMap(v, 0, 1, 0.0f, -90.0f);
					break;
				}
			}			

			// Make Vertexes for that point;
			for(uint uIndex = 0; uIndex < uStep; uIndex++)
			{
				float u = (float)uIndex / (float)wedges;
				float theta = u * 360.0f;

				Vec3 position = GetSphericalToCoord(radius, theta, phi) + point;
				out->SetUV(Vec2(u, v));

				Vec3 displacement = position - point;
				Vec3 normal = displacement.GetNormalized();
				out->SetNormal(normal);

				out->AddVertex(position);			
			}		
		}

		// Make Indices;
		for(uint y = sliceStart; y < sliceEnd; y++)
		{
			for(uint x = wedgeStart; x < wedgeEnd; x++)
			{
				uint TLIndex = y * uStep + x;
				uint TRIndex = TLIndex + 1;
				uint BLIndex = TLIndex + uStep;
				uint BRIndex = BLIndex + 1;

				out->AddIndexedQuad(TLIndex, TRIndex, BLIndex, BRIndex);
			}
		}

		// Set up for next point;
		capsuleSection = CapsuleSections((int)capsuleSection + 1);
		wedgeStart = wedges - 1;
		sliceStart = slices - 1;
		wedgeEnd = wedges * 2 + 1;
		sliceEnd = slices * 2 + 1;		
	}
}

void CreateMeshFromFile( const char* filename, CPUMesh* cpuMesh )
{
	tinyxml2::XMLDocument meshesXMLDoc;
	meshesXMLDoc.LoadFile(filename);

	if(meshesXMLDoc.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		printf("Error with XML Doc: %s\n", filename);
		printf("ErrorID:      %i\n", meshesXMLDoc.ErrorID());
		printf("ErrorLineNum: %i\n", meshesXMLDoc.ErrorLineNum());
		printf("ErrorLineNum: \"%s\"\n", meshesXMLDoc.ErrorName());
	}
	else
	{
		printf("Success with XML Doc: %s\n", filename);

		// Object Values;
		XmlElement* meshesElement = meshesXMLDoc.RootElement();

		std::string invertBool = ParseXmlAttribute(*meshesElement, "invert", "");
		std::string tangentsBool = ParseXmlAttribute(*meshesElement, "tangents", "");
		std::string scale = ParseXmlAttribute(*meshesElement, "scale", "");
		std::string transform = ParseXmlAttribute(*meshesElement, "transform", "");
		bool invertFaces = invertBool == "true" ? true : false;
		float scalef = (float)atof(scale.c_str());
		std::vector<std::string> objectSourceFilenames;

		XmlElement* meshElement = meshesElement->FirstChildElement("mesh");
		while(meshElement)
		{
			std::string objectSourceFilename = ParseXmlAttribute(*meshElement, "src", "");
			objectSourceFilenames.push_back(objectSourceFilename);

			XmlElement* materialElement = meshElement->FirstChildElement("material");
			cpuMesh->m_defaultMaterialName = ParseXmlAttribute(*materialElement, "src", "");

			meshElement = meshElement->NextSiblingElement();
		}

		CreateObjMeshFromFile(cpuMesh, objectSourceFilenames, invertFaces, scalef, transform);
	}
}

void CreateObjMeshFromFile( CPUMesh* cpuMesh, std::vector<std::string>& objectFilenames, bool invertFaces, float scalef, std::string transform )
{
	std::vector<Vec3> verts;
	std::vector<Vec3> uvcoords;
	std::vector<Vec3> normals;
	std::vector<std::string> faces;	

	//std::string& filename = objectFilenames.front();

	for(auto& filename: objectFilenames)
	{
		// The four vectors going in will come out with data;
		ReadContentsOfObjFile(verts, uvcoords, normals, faces, filename.c_str());

		for(auto& face: faces)
		{
			std::stringstream faceStream(face);

			std::string faceChunks[5];
			int chunkIndex = 0;

			while (faceStream.good() && chunkIndex < 5)
			{
				faceStream >> faceChunks[chunkIndex];
				chunkIndex++;
			}

			bool fourVerts = faceChunks[4] != "" ? true : false;
			bool assBackedUp = false;
			for(int x = 1; x < 5; x++)
			{
				if(x == 4 && !fourVerts)
				{
					continue;
				}

				if(!assBackedUp && x == 4 && fourVerts)
				{
					x = 3;
					assBackedUp = true;
				}
				std::string chunk = faceChunks[x];

				std::string delimiter = "/";
				size_t pos = 0;

				std::string splitString[3];
				int splitIndex = 0;
				while ((pos = chunk.find("/")) != std::string::npos) 
				{
					splitString[splitIndex] = chunk.substr(0, pos);
					chunk.erase(0, pos + 1);
					splitIndex++;
				}
				splitString[splitIndex] = chunk;

				int a = atoi(splitString[0].c_str()) - 1;
				int b = atoi(splitString[1].c_str()) - 1;
				int c = atoi(splitString[2].c_str()) - 1;

				VertexMaster vertex;
				vertex.position = verts[a];
				vertex.color = Rgba::WHITE;
				vertex.uv = Vec2(uvcoords[b].x, 1.0f - uvcoords[b].y);
				vertex.normal = normals[c];			
				vertex.tangent = Vec3(1.0f, 1.0f, 1.0f);
				vertex.bitangent = Vec3(1.0f, 1.0f, 1.0f);

				cpuMesh->m_vertices.push_back(vertex);

				if(x == 4 && fourVerts)
				{
					int position = (int)cpuMesh->m_vertices.size() - 5;
					VertexMaster v = cpuMesh->m_vertices[position];
					cpuMesh->m_vertices.push_back(v);
				}
			}
		}

		if(invertFaces)
		{
			for(int i = 0; i < cpuMesh->m_vertices.size() - 3; i += 3)
			{
				VertexMaster vertex1 = cpuMesh->m_vertices[i+1];
				VertexMaster vertex2 = cpuMesh->m_vertices[i+2];

				cpuMesh->m_vertices[i+1] = vertex2;
				cpuMesh->m_vertices[i+2] = vertex1;
			}
		}



		Matrix4x4 transformMatrix;

		// Check transform
		std::stringstream stringStream(transform);
		std::string lineChunks[3];
		int chunkIndex = 0;

		while (stringStream.good() && chunkIndex < 3)
		{
			stringStream >> lineChunks[chunkIndex];
			chunkIndex++;
		}

		Vec3 vectors[3];
		for(int i = 0; i < 3; i++)
		{
			bool negative = false;
			Vec3 vec = Vec3(0.0f, 0.0f, 0.0f);
			if(lineChunks[i].find("-") != std::string::npos)
			{
				negative = true; 
			}

			if(lineChunks[i].find("x") != std::string::npos)
			{
				vec = negative ? Vec3(-1.0f, 0.0f, 0.0f) : Vec3(1.0f, 0.0f, 0.0f);
			}
			else if(lineChunks[i].find("y") != std::string::npos)
			{
				vec = negative ? Vec3(0.0f, -1.0f, 0.0f) : Vec3(0.0f, 1.0f, 0.0f);
			}
			else if(lineChunks[i].find("z") != std::string::npos)
			{
				vec = negative ? Vec3(0.0f, 0.0f, -1.0f) : Vec3(0.0f, 0.0f, 1.0f);
			}
			vec *= scalef;
			vectors[i] = vec;
		}

		transformMatrix.SetI(vectors[0]);
		transformMatrix.SetJ(vectors[1]);
		transformMatrix.SetK(vectors[2]);

		for(auto& vertex: cpuMesh->m_vertices)
		{
			vertex.position = transformMatrix.TransformPosition3D(vertex.position);
			vertex.normal = transformMatrix.TransformPosition3D(vertex.normal);
			vertex.normal.Normalize();
		}

		// do mct here
	}
}

void ReadContentsOfObjFile( std::vector<Vec3>& verts, std::vector<Vec3>& uvcoords, std::vector<Vec3>& normals, std::vector<std::string>& faces, const char* filename )
{
	std::ifstream myfile (filename);
	std::string line;	


	while(!myfile.eof())
	{
		std::string lineChunks[5];
		int chunkIndex = 0;

		getline(myfile, line);
		std::stringstream stringStream(line);

		if(line[0] == 'f' && line[1] == ' ')
		{
			faces.push_back(line);
		}
		else if(line[0] == 'v')
		{
			while (stringStream.good() && chunkIndex < 5)
			{
				stringStream >> lineChunks[chunkIndex];
				chunkIndex++;
			}

			float x = (float)atof(lineChunks[1].c_str());
			float y = (float)atof(lineChunks[2].c_str());
			float z = (float)atof(lineChunks[3].c_str());
			Vec3 value = Vec3(x, y, z);

			if(line[0] == 'v' && line[1] == ' ')
			{
				verts.push_back(value);
			}
			if(line[0] == 'v' && line[1] == 't' && line[2] == ' ')
			{
				uvcoords.push_back(value);
			}
			if(line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
			{
				normals.push_back(value);
			}
		}		
	}
}
