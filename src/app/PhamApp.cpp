//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#include "../stdafx.h"

#include "PhamApp.h"
#include "imgui.h"
#include "imgui/imgui_node_editor.h"

#include "res/ResDGLayout.h"
#include "res/ResDGPipelineState.h"
#include "grx/Generate.h"

#include "grx/DGRenderer.h"
#include "grx/GeoDiligent.h"

#include "df/comTest.h"

void Application_Frame();
void Application_Initialize();


namespace ed = ax::NodeEditor;

// Struct to hold basic information about connection between
// pins. Note that connection (aka. link) has its own ID.
// This is useful later with dealing with selections, deletion
// or other operations.
struct LinkInfo
{
	ed::LinkId Id;
	ed::PinId  InputId;
	ed::PinId  OutputId;
};


static ImVector<LinkInfo>   g_Links;                // List of live links. It is dynamic unless you want to create read-only view over nodes.

static ed::EditorContext *g_Context = nullptr;
static int                  g_NextLinkId = 100;     // Counter to help generate link ids. In real application this will probably based on pointer to user data structure.


dg::float3 c( const cb::Vec3 &v )
{
	return dg::float3( v.x, v.y, v.z );
}

static PhamApp *s_pApp = nullptr;


namespace Diligent
{
	SampleBase *CreateSample()
	{
		auto *pApp = new PhamApp();
		s_pApp = pApp;
		return pApp;
	}

}



PhamApp &PhamApp::Info()
{
	return *s_pApp;
}





PhamApp::~PhamApp()
{
	ed::DestroyEditor( g_Context );
	DGRenderer::shutdown();
}

void PhamApp::ModifyEngineInitInfo( const ModifyEngineInitInfoAttribs &Attribs )
{
	SampleBase::ModifyEngineInitInfo( Attribs );
	
	Attribs.EngineCI.NumDeferredContexts = 32; //std::max( std::thread::hardware_concurrency() - 1, 2u );

#if VULKAN_SUPPORTED
	if( Attribs.DeviceType == dg::RENDER_DEVICE_TYPE_VULKAN )
	{
		auto &EngineVkCI = static_cast<dg::EngineVkCreateInfo &>( Attribs.EngineCI );
		EngineVkCI.DynamicHeapSize = 26 << 20; // Enough space for 32x32x32x256 bytes allocations for 3 frames
	}
#endif

#if D3D12_SUPPORTED
	if( Attribs.DeviceType == dg::RENDER_DEVICE_TYPE_D3D12 )
	{
		auto &D3D12CI = static_cast<dg::EngineD3D12CreateInfo &>( Attribs.EngineCI );
		D3D12CI.GPUDescriptorHeapSize[1] = 1 * 1024; // Sampler descriptors
		D3D12CI.GPUDescriptorHeapDynamicSize[1] = 1 * 1024;
	}
#endif

}

/*
void PhamApp::GetEngineInitializationAttribs(
	dg::RENDER_DEVICE_TYPE DeviceType,
	dg::EngineCreateInfo &EngineCI,
	dg::SwapChainDesc &SCDesc )
{
	SampleBase::GetEngineInitializationAttribs( DeviceType, EngineCI, SCDesc );

	EngineCI.Features.DepthClamp = dg::DEVICE_FEATURE_STATE_OPTIONAL;

	EngineCI.NumDeferredContexts = 32;

#if D3D12_SUPPORTED
	if( DeviceType == dg::RENDER_DEVICE_TYPE_D3D12 )
	{
		auto &D3D12CI = static_cast<dg::EngineD3D12CreateInfo &>( EngineCI );
		D3D12CI.GPUDescriptorHeapSize[1] = 1 * 1024; // Sampler descriptors
		D3D12CI.GPUDescriptorHeapDynamicSize[1] = 1 * 1024;
	}
#endif

#if VULKAN_SUPPORTED
		if( DeviceType == dg::RENDER_DEVICE_TYPE_VULKAN )
		{
			auto &VkCI = static_cast<dg::EngineVkCreateInfo &>( EngineCI );

			VkCI.DynamicHeapSize = 26 << 20; // Enough space for 32x32x32x256 bytes allocations for 3 frames

			//VkCI.GPUDescriptorHeapSize[1] = 1 * 1024; // Sampler descriptors
			//VkCI.GPUDescriptorHeapDynamicSize[1] = 1 * 1024;
		}
#endif



}
*/


void Style_MarcGreen()
{

	ImGuiStyle &style = ImGui::GetStyle();
	style.ChildRounding = 0;
	style.FrameRounding = 0;
	style.GrabRounding = 0;
	style.PopupRounding = 0;
	//style.ScrollbarRounding = 0;
	style.TabRounding = 0;
	style.WindowRounding = 0;

	style.ChildBorderSize = 1;
	style.FrameBorderSize = 1;
	style.PopupBorderSize = 1;
	style.TabBorderSize = 1;
	style.WindowBorderSize = 1;

	style.Colors[ImGuiCol_Text] = ImVec4( 0.90f, 0.90f, 0.90f, 0.90f );//ImGuiCol_Text,
	style.Colors[ImGuiCol_TextDisabled] = ImVec4( 0.60f, 0.60f, 0.60f, 1.00f );//ImGuiCol_TextDisabled,
	style.Colors[ImGuiCol_WindowBg] = ImVec4( 0.09f, 0.15f, 0.09f, 1.00f );//ImGuiCol_WindowBg,            
	style.Colors[ImGuiCol_ChildBg] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );//ImGuiCol_ChildBg,             
	style.Colors[ImGuiCol_PopupBg] = ImVec4( 0.05f, 0.10f, 0.05f, 0.85f );//ImGuiCol_PopupBg,             
	style.Colors[ImGuiCol_Border] = ImVec4( 0.70f, 0.70f, 0.70f, 0.65f );//ImGuiCol_Border,
	style.Colors[ImGuiCol_BorderShadow] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );//ImGuiCol_BorderShadow,
	style.Colors[ImGuiCol_FrameBg] = ImVec4( 0.00f, 0.01f, 0.00f, 1.00f );//ImGuiCol_FrameBg,             
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.90f, 0.80f, 0.80f, 0.40f );//ImGuiCol_FrameBgHovered,
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4( 0.90f, 0.65f, 0.65f, 0.45f );//ImGuiCol_FrameBgActive,
	style.Colors[ImGuiCol_TitleBg] = ImVec4( 0.00f, 0.00f, 0.00f, 0.83f );//ImGuiCol_TitleBg,
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 0.40f, 0.80f, 0.40f, 0.20f );//ImGuiCol_TitleBgActive,
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4( 0.00f, 0.00f, 0.00f, 0.87f );//ImGuiCol_TitleBgCollapsed,
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4( 0.01f, 0.02f, 0.01f, 0.80f );//ImGuiCol_MenuBarBg,
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.20f, 0.30f, 0.25f, 0.60f );//ImGuiCol_ScrollbarBg,
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.55f, 0.55f, 0.53f, 0.51f );//ImGuiCol_ScrollbarGrab,
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.56f, 0.56f, 0.56f, 1.00f );//ImGuiCol_ScrollbarGrabHovered,
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.56f, 0.56f, 0.56f, 0.91f );//ImGuiCol_ScrollbarGrabActive,
	style.Colors[ImGuiCol_CheckMark] = ImVec4( 0.90f, 0.90f, 0.90f, 0.83f );//ImGuiCol_CheckMark,
	style.Colors[ImGuiCol_SliderGrab] = ImVec4( 0.70f, 0.70f, 0.70f, 0.62f );//ImGuiCol_SliderGrab,
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4( 0.30f, 0.30f, 0.30f, 0.84f );//ImGuiCol_SliderGrabActive,
	style.Colors[ImGuiCol_Button] = ImVec4( 0.48f, 0.89f, 0.72f, 0.49f );//ImGuiCol_Button,
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4( 0.50f, 0.99f, 0.69f, 0.68f );//ImGuiCol_ButtonHovered,
	style.Colors[ImGuiCol_ButtonActive] = ImVec4( 0.80f, 0.50f, 0.50f, 1.00f );//ImGuiCol_ButtonActive,
	style.Colors[ImGuiCol_Header] = ImVec4( 0.30f, 1.00f, 0.69f, 0.53f );//ImGuiCol_Header,              
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4( 0.44f, 0.86f, 0.61f, 1.00f );//ImGuiCol_HeaderHovered,
	style.Colors[ImGuiCol_HeaderActive] = ImVec4( 0.38f, 0.83f, 0.62f, 1.00f );//ImGuiCol_HeaderActive,

//style.Colors[ImGuiCol_Separator]             = ImVec4( 0.38f, 0.83f, 0.62f, 1.00f );//ImGuiCol_Separator,
//style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4( 0.38f, 0.83f, 0.62f, 1.00f );//ImGuiCol_SeparatorHovered,
//style.Colors[ImGuiCol_SeparatorActive]       = ImVec4( 0.38f, 0.83f, 0.62f, 1.00f );//ImGuiCol_SeparatorActive,

	style.Colors[ImGuiCol_ResizeGrip] = ImVec4( 1.00f, 1.00f, 1.00f, 0.85f );//ImGuiCol_ResizeGrip,
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4( 1.00f, 1.00f, 1.00f, 0.60f );//ImGuiCol_ResizeGripHovered,
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4( 1.00f, 1.00f, 1.00f, 0.90f );//ImGuiCol_ResizeGripActive,
//ImGuiCol_Tab,
//ImGuiCol_TabHovered,
//ImGuiCol_TabActive,
//ImGuiCol_TabUnfocused,
	style.Colors[ImGuiCol_PlotLines] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );																																											//ImGuiCol_TabUnfocusedActive,
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4( 0.90f, 0.00f, 0.70f, 1.00f );																																											//ImGuiCol_PlotLines,
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4( 0.90f, 0.00f, 0.70f, 1.00f );																																																																																						//ImGuiCol_PlotLinesHovered,
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 1.00f, 0.00f, 0.60f, 1.00f );																																											//ImGuiCol_PlotHistogram,
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.00f, 1.00f, 0.00f, 0.35f );																																											//ImGuiCol_PlotHistogramHovered,
	//style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );																																											//ImGuiCol_TextSelectedBg,
																																											//ImGuiCol_DragDropTarget,
																																											//ImGuiCol_NavHighlight,        
																																											//ImGuiCol_NavWindowingHighlight
																																											//ImGuiCol_NavWindowingDimBg,   
																																											//ImGuiCol_ModalWindowDimBg,    

}

void Style_DarkGraphite()
{
	ImGuiStyle *style = &ImGui::GetStyle();
	ImVec4 *colors = style->Colors;

	colors[ImGuiCol_Text] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_TextDisabled] = ImVec4( 0.40f, 0.40f, 0.40f, 1.00f );
	colors[ImGuiCol_ChildBg] = ImVec4( 0.25f, 0.25f, 0.25f, 1.00f );
	colors[ImGuiCol_WindowBg] = ImVec4( 0.25f, 0.25f, 0.25f, 1.00f );
	colors[ImGuiCol_PopupBg] = ImVec4( 0.25f, 0.25f, 0.25f, 1.00f );
	colors[ImGuiCol_Border] = ImVec4( 0.12f, 0.12f, 0.12f, 0.90f );
	colors[ImGuiCol_BorderShadow] = ImVec4( 1.00f, 1.00f, 1.00f, 0.06f );
	colors[ImGuiCol_FrameBg] = ImVec4( 0.42f, 0.42f, 0.42f, 0.54f );
	colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.42f, 0.42f, 0.42f, 0.40f );
	colors[ImGuiCol_FrameBgActive] = ImVec4( 0.56f, 0.56f, 0.56f, 0.67f );
	colors[ImGuiCol_TitleBg] = ImVec4( 0.19f, 0.19f, 0.19f, 1.00f );
	colors[ImGuiCol_TitleBgActive] = ImVec4( 0.22f, 0.22f, 0.22f, 1.00f );
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 0.17f, 0.17f, 0.17f, 0.90f );
	colors[ImGuiCol_MenuBarBg] = ImVec4( 0.335f, 0.335f, 0.335f, 1.000f );
	colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.24f, 0.24f, 0.24f, 0.53f );
	colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.41f, 0.41f, 0.41f, 1.00f );
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.52f, 0.52f, 0.52f, 1.00f );
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.76f, 0.76f, 0.76f, 1.00f );
	colors[ImGuiCol_CheckMark] = ImVec4( 0.65f, 0.65f, 0.65f, 1.00f );
	colors[ImGuiCol_SliderGrab] = ImVec4( 0.52f, 0.52f, 0.52f, 1.00f );
	colors[ImGuiCol_SliderGrabActive] = ImVec4( 0.64f, 0.64f, 0.64f, 1.00f );
	colors[ImGuiCol_Button] = ImVec4( 0.54f, 0.54f, 0.54f, 0.35f );
	colors[ImGuiCol_ButtonHovered] = ImVec4( 0.52f, 0.52f, 0.52f, 0.59f );
	colors[ImGuiCol_ButtonActive] = ImVec4( 0.76f, 0.76f, 0.76f, 1.00f );
	colors[ImGuiCol_Header] = ImVec4( 0.38f, 0.38f, 0.38f, 1.00f );
	colors[ImGuiCol_HeaderHovered] = ImVec4( 0.47f, 0.47f, 0.47f, 1.00f );
	colors[ImGuiCol_HeaderActive] = ImVec4( 0.76f, 0.76f, 0.76f, 0.77f );
	colors[ImGuiCol_Separator] = ImVec4( 0.000f, 0.000f, 0.000f, 0.137f );
	colors[ImGuiCol_SeparatorHovered] = ImVec4( 0.700f, 0.671f, 0.600f, 0.290f );
	colors[ImGuiCol_SeparatorActive] = ImVec4( 0.702f, 0.671f, 0.600f, 0.674f );
	colors[ImGuiCol_ResizeGrip] = ImVec4( 0.26f, 0.59f, 0.98f, 0.25f );
	colors[ImGuiCol_ResizeGripHovered] = ImVec4( 0.26f, 0.59f, 0.98f, 0.67f );
	colors[ImGuiCol_ResizeGripActive] = ImVec4( 0.26f, 0.59f, 0.98f, 0.95f );
	colors[ImGuiCol_PlotLines] = ImVec4( 0.61f, 0.61f, 0.61f, 1.00f );
	colors[ImGuiCol_PlotLinesHovered] = ImVec4( 1.00f, 0.43f, 0.35f, 1.00f );
	colors[ImGuiCol_PlotHistogram] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
	colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.73f, 0.73f, 0.73f, 0.35f );
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4( 0.80f, 0.80f, 0.80f, 0.35f );
	colors[ImGuiCol_DragDropTarget] = ImVec4( 1.00f, 1.00f, 0.00f, 0.90f );
	colors[ImGuiCol_NavHighlight] = ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4( 0.80f, 0.80f, 0.80f, 0.20f );

	/*
	style->ChildRounding = 4.0f;
	style->FrameBorderSize = 1.0f;
	style->FrameRounding = 2.0f;
	style->GrabMinSize = 7.0f;
	style->PopupRounding = 2.0f;
	style->ScrollbarRounding = 12.0f;
	style->ScrollbarSize = 13.0f;
	style->TabBorderSize = 1.0f;
	style->TabRounding = 0.0f;
	style->WindowRounding = 4.0f;
	//*/
}

void PhamApp::Initialize( const dg::SampleInitInfo &InitInfo )
{
	cb::LogOpen( nullptr, nullptr );

	lprintf( "Starting up Cubit\n" );
	lprintf( "Test\n" );

	auto now = std::chrono::system_clock::now();
	std::time_t ctimeNow = std::chrono::system_clock::to_time_t( now );



	const auto nowStr = std::ctime( &ctimeNow );
	lprintf( "Starting at %s\n", nowStr );

	const i32 srandValue = (i32)ctimeNow;

	lprintf( "Setting srand to %i\n", srandValue );

	srand( srandValue );

	//Modify this later.
	//enki::TaskSchedulerConfig enkiCfg;

	PhamApp::Info().Task.Initialize( RCDiligent::k_maxThreads );



	enki::TaskSet task( 1024, 
		[]( enki::TaskSetPartition range, uint32_t threadnum ) { 
			lprintf( "Thread %d, start %d, end %d\n", threadnum, range.start, range.end ); 
		});


	PhamApp::Info().Task.AddTaskSetToPipe( &task );
	PhamApp::Info().Task.WaitforTask( &task );


	m_Camera.SetSpeedUpScales( 6.0f, 20.0f );


	ent::EntityId::initStartingEntityId( 1024 );

	dg::SampleBase::Initialize( InitInfo );


	const auto &SCDesc = m_pSwapChain->GetDesc();

	float NearPlane = 0.1f;
	float FarPlane = 10000.f;
	float AspectRatio = static_cast<float>( SCDesc.Width ) / static_cast<float>( SCDesc.Height );
	m_Camera.SetProjAttribs( NearPlane, FarPlane, AspectRatio, dg::PI_F / 4.f,
		m_pSwapChain->GetDesc().PreTransform, m_pDevice->GetDeviceInfo().IsGLDevice() );


	m_Camera.SetReferenceAxes( dg::float3( 1.0f, 0.0f, 0.0f ), dg::float3( 0.0f, 0.0f, 1.0f ), true );

	m_Camera.SetPos( dg::float3( 1.71f, 3.84f, 10.90f ) );

	m_Camera.SetLookAt( dg::float3( 2.00f, 3.84f, 9.94f ) );




	ImGuiStyle &style = ImGui::GetStyle();
	style.ChildRounding = 0;
	style.FrameRounding = 0;
	style.GrabRounding = 0;
	style.PopupRounding = 0;
	//style.ScrollbarRounding = 0;
	style.TabRounding = 0;
	style.WindowRounding = 0;

	style.ChildBorderSize = 1;
	style.FrameBorderSize = 1;
	style.PopupBorderSize = 1;
	style.TabBorderSize = 1;
	style.WindowBorderSize = 1;

	Style_DarkGraphite();


	DGRenderer::startup();


	ed::Config config;
	config.SettingsFile = "Simple.json";
	g_Context = ed::CreateEditor( &config );


	//Application_Initialize();


	/*
	VertPosUV verts;

	std::vector<dg::LayoutElement> layout;

	StructToLayout( verts, &layout );

	const auto vecLayout = ResDGLayout::create( layout );

	ResourceMgr::AddResource( "+VertPosUV.layout", vecLayout );

	const auto vecLayoutLU = ResourceMgr::LookupResource( "+VertPosUV.layout" );
	*/

	/*
	*/

	/*
	{
		GeoDiligentCfgPtr cfg0;
		{
			cfg0 = ResourceMgr::GetResource<GeoDiligentCfg>( "config/geo/test.xml" );

			{
				GeoDiligentCfgPtr cfg1 = ResourceMgr::GetResource<GeoDiligentCfg>( "config/geo/test.xml" );
				GeoDiligentCfgPtr cfg2 = ResourceMgr::GetResource<GeoDiligentCfg>( "config/geo/test.xml" );
				GeoDiligentCfgPtr cfg3 = ResourceMgr::GetResource<GeoDiligentCfg>( "config/geo/test.xml" );
				GeoDiligentCfgPtr cfg4 = ResourceMgr::GetResource<GeoDiligentCfg>( "config/geo/test.xml" );
				GeoDiligentCfgPtr cfg5 = ResourceMgr::GetResource<GeoDiligentCfg>( "config/geo/test.xml" );
			}
		}
	}
	//*/


	const auto cubeVerts = grx::gen::createCenteredCubeVertices( 0.5f );
	const auto cubeIndicies = grx::gen::createCenteredCubeIndicies();

	ResourceMgr::AddResource( "+gen:0.5.verts", cubeVerts );
	ResourceMgr::AddResource( "+gen:0.5.indices", cubeIndicies );


	m_cubit = vox::CubitPlanePtr( new vox::CubitPlane() );

	m_freefall = FreefallPlanePtr( new FreefallPlane() );


	//*
	{

		cb::Vec3 pos( 0.0f, 0.0f, 0.0f );

		m_cubit->genWorld( pos );

		m_cubit->genGeo( pos );
	}
	//*/

#ifdef _DEBUG
	// We do 2 sets of these, so double it
	const i32 k_maxMovingObjects = 200;
#else
	// We do 2 sets of these, so double it
	const i32 k_maxMovingObjects = 50000;
#endif


	//*
	lprintf( "Create %i moving objects\n", 2 * k_maxMovingObjects );
	{
		/*
		const auto center = cb::Vec3( 50.0f, 0.0f, 50.0f );
		const auto size  =  cb::Vec3( 50.0f, 0.0f, 10.0f );
		/*/
		const auto center = cb::Vec3( 300.0f, 300.0f, 40.0f );
		const auto size = cb::Vec3( 300.0f, 300.0f, 10.0f );
		//*/

		for( i32 i = 0; i < k_maxMovingObjects; ++i )
		{
			const auto id = ent::EntityId::makeNext();

			const auto pos = cb::MakeRandomInBox( center, size );

			GeoDiligentCfgPtr cfg = ResourceMgr::GetResource<GeoDiligentCfg>( "config/geo/vehicle.xml" );
			GeoDiligentPtr geo = GeoDiligentPtr( new GeoDiligent( id, cfg ) );

			auto rot = cb::Mat3( cb::Mat3::eIdentity );

			f32 uniform = cast<f32>( rand() % 0xff ) / 255.0f;

			cb::SetZRotation( &rot, CB_PIf * ( uniform ) );

			const cb::Frame3 frame( rot, pos );

			const cb::Vec3 vel( uniform * 1.25f, 0.0f, 0.0f );

			m_freefall->add( pos, id, geo, frame, vel );
		}
	}
	//*/

	//*
	{
		/*
		const auto center = cb::Vec3( 00.0f, 50.0f, 50.0f );
		const auto size   = cb::Vec3( 00.0f, 50.0f, 10.0f );
		/*/
		const auto center = cb::Vec3( 300.0f, 300.0f, 40.0f );
		const auto size = cb::Vec3( 300.0f, 300.0f, 10.0f );
		//*/

		for( i32 i = 0; i < k_maxMovingObjects; ++i )
		{
			const auto id = ent::EntityId::makeNext();

			const auto pos = cb::MakeRandomInBox( center, size );

			GeoDiligentCfgPtr cfg = ResourceMgr::GetResource<GeoDiligentCfg>( "config/geo/vehicle.xml" );
			GeoDiligentPtr geo = GeoDiligentPtr( new GeoDiligent( id, cfg ) );

			auto rot = cb::Mat3( cb::Mat3::eIdentity );

			cb::SetZRotation( &rot, CB_PIf * ( ((f32)i / 65.0f) ) );

			const cb::Frame3 frame( rot, pos );

			const cb::Vec3 vel( 0.1f, 0.0f, 0.0f );

			m_freefall->add( pos, id, geo, frame, vel );
		}
	}
	lprintf( "Done creating %i moving objects\n", 2 * k_maxMovingObjects );
	//*/



	DGRenderer::Inst().m_rsFreefall->m_freefall = m_freefall;


	/*
	const i32 width = 1;
	const i32 hight = 1;

	for( i32 iy = -hight; iy < hight + 1; ++iy )
	{
		const auto y = cast<f32>( iy );

		if( ( iy % ( 1000 / (width+1) ) ) == 0 )
		{
			OutputDebugString( "Ping\n" );

			if( PhamApp::Info().Barriers.size() > 0 )
			{
				m_pImmediateContext->TransitionResourceStates( static_cast<u32>( PhamApp::Info().Barriers.size() ), PhamApp::Info().Barriers.data() );
				for( auto context : PhamApp::Info().DeferredContexts() )
				{
					context->TransitionResourceStates( static_cast<u32>( PhamApp::Info().Barriers.size() ), PhamApp::Info().Barriers.data() );
				}
				PhamApp::Info().Barriers.clear();
			}

			//std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

		}

		for( i32 ix = 0; ix < width + 1; ++ix )
		{
			const auto id = ent::EntityId::makeNext();

			const auto x = cast<f32>( ix );

			cb::Mat3 mat( cb::Mat3::eIdentity );

			//cb::SetZRotation( &mat, CB_PIf * ( x + y ) / 50.0f );

			cb::Vec3 cubePos( x * 10, y, -1.0f );
			cb::Frame3 frame( mat, cubePos );


			//const dg::float4x4 dgTrans = dg::float4x4::RotationZ( CB_PIf * 0.25f ) * dg::float4x4::Translation( c(pos) );

			//const cb::Mat4 mat4( frame );


			//const dg::float4x4 another = dg::float4x4::MakeMatrix( mat4.GetData() );
			GeoDiligentCfgPtr cfg = ResourceMgr::GetResource<GeoDiligentCfg>( "config/geo/test.xml" );

			// PSO FIX
			//auto pso = ResDGPipelineState::createRaw( cfg->m_vertexShader, cfg->m_pixelShader, cfg->m_namedBuffers.front().m_buffer );

			GeoDiligentPtr geo = GeoDiligentPtr( new GeoDiligent( id, cfg ) );


			DGRenderer::Inst().addStaticGeo( frame, geo );

		}

		//ResourceMgr::RemResource( "config/geo/test.xml" );

	}
	//*/

	//const auto pso = ResDGPipelineState::create( )

	// BUG BUG BUG BUG BUG BUG
	// 
	// If this is turned on, the WORLD rotates!  
	// 
	// BUG BUG BUG BUG BUG BUG


	//* Single Geo test
	const auto id = ent::EntityId::makeNext();

	const auto x = cast<f32>( 10 );

	cb::Mat3 mat( cb::Mat3::eIdentity );

	cb::SetZRotation( &mat, CB_PIf * ( x + 1 ) / 50.0f );

	cb::Vec3 pos( x, 1, 0 );
	cb::Frame3 frame( mat, pos );


	//const dg::float4x4 dgTrans = dg::float4x4::RotationZ( CB_PIf * 0.25f ) * dg::float4x4::Translation( c(pos) );

	//const cb::Mat4 mat4( frame );


	//const dg::float4x4 another = dg::float4x4::MakeMatrix( mat4.GetData() );
	GeoDiligentCfgPtr cfg = ResourceMgr::GetResource<GeoDiligentCfg>( "config/geo/test.xml" );

	// PSO FIX
	//auto pso = ResDGPipelineState::createRaw( cfg->m_vertexShader, cfg->m_pixelShader, cfg->m_namedBuffers.front().m_buffer );

	GeoDiligentPtr geo = GeoDiligentPtr( new GeoDiligent( id, cfg ) );


	DGRenderer::Inst().m_rsCubes->add( frame, geo );
	//*/


}

void node( const u32 nodeIdBase )
{

	u32 nodeId = nodeIdBase * 10;

	ed::BeginNode( nodeId++ );
	ImGui::Text( "Node %i", nodeIdBase );
	ed::BeginPin( nodeId++, ed::PinKind::Input );
	ImGui::Text( "-> In" );
	ed::EndPin();

	ImGui::SameLine();
	ed::BeginPin( nodeId++, ed::PinKind::Output );
	ImGui::Text( "Out ->" );
	ed::EndPin();


	ed::BeginPin( nodeId++, ed::PinKind::Input );
	ImGui::Text( "-> In" );
	ed::EndPin();


	ImGui::SameLine();
	ed::BeginPin( nodeId++, ed::PinKind::Output );
	ImGui::Text( "Out ->" );
	ed::EndPin();

	ed::BeginPin( nodeId++, ed::PinKind::Input );
	ImGui::Text( "-> In" );
	ed::EndPin();

	ed::EndNode();
}

static bool s_showDemoWindow = false;

static bool s_timeBlocks = false;

static i32 s_numLoops = 2;
static i32 s_numEnts = 1000;
static u32 s_timeSoAIndividualuS = 0;
static u32 s_timeAoSuS = 0;
static u32 s_timeSoA3uS = 0;
static u32 s_timeSoA4uS = 0;

static i32							s_atTestSize = 1000;
static i32							s_atTestCalls = 1000;
static std::vector<i32> s_atTest;
static i32							s_frameNum = 0;

void PhamApp::UpdateUI()
{

	if( s_showDemoWindow )
	{
		ImGui::ShowDemoWindow( &m_ShowDemoWindow );
	}

	ImGui::GetIO().WantCaptureKeyboard = false;

#if 0
	{
		//ImGuiIO& io = ImGui::GetIO();

		/*
		//ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
		//ImGui::SetNextWindowSize( io.DisplaySize );

		ImGui::Begin( "Content", nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoBringToFrontOnFocus );
		/*/
		ImGui::SetNextWindowSize( ImVec2( 256, 256 ) );
		ImGui::Begin( "Content" );
		//*/

		//Application_Frame();

		ImGui::End();

	}
#endif



	/*
	{
		ImGui::Begin( "Pham" );

		ImGui::Checkbox( "Show Demo Window", &s_showDemoWindow ); // Edit bools storing our window open/close state
		ImGui::SameLine();
		ImGui::Checkbox( "Time Blocks", &s_timeBlocks ); // Edit bools storing our window open/close state

		if( s_timeBlocks )
		{
			df::timeBlocks( s_numLoops, s_numEnts, &s_timeSoAIndividualuS, &s_timeAoSuS, &s_timeSoA3uS, &s_timeSoA4uS );
		}

		{
			ImGui::SliderInt( "s_numLoops", &s_numLoops, 0, 10 );
			ImGui::SliderInt( "s_numEnts", &s_numEnts, 0, 1000000 );

			ImGui::Text( "Time in uS, SoAFine %i | AoS %i | SoA 3 %i | SoA 4 %i", s_timeSoAIndividualuS, s_timeAoSuS, s_timeSoA3uS, s_timeSoA4uS );
		}

		ImGui::End();
	}
	*/

	/*
	{
		ImGui::Begin( "At Test" );

		ImGui::SliderInt( "s_atTestSize", &s_atTestSize, 1, 100000000 );
		ImGui::SliderInt( "s_atTestCalls", &s_atTestCalls, 1, 100000000 );


		if( s_atTest.size() != s_atTestSize )
		{
			s_atTest.resize( s_atTestSize );

			for( i32 i = 0; i < s_atTest.size(); ++i )
			{
				s_atTest[i] = rand();
			}
		}

		i32 accumBrace = 0;

		const auto timeBrace = Timer<>::execution( [&]() {
			for( int32_t i = 0; i < s_atTestCalls; ++i )
			{
				const i32 val = s_atTest[i % s_atTestSize];

				accumBrace += val;
			}
			} );

		const float fTimeBraceUs = (float)timeBrace;
		const float fTimeBraceMs = fTimeBraceUs / 1000.0f;


		i32 accumMath = 0;

		i32* pStart = &s_atTest[0];

		const auto timeMath = Timer<>::execution( [&]() {
			for( int32_t i = 0; i < s_atTestCalls; ++i )
			{
				const i32 val = *( pStart + ( i % s_atTestSize ) );

				accumMath += val;
			}
			} );

		const float fTimeMathUs = (float)timeMath;
		const float fTimeMathMs = fTimeMathUs / 1000.0f;


		ImGui::Text( "Ms Brace %.3f  Math %.3f\n", fTimeBraceMs, fTimeMathMs );
		ImGui::Text( "Accum %i %i\n", accumBrace, accumMath );
		ImGui::End();
	}
	*/

	//*
	{
		ImGui::Begin( "Nodes" );
		ed::SetCurrentEditor( g_Context );
		ed::Begin( "My Editor", ImVec2( 0.0, 0.0f ) );

		u32 uniqueId = 1;

		node( uniqueId++ );
		node( uniqueId++ );
		node( uniqueId++ );
		node( uniqueId++ );






		// Submit Links
		for( auto &linkInfo : g_Links )
			ed::Link( linkInfo.Id, linkInfo.InputId, linkInfo.OutputId );


		// Handle creation action, returns true if editor want to create new object (node or link)
		if( ed::BeginCreate() )
		{
			ed::PinId inputPinId, outputPinId;
			if( ed::QueryNewLink( &inputPinId, &outputPinId ) )
			{
				// QueryNewLink returns true if editor want to create new link between pins.
				//
				// Link can be created only for two valid pins, it is up to you to
				// validate if connection make sense. Editor is happy to make any.
				//
				// Link always goes from input to output. User may choose to drag
				// link from output pin or input pin. This determine which pin ids
				// are valid and which are not:
				//   * input valid, output invalid - user started to drag new ling from input pin
				//   * input invalid, output valid - user started to drag new ling from output pin
				//   * input valid, output valid   - user dragged link over other pin, can be validated

				if( inputPinId && outputPinId ) // both are valid, let's accept link
				{
					// ed::AcceptNewItem() return true when user release mouse button.
					if( ed::AcceptNewItem() )
					{
						// Since we accepted new link, lets add one to our list of links.
						g_Links.push_back( { ed::LinkId( g_NextLinkId++ ), inputPinId, outputPinId } );

						// Draw new link.
						ed::Link( g_Links.back().Id, g_Links.back().InputId, g_Links.back().OutputId );
					}

					// You may choose to reject connection between these nodes
					// by calling ed::RejectNewItem(). This will allow editor to give
					// visual feedback by changing link thickness and color.
				}
			}
		}
		ed::EndCreate(); // Wraps up object creation action handling.


		// Handle deletion action
		if( ed::BeginDelete() )
		{
			// There may be many links marked for deletion, let's loop over them.
			ed::LinkId deletedLinkId;
			while( ed::QueryDeletedLink( &deletedLinkId ) )
			{
				// If you agree that link can be deleted, accept deletion.
				if( ed::AcceptDeletedItem() )
				{
					// Then remove link from your data.
					for( auto &link : g_Links )
					{
						if( link.Id == deletedLinkId )
						{
							g_Links.erase( &link );
							break;
						}
					}
				}

				// You may reject link deletion by calling:
				// ed::RejectDeletedItem();
			}
		}
		ed::EndDelete(); // Wrap up deletion action
		//*/


		//*
		{
			ImGui::Begin( "World" );

			const auto pos = m_Camera.GetPos();
			const auto fwd = pos + m_Camera.GetWorldAhead();

			ImGui::Text( "Pos %.2f, %.2f, %.2f  ", pos.x, pos.y, pos.z );
			ImGui::Text( "Fwd %.2f, %.2f, %.2f  ", fwd.x, fwd.y, fwd.z );

			ImGui::End();
		}
		//*/







		ed::End();
		ed::SetCurrentEditor( nullptr );
		ImGui::End();

	}

}

// Render a frame
void PhamApp::Render()
{
	++s_frameNum;


	if( PhamApp::Info().Barriers.size() == 0 )
	{
		auto *pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
		auto *pDSV = m_pSwapChain->GetDepthBufferDSV();
		// Clear the back buffer
		const float ClearColor[] = { 0.350f, 0.150f, 0.350f, 1.0f };
		m_pImmediateContext->ClearRenderTarget( pRTV, ClearColor, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION );
		m_pImmediateContext->ClearDepthStencil( pDSV, dg::CLEAR_DEPTH_FLAG, 1.f, 0, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION );

		const auto viewProj = /*m_Camera.GetWorldMatrix() * */ m_Camera.GetViewMatrix() * m_Camera.GetProjMatrix();

		RCDiligent context( viewProj, DGViewPtr( pRTV ), m_pImmediateContext );

		DGRenderer::Inst().render( &context );
	}



}


void PhamApp::Update( double CurrTime, double ElapsedTime )
{

	SampleBase::Update( CurrTime, ElapsedTime );

	//*
	cb::Vec3 pos( 0.0f, 0.0f, 0.0f );

	m_cubit->genWorld( pos );

	m_cubit->genGeo( pos );
	//*/



	m_Camera.Update( m_InputController, cast<float>( ElapsedTime ) );

	UpdateUI();

	const auto timeBrace = Timer<>::execution( [&]() {
		m_freefall->updateBlocks( ElapsedTime );
		});

	if( (s_frameNum & 0x3f) == 0 )
	{
		const auto timeBraceF = (f32)timeBrace;

		lprintf( "Block update in %.3f ms\n", timeBraceF / 1000.0f );
	}


	if( PhamApp::Info().Barriers.size() > 0 )
	{
		//*
		m_pImmediateContext->TransitionResourceStates( static_cast<u32>( PhamApp::Info().Barriers.size() ), PhamApp::Info().Barriers.data() );
		//for( auto context : PhamApp::Info().DeferredContexts() )
		{
			//context->TransitionResourceStates( static_cast<u32>( PhamApp::Info().Barriers.size() ), PhamApp::Info().Barriers.data() );
		}
		//m_pImmediateContext->Flush();
		//m_pImmediateContext->WaitForIdle();
		//*/
		PhamApp::Info().Barriers.clear();
	}

	//m_pImmediateContext->WaitForIdle();


}

void PhamApp::WindowResize( dg::Uint32 Width, dg::Uint32 Height )
{
	float NearPlane = 0.1f;
	float FarPlane = 10000.f;
	float AspectRatio = static_cast<float>( Width ) / static_cast<float>( Height );
	m_Camera.SetProjAttribs( NearPlane, FarPlane, AspectRatio, dg::PI_F / 4.f,
		m_pSwapChain->GetDesc().PreTransform, m_pDevice->GetDeviceInfo().IsGLDevice() );
}





