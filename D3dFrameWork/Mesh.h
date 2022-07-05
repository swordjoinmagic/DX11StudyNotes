#pragma once

#include "D3DUtils.h"
#include "DefaultVertex.h"
#include "Shader.h"
class Mesh {
private:
	ComPtr<ID3D11Buffer> verticesBuffer;
	ComPtr<ID3D11Buffer> indicesBuffer;
	uint indexCount;

	std::vector<DefaultVertex> vertics;
	std::vector<uint> indices;
public:
	Mesh(const std::vector<DefaultVertex>& vertices, const std::vector<uint>& indices) :vertics(vertices), indices(indices) { indexCount = indices.size(); }
	Mesh(const std::vector<DefaultVertex>& vertics,const std::vector<uint>& indices,ID3D11Device* device);
	virtual ~Mesh();

	void SetUpBuffer(ID3D11Device* device);

	void Draw(std::shared_ptr<Shader> shader,ID3D11DeviceContext* context,D3D_PRIMITIVE_TOPOLOGY primitiveTopology=D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void DrawInstanced(std::shared_ptr<Shader> shader,uint instancedCount, ID3D11DeviceContext* context, D3D_PRIMITIVE_TOPOLOGY primitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
};