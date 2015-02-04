/*****************************************************************************/
/*

CS391 Project 3 Profiler Scenario
This is adapted from Cody Pritchard's PHY350/399 Fluid Simulation Work

This sample provides a nice, juicy, CPU intensive application to test
your profiler on for project 3.

*/
/*****************************************************************************/

#include "DXUT.h"
#include "DXUTgui.h"
#include "SDKmisc.h"
#include "Fluid.h"
#include "render_vs.h"
#include "render_gs.h"
#include "render_ps.h"

#pragma warning(disable:4100) // warning C4100: 'x' : unreferenced formal parameter

extern Fluid* FLUID;

struct ParticleVertex {
	DirectX::XMFLOAT2 position;
	DirectX::XMFLOAT4 color;
};

struct ConstantBuffer {
	DirectX::XMFLOAT4X4A mViewProjection;
	FLOAT fParticleSize;
};

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
CDXUTDialogResourceManager* g_pDialogResourceManager = NULL;
CDXUTTextHelper* g_pTxtHelper = NULL;
CDXUTDialog* g_pHUD = NULL;
const float PARTICLE_SIZE = 0.005f;
const unsigned int FluidVertexBufferSize = 4096;

ID3D11VertexShader* g_pVertexShader = NULL;
ID3D11GeometryShader* g_pGeometryShader = NULL;
ID3D11PixelShader* g_pPixelShader = NULL;
ID3D11Buffer* g_pVertexBuffer = NULL;
ID3D11InputLayout* g_pVertexLayout = NULL;
ID3D11Buffer* g_pConstantBuffer = NULL;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_RESETSIMULATION     2
#define IDC_PAUSESIMULATION     3
#define IDC_PAUSESIMULATIONONSTEP600 4
#define IDC_SIMULATIONSIZE      5
#define IDC_SIMULATIONSIZE_TEXT 6

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT CALLBACK OnResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnFrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnDestroyDevice( void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );

void InitApp();
void DestroyApp();
void RenderFluid( ID3D11DeviceContext* pd3dImmediateContext );
void RenderText( double fTime );


//--------------------------------------------------------------------------------------
// Initialization
//--------------------------------------------------------------------------------------
void InitializeDirectX( )
{
	DXUTSetCallbackD3D11DeviceCreated( OnCreateDevice );
	DXUTSetCallbackD3D11FrameRender( OnFrameRender );
	DXUTSetCallbackD3D11DeviceDestroyed( OnDestroyDevice );
	DXUTSetCallbackD3D11SwapChainResized( OnResizedSwapChain );
	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackFrameMove( OnFrameMove );

	DXUTSetCursorSettings( true, true );

	InitApp();

	DXUTInit( true, true );
	DXUTCreateWindow( L"CS391: Project 3" );
	DXUTCreateDevice( D3D_FEATURE_LEVEL_10_0, true, 640, 480 );
}


//--------------------------------------------------------------------------------------
// Initialization
//--------------------------------------------------------------------------------------
int RunDirectX( )
{
	DXUTMainLoop();

	DestroyApp();

	return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialization
//--------------------------------------------------------------------------------------
void InitApp()
{
	// Initialize dialogs
	g_pDialogResourceManager = new CDXUTDialogResourceManager();
	g_pHUD = new CDXUTDialog();
	g_pHUD->Init( g_pDialogResourceManager );

	// HUD
	g_pHUD->SetCallback( OnGUIEvent );
	int iY = 10;
	g_pHUD->AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 155, 22 );
	g_pHUD->AddButton( IDC_RESETSIMULATION, L"Reset Simulation", 35, iY += 24, 155, 22 );
	g_pHUD->AddCheckBox( IDC_PAUSESIMULATION, L"Pause Simulation", 35, iY += 24, 155, 22 );
	g_pHUD->AddCheckBox( IDC_PAUSESIMULATIONONSTEP600, L"Pause On Step 600", 35, iY += 24, 155, 22 );
	g_pHUD->AddStatic( IDC_SIMULATIONSIZE_TEXT, L"Simulation Size:", 35, iY += 24, 155, 22 );
	g_pHUD->AddSlider( IDC_SIMULATIONSIZE, 35, iY += 24, 155, 22, 1, 8, 5 );
}


//--------------------------------------------------------------------------------------
// Initialization
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;
	ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();

	V_RETURN( g_pDialogResourceManager->OnD3D11CreateDevice( pd3dDevice, pd3dImmediateContext ) );
	
	g_pTxtHelper = new CDXUTTextHelper( pd3dDevice, pd3dImmediateContext, g_pDialogResourceManager, 15 );

	V_RETURN( pd3dDevice->CreateVertexShader( g_ParticleVS, sizeof(g_ParticleVS), NULL, &g_pVertexShader ) );

	V_RETURN( pd3dDevice->CreateGeometryShader( g_ParticleGS, sizeof(g_ParticleGS), NULL, &g_pGeometryShader ) );

	V_RETURN( pd3dDevice->CreatePixelShader( g_ParticlePS, sizeof(g_ParticlePS), NULL, &g_pPixelShader ) );
	
	// Create VB
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory( &bufferDesc, sizeof(bufferDesc) );
	bufferDesc.ByteWidth = sizeof(ParticleVertex) * FluidVertexBufferSize;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	V_RETURN( pd3dDevice->CreateBuffer( &bufferDesc, NULL, &g_pVertexBuffer ) );

	bufferDesc.ByteWidth = sizeof(ConstantBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	V_RETURN( pd3dDevice->CreateBuffer( &bufferDesc, NULL, &g_pConstantBuffer ) );
	
	// Create the IA layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	V_RETURN( pd3dDevice->CreateInputLayout( layout, _countof(layout), g_ParticleVS, sizeof(g_ParticleVS), &g_pVertexLayout ) );
	
	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;

	V_RETURN( g_pDialogResourceManager->OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );

	g_pHUD->SetLocation( pBackBufferSurfaceDesc->Width - 200, 0 );
	g_pHUD->SetSize( 200, 170 );

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Render
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext )
{
	// Clear the render target and depth stencil
	auto pRTV = DXUTGetD3D11RenderTargetView();
	pd3dImmediateContext->ClearRenderTargetView( pRTV, DirectX::Colors::Gray );
	auto pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );
	
	pd3dImmediateContext->OMSetRenderTargets( 1, &pRTV, pDSV );

	// Fluid
	RenderFluid( pd3dImmediateContext );

	// HUD
	g_pHUD->OnRender( fElapsedTime );

	// FPS and Statistics
	RenderText( fTime );
}


//--------------------------------------------------------------------------------------
// Render the Fluid
//--------------------------------------------------------------------------------------
void RenderFluid( ID3D11DeviceContext* pd3dImmediateContext )
{
	DirectX::XMMATRIX proj = DirectX::XMMatrixOrthographicOffCenterLH(0, FLUID->Width(), FLUID->Height(), 0, 0, 1);
	proj = DirectX::XMMatrixTranspose(proj);

	D3D11_MAPPED_SUBRESOURCE MappedResource = {};
	pd3dImmediateContext->Map( g_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
	ConstantBuffer* pCB = reinterpret_cast<ConstantBuffer*>(MappedResource.pData);
	DirectX::XMStoreFloat4x4A( &pCB->mViewProjection, proj );
	pCB->fParticleSize = PARTICLE_SIZE;
	pd3dImmediateContext->Unmap( g_pConstantBuffer, 0 );

	pd3dImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
	pd3dImmediateContext->GSSetShader( g_pGeometryShader, NULL, 0 );
	pd3dImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );

	pd3dImmediateContext->GSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	
	pd3dImmediateContext->IASetInputLayout( g_pVertexLayout );
	pd3dImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	
	for (unsigned int i = 0; i < FLUID->Size(); i += FluidVertexBufferSize)
	{
		pd3dImmediateContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		ParticleVertex* pVertices = reinterpret_cast<ParticleVertex*>(MappedResource.pData);

		// Render each particle
		unsigned int j = 0;
		for (; j < FluidVertexBufferSize && i + j < FLUID->Size(); j++) {
			float color = (float)(0.5f * FLUID->density[i + j] / FluidRestDensity);
			color = (color > 1.0f) ? 1.0f : color;
			DirectX::XMFLOAT4 vColor(1.0f - color, 1.0f - color, 1.0f, 1.0f);
			DirectX::XMFLOAT2 vPosition(FLUID->pos[i + j].x, FLUID->pos[i + j].y);
			pVertices[j].position = vPosition;
			pVertices[j].color = vColor;
		}

		pd3dImmediateContext->Unmap(g_pVertexBuffer, 0);

		UINT stride = sizeof(ParticleVertex);
		UINT offset = 0;
		pd3dImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

		pd3dImmediateContext->Draw(j, 0);
	}
}


//--------------------------------------------------------------------------------------
// Render the FPS and statistics text
//--------------------------------------------------------------------------------------
void RenderText( double fTime )
{
	// Output statistics
	g_pTxtHelper->Begin();
	g_pTxtHelper->SetInsertionPos( 2, 0 );
	g_pTxtHelper->SetForegroundColor( DirectX::Colors::White );
	g_pTxtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
	g_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );

	g_pTxtHelper->SetForegroundColor( DirectX::Colors::White );
	g_pTxtHelper->DrawFormattedTextLine( L"Particles: %i", FLUID->Size() );
	g_pTxtHelper->DrawFormattedTextLine( L"Step: %i", FLUID->Step() );

	g_pTxtHelper->End();
}


//--------------------------------------------------------------------------------------
// Handles the input events
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
	// Always allow dialog resource manager calls to handle global messages
	// so GUI state is updated correctly
	*pbNoFurtherProcessing = g_pDialogResourceManager->MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 0;

	*pbNoFurtherProcessing = g_pHUD->MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 0;

	return 0;
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
	switch( nControlID )
	{
		case IDC_TOGGLEFULLSCREEN:
			DXUTToggleFullScreen(); break;
		case IDC_PAUSESIMULATION:
			FLUID->Pause( g_pHUD->GetCheckBox( IDC_PAUSESIMULATION )->GetChecked() ); break;
		case IDC_PAUSESIMULATIONONSTEP600:
			FLUID->PauseOnStep( (g_pHUD->GetCheckBox( IDC_PAUSESIMULATIONONSTEP600 )->GetChecked())? 600 : 0xFFFFFFFF ); break;
		case IDC_SIMULATIONSIZE:
			// fallthrough
		case IDC_RESETSIMULATION:
			FLUID->Fill( g_pHUD->GetSlider( IDC_SIMULATIONSIZE )->GetValue() / 10.0f );
			break;
	}
}


//--------------------------------------------------------------------------------------
// Cleanup 
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
	if( g_pDialogResourceManager )
		g_pDialogResourceManager->OnD3D11DestroyDevice();
	SAFE_DELETE( g_pTxtHelper );
	SAFE_RELEASE( g_pVertexShader );
	SAFE_RELEASE( g_pGeometryShader );
	SAFE_RELEASE( g_pPixelShader );
	SAFE_RELEASE( g_pVertexBuffer );
	SAFE_RELEASE( g_pVertexLayout );
	SAFE_RELEASE( g_pConstantBuffer );
}


//--------------------------------------------------------------------------------------
// Cleanup
//--------------------------------------------------------------------------------------
void DestroyApp()
{
	if( g_pDialogResourceManager )
		g_pDialogResourceManager->OnD3D11DestroyDevice();
	SAFE_DELETE( g_pDialogResourceManager );
	SAFE_DELETE( g_pHUD );
}
