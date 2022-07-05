#pragma once

#include "Mesh.h"
#include "D3DUtils.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
class MeshLoader {
public:
	static std::shared_ptr<Mesh> LoadMesh(const std::string& path);
	static std::shared_ptr<Mesh> LoadMesh(const std::string& path,uint index);
	// 找到场景中第一个Mesh
	static aiMesh* GetFirstMesh(aiNode* node,const aiScene* scene);

	// 处理所有节点,将他们的Mesh加入vector
	static void ProcessNode(aiNode* node,const aiScene* scene,std::vector<aiMesh*>& meshList);
};
