#include "ObjLoader.h"

#include "DefaultVertex.h"


std::shared_ptr<Mesh> ObjLoader::LoadMesh(const std::wstring& path) {
	std::vector<DefaultVertex> vertices;
	std::vector<uint> indices;

	std::vector<float3> positions;
	std::vector<float3> normals;
	std::vector<float2> texcoords;

	// �±�Ϊposition��ĳ��������±�,ֵΪ�ö��㹲�淨������
	std::vector<uint> coplanarNormalNumber;
	// �������������
	std::vector<uint> coplanarTexcoordNumber;

	std::wifstream wfin(path);
	// �л�Ϊ����,��ֹ����
	std::locale china("chs");
	wfin.imbue(china);

	while (true) {
		std::wstring wstr;
		if (!(wfin >> wstr)) break;
		
		if (wstr[0] == '#') {			
			// ����ע��������			
			while (!wfin.eof() && wfin.get() != '\n')
				continue;
		} else if (wstr == L"v") {
			// ����ṹ,�����������float��

			// objʹ����������ϵ,���ﷴתzֵ
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
			// ����uv

			float u, v;
			wfin >> u >> v;
			// dx��y�ᳯ��,����Ҫ��obj���y�ᷴתһ��
			v = 1.0f - v;
			texcoords.push_back(float2(u,v));
		} else if(wstr==L"vn"){
			// ���߽ṹ,�����������float
			// ��Ҫע��obj�����淨��,���Զ���ͬһ������,�������ж���淨��
			// ���ʱ��ֱ�����ƽ����Щ�淨��

			float x, y, z;
			wfin >> x >> y >> z;
			z = -z;
			normals.push_back(float3(x,y,z));
		} else if(wstr==L"f"){
			// ������,��������������������������
			
			if (coplanarNormalNumber.size() == 0) coplanarNormalNumber.resize(vertices.size());
			if (coplanarTexcoordNumber.size() == 0) coplanarTexcoordNumber.resize(vertices.size());

			uint vIndex[3], vNormalIndex[3], vTexcoord[3];
			wchar_t ignore;
			// obj������������ʱ��,dx��˳ʱ��
			for (int i = 2; i >= 0; --i) {
				wfin >> vIndex[i] >> ignore >> vTexcoord[i] >> ignore >> vNormalIndex[i];
			}

			for (int i = 0; i < 3;i++) {
				vIndex[i] -= 1; vNormalIndex[i] -= 1; vTexcoord[i] -= 1;
				// ��ö���
				DefaultVertex& vertex = vertices[vIndex[i]];

				// ���normal��Ϊ0����,˵��֮ǰ���ù�,��ô��ǰnormal�Ǹö���Ĺ��淨��
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

				// �������
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