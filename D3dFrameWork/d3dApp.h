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

	// 获得这个d3d程序的句柄
	HINSTANCE AppInst() const;
	// 获得窗口句柄
	HWND MainWnd() const;
	// 获得窗口比例
	float AspectRatio() const;

	// 运行D3D程序的方法
	int Run();

	// 初始化窗口和D3D设备
	virtual bool Init();

	// 当窗口改变时,自动引用分辨率改变的模板/深度缓冲区
	virtual void OnResize();

	// 用于更新场景逻辑(位移/碰撞检测等)
	virtual void UpdateScene(float deltaTime) {}

	// 初始化该d3dApp的方法,一般用于初始化模型,顶点结构,着色器等
	virtual void OnStart() = 0;

	// 窗口过程函数,用于处理各类消息
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// 用于处理鼠标事件
	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	virtual void OnMouseWheel(WPARAM wParam,LPARAM lParam);

	// 读取场景，读取完毕后，每帧会自动执行场景的渲染流程
	void LoadScene(std::shared_ptr<Scene> scene);
protected:
	// 初始化Windows窗口
	bool InitMainWindow();
	// 初始化D3D程序
	bool InitDirect3D();

	bool InitDirect2D();

	void CalculateFrameState();
	virtual void DrawScene();

	// 清除控制台信息
	void ClearConsoleDebugInformation();
protected:
	// 应用程序实例句柄
	HINSTANCE mhAppInst;
	// 主窗口句柄
	HWND mhMainWnd;	
	// 程序是否处于暂停状态
	bool mAppPaused;
	// 程序是否最小化最大化
	bool mMinimized;
	bool mMaximized;
	// 程序是否处于在改变大小的状态
	bool mResizing;

	// 4x msaa的质量等级
	UINT m4xMessQuality;

	// 当前运行的场景
	std::shared_ptr<Scene> currentScene;

	// d3d设备,用于创建各种渲染所需资源(Buffer,inputLayout等)
	ComPtr<ID3D11Device> md3dDevice;
	// d3d上下文,用于对管线各阶段进行设置
	ComPtr<ID3D11DeviceContext> md3dImmediateContext;

	// 渲染目标视图
	ComPtr<ID3D11RenderTargetView> mRenderTargetView;

	// 深度/模板视图
	ComPtr<ID3D11DepthStencilView> mDepthStencilView;
	// 深度的着色器资源视图
	ComPtr<ID3D11ShaderResourceView> mDepthStencilSRV;

	// 用于记录deltaTime的计时器类
	std::shared_ptr<GameTimer> timer;
protected:
	// d3d交换链
	ComPtr<IDXGISwapChain> mSwapChain;
	// 后备缓冲区纹理
	ComPtr<ID3D11Texture2D> backBuffer;
	// 深度/模板缓冲区
	ComPtr<ID3D11Texture2D> mDepthStencilBuffer;

	// 视口
	D3D11_VIEWPORT mScreenViewport;

	D3D_DRIVER_TYPE md3dDriverType;

	// D2D工厂
	ComPtr<ID2D1Factory> m_pd2dFactory;							
	// D2D渲染目标
	ComPtr<ID2D1RenderTarget> m_pd2dRenderTarget;				
	// DWrite工厂
	ComPtr<IDWriteFactory> m_pdwriteFactory;					

	int mClientWidth;
	int mClientHeight;
	
	bool mEnable4xMsaa;

	// 窗口名
	std::wstring mMainWndCaption = L"Racing Game";

protected:
	// 用于管理渲染状态
	RenderStates renderStates;
	// 用于标识是否是第一次移动鼠标
	// (第一次移动鼠标无法计算偏移,需要先记录鼠标鼠标)
	bool firstMouseMove = true;

	void ProcessRawInput(WPARAM wParam, LPARAM lParam);	
};

#endif // !D3DAPP_H
