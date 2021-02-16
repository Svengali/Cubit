//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
//
// copyright 2005-2008 Marc Hernandez

#include "../stdafx.h"

#include "PhamApp.h"
#include "imgui.h"

namespace Diligent
{
SampleBase *CreateSample()
{
	return new PhamApp();
}

}

PhamApp::~PhamApp()
{
}

void PhamApp::GetEngineInitializationAttribs( dg::RENDER_DEVICE_TYPE DeviceType,
	dg::EngineCreateInfo &EngineCI,
	dg::SwapChainDesc &SCDesc )
{
	SampleBase::GetEngineInitializationAttribs( DeviceType, EngineCI, SCDesc );

	EngineCI.Features.DepthClamp = dg::DEVICE_FEATURE_STATE_OPTIONAL;

#if D3D12_SUPPORTED
	if( DeviceType == dg::RENDER_DEVICE_TYPE_D3D12 )
	{
		auto &D3D12CI                           = static_cast<dg::EngineD3D12CreateInfo &>(EngineCI);
		D3D12CI.GPUDescriptorHeapSize[1]        = 1024; // Sampler descriptors
		D3D12CI.GPUDescriptorHeapDynamicSize[1] = 1024;
	}
#endif
}


void PhamApp::Initialize( const dg::SampleInitInfo &InitInfo )
{
	dg::SampleBase::Initialize( InitInfo );


}

static bool s_timeBlocks = false;

static i32 s_numLoops = 2;
static i32 s_numEnts  = 1000;
static u32 s_timeSoAIndividualuS = 0;
static u32 s_timeAoSuS = 0;
static u32 s_timeSoA3uS = 0;
static u32 s_timeSoA4uS = 0;

void PhamApp::UpdateUI()
{





	{
		ImGui::Begin( "Pham" );

		ImGui::Checkbox( "Time Blocks", &s_timeBlocks ); // Edit bools storing our window open/close state

		//if( s_timeBlocks )
		{
			ImGui::SliderInt( "s_numLoops", &s_numLoops, 0, 10 );
			ImGui::SliderInt( "s_numEnts", &s_numEnts, 0, 1000000 );

			df::timeBlocks( s_numLoops, s_numEnts, &s_timeSoAIndividualuS, &s_timeAoSuS, &s_timeSoA3uS, &s_timeSoA4uS );

			ImGui::Text( "Time in uS, SoAFine %i | AoS %i | SoA 3 %i | SoA 4 %i", s_timeSoAIndividualuS, s_timeAoSuS, s_timeSoA3uS, s_timeSoA4uS );
		}

		ImGui::End();
	}

}

// Render a frame
void PhamApp::Render()
{


	auto *pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
	m_pImmediateContext->ClearRenderTarget( pRTV, &m_ClearColor.x, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION );
}


void PhamApp::Update( double CurrTime, double ElapsedTime )
{
	SampleBase::Update( CurrTime, ElapsedTime );

	UpdateUI();
}

void PhamApp::WindowResize( dg::Uint32 Width, dg::Uint32 Height )
{
}

