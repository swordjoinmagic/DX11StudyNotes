#include "MeshLoader.h"
#include "DefaultVertex.h"


#include <iostream>

aiMesh* MeshLoader::GetFirstMesh(aiNode* node, const aiScene* scene) {
	// 处理节点所有的网格（如果有的话）
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		return mesh;
	}
	// 接下来对它的子节点重复这一过程
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		return GetFirstMesh(node->mChildren[i], scene);
	}
}

void MeshLoader::ProcessNode(aiNode* node, const aiScene* scene, std::vector<aiMesh*>& meshList) {
	// 处理节点所有的网格（如果有的话）
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshList.push_back(mesh);
	}
	// 接下来对它的子节点重复这一过程
	for (unsigned int i = 0; i < node->mNumChildren; i++) {		
		ProcessNode(node->mChildren[i],scene,meshList);
	}
}

std::shared_ptr<Mesh> MeshLoader::LoadMesh(const std::string& path,uint index) {
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		throw;
	}

	aiNode* rootNode = scene->mRootNode;

	std::vector<aiMesh*> meshList;
	ProcessNode(rootNode, scene, meshList);

	auto mesh = meshList[index];

	std::vector<DefaultVertex> vertices;
	std::vector<uint> indices;

	for (uint i = 0; i < mesh->mNumVertices; i++) {
		DefaultVertex vertex;
		vertex.pos = float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.normal = float3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if (mesh->mTextureCoords[0]) // 网格是否有纹理坐标？
			vertex.texcoord = float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		vertex.tangent = float3(0, 0, 0);

		vertices.push_back(vertex);
	}

	for (uint i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	return std::make_shared<Mesh>(vertices, indices);

}



std::shared_ptr<Mesh> MeshLoader::LoadMesh(const std::string& path) {
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		throw;
	}	

	aiNode* rootNode = scene->mRootNode;

	std::vector<aiMesh*> meshList;
	ProcessNode(rootNode,scene, meshList);

	std::vector<DefaultVertex> vertices;
	std::vector<uint> indices;

	int baseIndex = 0;

	for (auto mesh : meshList) {
		std::vector<uint> iList;

		for (uint i = 0; i < mesh->mNumVertices; i++) {
			DefaultVertex vertex;
			vertex.pos = float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			vertex.normal = float3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			if (mesh->mTextureCoords[0]) // 网格是否有纹理坐标？
				vertex.texcoord = float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			vertex.tangent = float3(0, 0, 0);

			vertices.push_back(vertex);
		}

		for (uint i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				iList.push_back(face.mIndices[j]);

				indices.push_back(face.mIndices[j]+ baseIndex);
			}
		}

		baseIndex += iList.size();
	}

	return std::make_shared<Mesh>(vertices,indices);
}