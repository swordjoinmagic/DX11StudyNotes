#pragma once
#ifndef D3DAPP_H
#define D3DAPP_H

#include "D3DUtils.h"
#include "GameTimer.h"
#include <Windows.h>
#include <windowsx.h>
#include <string>
#include "RenderStates.h"
#include "Input.h"
#include <d2d1.h>
#include <dwrite.h>

class Scene;
class D3DApp {
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp();

	// ������d3d����ľ��
	HINSTANCE AppInst() const;
	// ��ô��ھ��
	HWND MainWnd() const;
	// ��ô��ڱ���
	float AspectRatio() const;

	// ����D3D����ķ���
	int Run();

	// ��ʼ�����ں�D3D�豸
	virtual bool Init();

	// �����ڸı�ʱ,�Զ����÷ֱ��ʸı��ģ��/��Ȼ�����
	virtual void OnResize();

	// ���ڸ��³����߼�(λ��/��ײ����)
	virtual void UpdateScene(float deltaTime) {}

	// ��ʼ����d3dApp�ķ���,һ�����ڳ�ʼ��ģ��,����ṹ,��ɫ����
	virtual void OnStart() = 0;

	// ���ڹ��̺���,���ڴ��������Ϣ
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// ���ڴ�������¼�
	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	virtual void OnMouseWheel(WPARAM wParam,LPARAM lParam);

	// ��ȡ��������ȡ��Ϻ�ÿ֡���Զ�ִ�г�������Ⱦ����
	void LoadScene(std::shared_ptr<Scene> scene);
protected:
	// ��ʼ��Windows����
	bool InitMainWindow();
	// ��ʼ��D3D����
	bool InitDirect3D();

	bool InitDirect2D();

	void CalculateFrameState();
	virtual void DrawScene();

	// �������̨��Ϣ
	void ClearConsoleDebugInformation();
protected:
	// Ӧ�ó���ʵ�����
	HINSTANCE mhAppInst;
	// �����ھ��
	HWND mhMainWnd;	
	// �����Ƿ�����ͣ״̬
	bool mAppPaused;
	// �����Ƿ���С�����
	bool mMinimized;
	bool mMaximized;
	// �����Ƿ����ڸı��С��״̬
	bool mResizing;

	// 4x msaa�������ȼ�
	UINT m4xMessQuality;

	// ��ǰ���еĳ���
	std::shared_ptr<Scene> currentScene;

	// d3d�豸,���ڴ���������Ⱦ������Դ(Buffer,inputLayout��)
	ComPtr<ID3D11Device> md3dDevice;
	// d3d������,���ڶԹ��߸��׶ν�������
	ComPtr<ID3D11DeviceContext> md3dImmediateContext;

	// ��ȾĿ����ͼ
	ComPtr<ID3D11RenderTargetView> mRenderTargetView;

	// ���/ģ����ͼ
	ComPtr<ID3D11DepthStencilView> mDepthStencilView;
	// ��ȵ���ɫ����Դ��ͼ
	ComPtr<ID3D11ShaderResourceView> mDepthStencilSRV;

	// ���ڼ�¼deltaTime�ļ�ʱ����
	std::shared_ptr<GameTimer> timer;
protected:
	// d3d������
	ComPtr<IDXGISwapChain> mSwapChain;
	// �󱸻���������
	ComPtr<ID3D11Texture2D> backBuffer;
	// ���/ģ�建����
	ComPtr<ID3D11Texture2D> mDepthStencilBuffer;

	// �ӿ�
	D3D11_VIEWPORT mScreenViewport;

	D3D_DRIVER_TYPE md3dDriverType;

	// D2D����
	ComPtr<ID2D1Factory> m_pd2dFactory;							
	// D2D��ȾĿ��
	ComPtr<ID2D1RenderTarget> m_pd2dRenderTarget;				
	// DWrite����
	ComPtr<IDWriteFactory> m_pdwriteFactory;					

	int mClientWidth;
	int mClientHeight;
	
	bool mEnable4xMsaa;

	// ������
	std::wstring mMainWndCaption = L"Racing Game";

protected:
	// ���ڹ�����Ⱦ״̬
	RenderStates renderStates;
	// ���ڱ�ʶ�Ƿ��ǵ�һ���ƶ����
	// (��һ���ƶ�����޷�����ƫ��,��Ҫ�ȼ�¼������)
	bool firstMouseMove = true;

	void ProcessRawInput(WPARAM wParam, LPARAM lParam);	
};

#endif // !D3DAPP_H
