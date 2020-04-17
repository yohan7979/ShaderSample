#pragma once
#include "ModelClass.h"

class InstancedModelClass :
	public ModelClass
{
	struct InstanceType
	{
		D3DXVECTOR3 position;
	};

	typedef ModelClass Super;

public:
	InstancedModelClass();
	~InstancedModelClass();

protected:
	virtual bool InitializeBuffers(ID3D11Device* device);
	virtual void ShutdownBuffers();
	virtual void RenderBuffers(ID3D11DeviceContext* deviceContext);

	virtual void CacheInstanceVector(ID3D11Device* device, const vector<D3DXVECTOR3>& instanceVector);
	bool CreateInstanceBuffer(ID3D11Device* device, void* vertices);

public:
	inline int GetInstanceCount() const { return m_instanceCount; }

private:
	int m_instanceCount;
	ID3D11Buffer* m_instanceBuffer;
	vector<D3DXVECTOR3> m_vecInstances;
};

