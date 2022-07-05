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
	// �ҵ������е�һ��Mesh
	static aiMesh* GetFirstMesh(aiNode* node,const aiScene* scene);

	// �������нڵ�,�����ǵ�Mesh����vector
	static void ProcessNode(aiNode* node,const aiScene* scene,std::vector<aiMesh*>& meshList);
};
