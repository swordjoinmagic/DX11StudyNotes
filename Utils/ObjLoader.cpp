#include "ObjLoader.h"

#include "DefaultVertex.h"


std::shared_ptr<Mesh> ObjLoader::LoadMesh(const std::wstring& path) {
	std::vector<DefaultVertex> vertices;
	std::vector<uint> indices;

	std::vector<float3> positions;
	std::vector<float3> normals;
	std::vector<float2> texcoords;

	// 下标为position里某个顶点的下标,值为该顶点共面法线数量
	std::vector<uint> coplanarNormalNumber;
	// 共面的纹理坐标
	std::vector<uint> coplanarTexcoordNumber;

	std::wifstream wfin(path);
	// 切换为中文,防止乱码
	std::locale china("chs");
	wfin.imbue(china);

	while (true) {
		std::wstring wstr;
		if (!(wfin >> wstr)) break;
		
		if (wstr[0] == '#') {			
			// 忽略注释所在行			
			while (!wfin.eof() && wfin.get() != '\n')
				continue;
		} else if (wstr == L"v") {
			// 顶点结构,后面跟着三个float型

			// obj使用右手坐标系,这里反转z值
			float3 pos;
			wfin >> pos.x >> pos.y >> pos.z;
			pos.z = -pos.z;
			positions.push_back(pos);

			DefaultVertex vertex;
			vertex.pos = pos;
			vertex.normal = float3(0,0,0);
			vertex.tangent = float3(0,0,0);
			vertices.push_back(vertex);
		} else if (wstr == L"vt") {
			// 纹理uv

			float u, v;
			wfin >> u >> v;
			// dx的y轴朝下,所以要将obj里的y轴反转一下
			v = 1.0f - v;
			texcoords.push_back(float2(u,v));
		} else if(wstr==L"vn"){
			// 法线结构,后面跟着三个float
			// 需要注意obj里是面法线,所以对于同一个顶点,他可能有多个面法线
			// 这个时候直接相加平均这些面法线

			float x, y, z;
			wfin >> x >> y >> z;
			z = -z;
			normals.push_back(float3(x,y,z));
		} else if(wstr==L"f"){
			// 几何面,这里仅处理有三个顶点的三角面
			
			if (coplanarNormalNumber.size() == 0) coplanarNormalNumber.resize(vertices.size());
			if (coplanarTexcoordNumber.size() == 0) coplanarTexcoordNumber.resize(vertices.size());

			uint vIndex[3], vNormalIndex[3], vTexcoord[3];
			wchar_t ignore;
			// obj的三角形是逆时针,dx是顺时针
			for (int i = 2; i >= 0; --i) {
				wfin >> vIndex[i] >> ignore >> vTexcoord[i] >> ignore >> vNormalIndex[i];
			}

			for (int i = 0; i < 3;i++) {
				vIndex[i] -= 1; vNormalIndex[i] -= 1; vTexcoord[i] -= 1;
				// 获得顶点
				DefaultVertex& vertex = vertices[vIndex[i]];

				// 如果normal不为0向量,说明之前设置过,那么当前normal是该顶点的共面法线
				if (vertex.normal.x != 0 || vertex.normal.y != 0 || vertex.normal.z != 0)
					coplanarNormalNumber[vIndex[i]] += 1;

				float3 normal = normals[vNormalIndex[i]];
				vertex.normal.x += normal.x; 
				vertex.normal.y += normal.y; 
				vertex.normal.z += normal.z;

				if (vertex.texcoord.x != 0 || vertex.texcoord.y != 0)
					coplanarTexcoordNumber[vIndex[i]] += 1;

				vertex.texcoord.x += texcoords[vTexcoord[i]].x;
				vertex.texcoord.y += texcoords[vTexcoord[i]].y;

				// 添加索引
				indices.push_back(vIndex[i]);
			}
		}
	}

	for (uint i = 0; i < coplanarNormalNumber.size(); i++) {
		DefaultVertex& vertex = vertices[i];
		
		XMVECTOR normal = XMLoadFloat3(&vertex.normal);
		XMVECTOR averageN = normal / coplanarNormalNumber[i];

		XMStoreFloat3(&vertex.normal,averageN);

		XMVECTOR texcoord = XMLoadFloat2(&vertex.texcoord);
		XMVECTOR averageT = texcoord / coplanarTexcoordNumber[i];
		XMStoreFloat2(&vertex.texcoord,averageT);
	}

	return std::make_shared<Mesh>(vertices,indices);
}