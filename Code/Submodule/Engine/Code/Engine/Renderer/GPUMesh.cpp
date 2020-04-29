#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/BufferLayout.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/VertexLit.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

GPUMesh::GPUMesh( RenderContext* renderContext )
{
	m_renderContext = renderContext;
	m_vertexBuffer = new VertexBuffer(renderContext);
	m_indexBuffer = new IndexBuffer(renderContext);
}

GPUMesh::~GPUMesh()
{
	delete m_vertexBuffer;
	m_vertexBuffer = nullptr;

	delete m_indexBuffer;
	m_indexBuffer = nullptr;
}

void GPUMesh::CreateFromCPUMesh( const CPUMesh* cpuMesh, GPUMemoryUsage gpuMemoryUsage /*= GPU_MEMORY_USAGE_STATIC */ )
{
	UNUSED(gpuMemoryUsage);

	// 1. Need to allocate a Buffer
	const BufferLayout* layout = cpuMesh->GetLayout();
	ASSERT_OR_DIE( layout != nullptr, "Mesh MUST have a layout." );

	size_t sizeNeeded = layout->GetStride() * cpuMesh->GetVertexCount();
	void* buffer = malloc( sizeNeeded );

	// 2. Copy into that buffer
	layout->m_copyFromMaster( buffer, cpuMesh->GetVertices(), cpuMesh->GetVertexCount() );

	// 3. Same as before
	m_vertexBuffer->CreateStaticFor(buffer, layout->GetStride(), cpuMesh->GetVertexCount());
	m_indexBuffer->CreateStaticFor(cpuMesh->m_indices.data(), cpuMesh->GetIndexCount());

	SetDrawCall( cpuMesh->UsesIndexBuffer(), cpuMesh->GetElementCount());

	// 4. Save off the layout - needed to create input layouts later
	m_layout = layout;
	m_defaultMaterialName = cpuMesh->m_defaultMaterialName;

	free( buffer );
}

void GPUMesh::CreateMeshFromFile( const char* filename )
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
			m_defaultMaterialName = ParseXmlAttribute(*materialElement, "src", "");

			meshElement = meshElement->NextSiblingElement();
		}
		
		this->CreateObjMeshFromFile(objectSourceFilenames, invertFaces, scalef, transform);
	}
}

void GPUMesh::CreateObjMeshFromFile( std::vector<std::string>& objectFilenames, bool invertFaces, float scalef, std::string transform )
{
	std::vector<Vertex_Lit> vertexs;
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

				Vertex_Lit vertex;
				vertex.position = verts[a];
				vertex.color = Rgba::WHITE;
				vertex.uvTexCoords = Vec2(uvcoords[b].x, 1.0f - uvcoords[b].y);
				vertex.normal = normals[c];			
				vertex.tangent = Vec3(1.0f, 1.0f, 1.0f);
				vertex.bitangent = Vec3(1.0f, 1.0f, 1.0f);

				vertexs.push_back(vertex);

				if(x == 4 && fourVerts)
				{
					int position = (int)vertexs.size() - 5;
					Vertex_Lit v = vertexs[position];
					vertexs.push_back(v);
				}
			}
		}

		if(invertFaces)
		{
			for(int i = 0; i < vertexs.size() - 3; i += 3)
			{
				Vertex_Lit vertex1 = vertexs[i+1];
				Vertex_Lit vertex2 = vertexs[i+2];

				vertexs[i+1] = vertex2;
				vertexs[i+2] = vertex1;
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

		for(auto& vertex: vertexs)
		{
			vertex.position = transformMatrix.TransformPosition3D(vertex.position);
			vertex.normal = transformMatrix.TransformPosition3D(vertex.normal);
			vertex.normal.Normalize();
		}

		// do mct here
	}

	

	this->CreateFromVertices(vertexs);
}

void GPUMesh::ReadContentsOfObjFile( std::vector<Vec3>& verts, std::vector<Vec3>& uvcoords, std::vector<Vec3>& normals, std::vector<std::string>& faces, const char* filename )
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

void GPUMesh::SetDrawCall( bool usesIndices, uint elementCount )
{
	if(usesIndices)
	{
		ASSERT_OR_DIE( m_indexBuffer->GetIndexCount() >= elementCount, "Index Buffer count is higher than element count." );
	}
	else
	{
		ASSERT_OR_DIE( m_vertexBuffer->GetVertexCount() >= elementCount, "Vertex Buffer count is higher than element count." );
	}

	m_elementCount = elementCount;
	m_useIndexBuffer = usesIndices;
}
