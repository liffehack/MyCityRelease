#pragma once

#include "CRenderManager.h"

int CRenderManager::Samples = 8;
int CRenderManager::Filter = 3;
int CRenderManager::UBOUpdateCount = -1;

// ���������� ��� ������ ������� (�������������� �� ���� �������������)
GLuint	VAO_Index = 0;		// ������ VAO-������
GLuint	VBO_Index = 0;		// ������ VBO-������

// ������������� ���������� ��� ������ �������
void InitObject()
{
	// �������� � ���������� VBO
	glGenBuffers(1, &VBO_Index);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Index);
	GLfloat	Verteces[] = { -1, 1, -1, -1, 1, 1, 1, 1, -1, -1, 1, -1 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(Verteces), Verteces, GL_STATIC_DRAW);
	// �������� VAO
	glGenVertexArrays(1, &VAO_Index);
	glBindVertexArray(VAO_Index);
	// ���������� VAO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Index);
	int	k = 0;
	glVertexAttribPointer(k, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(k);
	// "�������" ������ VAO, ���� �������� �� ���������
	glBindVertexArray(0);
}

//����������������������RenderManager
//�������������������������������OpenGL:�������������������.�.
void CRenderManager::init()
{
	// �������� ����������� ������ ������
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_MULTISAMPLE);
	// �������� ������� ��� ������ ��������
	Shaders[0].LoadVertexShader("Shaders\\DirectLight.vsh", false);
	Shaders[0].LoadFragmentShader("Shaders\\DirectLight.fsh", false);
	Shaders[0].Link(false);
	// �������� ������� ��� ������ ���������
	Shaders[1].LoadVertexShader("Shaders\\SkyBox.vsh", false);
	Shaders[1].LoadFragmentShader("Shaders\\SkyBox.fsh", false);
	Shaders[1].Link(false);
	// �������� ������� ��� ������ AABB
	Shaders[2].LoadVertexShader("Shaders\\BoundingBox.vsh", false);
	Shaders[2].LoadFragmentShader("Shaders\\BoundingBox.fsh", false);
	Shaders[2].Link(false);
	// �������� ������� ��� ������ ������ �����
	Shaders[3].LoadVertexShader("Shaders\\SimplePostProcessing.vsh", false);
	Shaders[3].LoadFragmentShader("Shaders\\SimplePostProcessing.fsh", false);
	Shaders[3].Link(false);
	// �������� ������� ��� �/� �������
	Shaders[4].LoadVertexShader("Shaders\\GreyPostProcessing.vsh", false);
	Shaders[4].LoadFragmentShader("Shaders\\GreyPostProcessing.fsh", false);
	Shaders[4].Link(false);
	// �������� ������� ��� ������� �����
	Shaders[5].LoadVertexShader("Shaders\\SepiaPostProcessing.vsh", false);
	Shaders[5].LoadFragmentShader("Shaders\\SepiaPostProcessing.fsh", false);
	Shaders[5].Link(false);
	// �������� ���� ��� ����� ���������
	SkyBoxMeshId = CResourceManager::Instance().LoadMesh("Meshes\\box.obj");
	// �������� ���� ��� ������ AABB
	AABBMeshId = CResourceManager::Instance().LoadMesh("Meshes\\box.obj");
	// �������� ���������� �������� ��� ������ ���������
	SkyBoxTextureId = CResourceManager::Instance().LoadCubeMap(3);
	// ������� uniform-������ � �������
	int Scene_Block, Object_Block;
	// ������� UBO-����� ��� ���� ��������
	glGenBuffers(1, &PerSceneUBOIndex);
	// ������� ����� ��������
	Scene_Block = glGetUniformBlockIndex(Shaders[0].GetShaderProgram(), "PerSceneBlock");
	glUniformBlockBinding(Shaders[0].GetShaderProgram(), Scene_Block, 0);
	Object_Block = glGetUniformBlockIndex(Shaders[0].GetShaderProgram(), "PerObjectBlock");
	glUniformBlockBinding(Shaders[0].GetShaderProgram(), Object_Block, 1);
	// ��������� �����, �.�. �� ����� �� �����������
	updatePerSceneBlock(PerSceneUBOIndex);
	// ������������� ������� ��� ������ ������ �����
	InitObject();
}

//����������������������������
void CRenderManager::start()
{
	// ������� ������� ������
	GraphicObjects.clear();
}

//����������������������������
void CRenderManager::setCamera(CCamera& camera)
{
	if (Camera != camera)
	{
		Camera = camera;
		updatePerSceneBlock(PerSceneUBOIndex);
		for (auto it = RenderManagerObjectStates.begin(); it != RenderManagerObjectStates.end(); it++)
			it->second.UBOIndexUpdate = true;
	};
}

//�������������������������������������
void CRenderManager::setLight(CLight& light)
{
	Light = light;
}

//���������������������������������������������������������
void CRenderManager::addToRenderQueue(CGraphicObject& GraphicObject)
{
	GraphicObjects.push_back(GraphicObject);
}

// ���������� FBO
void CRenderManager::UpdateFBO(int w, int h, int s)
{
	FBO.init(w, h);
	MSAA.init(w, h, s);
}

//����������������������
void CRenderManager::finish(void)
{
	// �������� FBO �� ������������� ��������
	MSAA.bind();
	// ����� ���������
	SkyBoxRender();
	// ����� ��������
	ObjectsRender();
	// ����� AABB
	if (AABBRenderEnable)
		AABBRender();
	// �������� ������������ ������ �����	
	MSAA.unbind();
	// ���������� �������� � FBO ��� ������������� �������
	MSAA.resolveToFbo(FBO);
	// ����� �������� �� �����
	glDisable(GL_DEPTH_TEST);
	Shaders[Filter].Activate();
	FBO.bindColorTexture();
	FBO.bindDepthTexture();
	glBindVertexArray(VAO_Index);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	Shaders[Filter].Deactivate();
	glEnable(GL_DEPTH_TEST);
	//������������������������������
	glutSwapBuffers();
}

//�����������������,�����������������������������
void CRenderManager::updatePerSceneBlock(int Index)
{
	UBOUpdateCount++;
	// ��������� ��� �������� ������ � ������
	struct SceneData
	{
		mat4 ProjectionMatrix;
		vec4 lAmbient;
		vec4 lDiffuse;
		vec4 lSpecular;
		vec4 lPosition;
	} UBO_Scene;
	// ��������� ���������� �����
	mat4 ViewMatrix = Camera.GetViewMatrix();
	UBO_Scene.ProjectionMatrix = Camera.GetProjectionMatrix();
	UBO_Scene.lAmbient = Light.getAmbient();
	UBO_Scene.lDiffuse = Light.getDiffuse();
	UBO_Scene.lSpecular = Light.getSpecular();
	UBO_Scene.lPosition = ViewMatrix * Light.getPosition();
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, Index);
	glBindBuffer(GL_UNIFORM_BUFFER, Index);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SceneData), &UBO_Scene, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

//�����������������,�������������������������������������������
void CRenderManager::updatePerObjectBlock(int UBOindex, CGraphicObject& GraphicObject)
{
	UBOUpdateCount++;
	// ��������� ��� �������� ������ � ������
	struct ObjectData
	{
		mat4 ModelViewMatrix;
		vec4 mAmbient;
		vec4 mDiffuse;
		vec4 mSpecular;
		float Shininess;
	} UBO_Object;
	// ��������� ���������� �������
	mat4 ViewMatrix = Camera.GetViewMatrix();
	UBO_Object.ModelViewMatrix = ViewMatrix * GraphicObject.getModelMatrix();
	UBO_Object.mAmbient = GraphicObject.getMaterial().getAmbient();
	UBO_Object.mDiffuse = GraphicObject.getMaterial().getDiffuse();
	UBO_Object.mSpecular = GraphicObject.getMaterial().getSpecular();
	UBO_Object.Shininess = GraphicObject.getMaterial().getShininess();
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, UBOindex);
	glBindBuffer(GL_UNIFORM_BUFFER, UBOindex);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(ObjectData), &UBO_Object, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

//������SkyBox'�
void CRenderManager::SkyBoxRender()
{
	Shaders[1].Activate();
	Shaders[1].SetUniformMat4("uProjectionMatrix", Camera.GetProjectionMatrix());
	Shaders[1].SetUniformMat4("uModelViewMatrix", Camera.GetViewMatrix());
	// ����������� �������� ����������
	glDepthFunc(GL_ALWAYS);
	glCullFace(GL_FRONT);
	// ������ ���
	CCubeMap* Texture = CResourceManager::Instance().GetCubeMap(SkyBoxTextureId);
	if (Texture != nullptr) Texture->Apply();
	CMesh *mesh = CResourceManager::Instance().GetMesh(SkyBoxMeshId);
	mesh->Render();
	Shaders[1].Deactivate();
	// ��������������� �������� ����������
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
}

//������������������������������������
void CRenderManager::ObjectsRender()
{
	//����������������� ��� ������ ��������
	Shaders[0].Activate();
	for (int i = 0; i < GraphicObjects.size(); i++)
	{
		SRenderManagerObjectState Prev;
		// ��������� ������� ������� � ����������
		auto it = RenderManagerObjectStates.find(GraphicObjects[i].getID());
		if (it != RenderManagerObjectStates.end())
		{
			Prev = it->second;
			if (Prev.GraphicObject != GraphicObjects[i])
				Prev.UBOIndexUpdate = true;
		}
		else
		{
			// ��������� ������� � ���������
			Prev.GraphicObject = GraphicObjects[i];
			glGenBuffers(1, &Prev.UBOIndex);
			RenderManagerObjectStates.insert(pair<int, SRenderManagerObjectState>(GraphicObjects[i].getID(), Prev));
		}
		// ���������� UBO-�����
		if (Prev.UBOIndexUpdate)
			updatePerObjectBlock(Prev.UBOIndex, Prev.GraphicObject);
		// ����� ������
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, Prev.UBOIndex);
		glBindBuffer(GL_UNIFORM_BUFFER, Prev.UBOIndex);
		CTexture* Texture = CResourceManager::Instance().GetTexture(GraphicObjects[i].getTexture());
		if (Texture != nullptr) Texture->Apply();
		CMesh* Mesh = CResourceManager::Instance().GetMesh(GraphicObjects[i].getMesh());
		if (Mesh != nullptr) Mesh->Render();
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	for (auto it = RenderManagerObjectStates.begin(); it != RenderManagerObjectStates.end(); it++)
		it->second.UBOIndexUpdate = false;
	// ������������ ������ ��� ������ ��������
	Shaders[0].Deactivate();
}

//��������� �������������
void CRenderManager::AABBRender()
{
	//�������̠��Š����
	//������������������������������
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	//�����������������(+����������������������������������)
	Shaders[AABB_SHADER_ID].Activate();
	Shaders[AABB_SHADER_ID].SetUniformMat4("ProjectionMatrix", Camera.GetProjectionMatrix());
	Shaders[AABB_SHADER_ID].SetUniformVec4("Color", vec4(1, 0, 0, 1));
	//�����������������(�������)
	for (auto it = GraphicObjects.begin(); it < GraphicObjects.end(); it++)
	{
		vec3 AABB = it->getAABB();
		mat4 scale = mat4(
			vec4(AABB.x, 0, 0, 0),
			vec4(0, AABB.y, 0, 0),
			vec4(0, 0, AABB.z, 0),
			vec4(0, 0, 0, 1));
		mat4 ModelViewMatrix = Camera.GetViewMatrix() * it->getModelMatrix() * scale;
		Shaders[AABB_SHADER_ID].SetUniformMat4("ModelViewMatrix", ModelViewMatrix);
		CMesh* mesh = CResourceManager::Instance().GetMesh(AABBMeshId);
		if (mesh != nullptr) mesh->Render();
	}
	Shaders[AABB_SHADER_ID].Deactivate();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
}

//����������������������������,������������������������
int CRenderManager::getObjectCount()
{
	return GraphicObjects.size();
}

//����������������������
void CRenderManager::changeAABBRenderMode()
{
	AABBRenderEnable = !AABBRenderEnable;
}

// ���������� UBO
void CRenderManager::UpdateUBO()
{
	for (auto it = RenderManagerObjectStates.begin(); it != RenderManagerObjectStates.end(); it++)
		it->second.UBOIndexUpdate = true;
}