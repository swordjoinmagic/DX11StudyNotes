#pragma once

#include "D3DUtils.h"
#include "Mesh.h"

class ObjLoader {
public:
	static std::shared_ptr<Mesh> LoadMesh(const std::wstring& path);
};