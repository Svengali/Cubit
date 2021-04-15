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

static ed::EditorContext* g_Context = nullptr;
static int                  g_NextLinkId = 100;     // Counter to help generate link ids. In real application this will probably based on pointer to user data structure.



namespace Diligent
{
	SampleBase* CreateSample()
	{
		return new PhamApp();
	}

}

PhamApp::~PhamApp()
{
	DGRenderer::shutdown();
}

void PhamApp::GetEngineInitializationAttribs(
	dg::RENDER_DEVICE_TYPE DeviceType,
	dg::EngineCreateInfo& EngineCI,
	dg::SwapChainDesc& SCDesc)
{
	SampleBase::GetEngineInitializationAttribs(DeviceType, EngineCI, SCDesc);

	EngineCI.Features.DepthClamp = dg::DEVICE_FEATURE_STATE_OPTIONAL;

#if D3D12_SUPPORTED
	if (DeviceType == dg::RENDER_DEVICE_TYPE_D3D12)
	{
		auto& D3D12CI = static_cast<dg::EngineD3D12CreateInfo&>(EngineCI);
		D3D12CI.GPUDescriptorHeapSize[1] = 1024; // Sampler descriptors
		D3D12CI.GPUDescriptorHeapDynamicSize[1] = 1024;
	}
#endif
}



void Style_MarcGreen()
{

	ImGuiStyle& style = ImGui::GetStyle();
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

	style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);//ImGuiCol_Text,
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);//ImGuiCol_TextDisabled,
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.15f, 0.09f, 1.00f);//ImGuiCol_WindowBg,            
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);//ImGuiCol_ChildBg,             
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.10f, 0.05f, 0.85f);//ImGuiCol_PopupBg,             
	style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);//ImGuiCol_Border,
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);//ImGuiCol_BorderShadow,
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.01f, 0.00f, 1.00f);//ImGuiCol_FrameBg,             
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);//ImGuiCol_FrameBgHovered,
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);//ImGuiCol_FrameBgActive,
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);//ImGuiCol_TitleBg,
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.80f, 0.40f, 0.20f);//ImGuiCol_TitleBgActive,
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);//ImGuiCol_TitleBgCollapsed,
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.01f, 0.02f, 0.01f, 0.80f);//ImGuiCol_MenuBarBg,
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.30f, 0.25f, 0.60f);//ImGuiCol_ScrollbarBg,
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.55f, 0.55f, 0.53f, 0.51f);//ImGuiCol_ScrollbarGrab,
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);//ImGuiCol_ScrollbarGrabHovered,
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);//ImGuiCol_ScrollbarGrabActive,
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);//ImGuiCol_CheckMark,
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);//ImGuiCol_SliderGrab,
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);//ImGuiCol_SliderGrabActive,
	style.Colors[ImGuiCol_Button] = ImVec4(0.48f, 0.89f, 0.72f, 0.49f);//ImGuiCol_Button,
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.99f, 0.69f, 0.68f);//ImGuiCol_ButtonHovered,
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);//ImGuiCol_ButtonActive,
	style.Colors[ImGuiCol_Header] = ImVec4(0.30f, 1.00f, 0.69f, 0.53f);//ImGuiCol_Header,              
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.86f, 0.61f, 1.00f);//ImGuiCol_HeaderHovered,
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.83f, 0.62f, 1.00f);//ImGuiCol_HeaderActive,

//style.Colors[ImGuiCol_Separator]             = ImVec4( 0.38f, 0.83f, 0.62f, 1.00f );//ImGuiCol_Separator,
//style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4( 0.38f, 0.83f, 0.62f, 1.00f );//ImGuiCol_SeparatorHovered,
//style.Colors[ImGuiCol_SeparatorActive]       = ImVec4( 0.38f, 0.83f, 0.62f, 1.00f );//ImGuiCol_SeparatorActive,

	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);//ImGuiCol_ResizeGrip,
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);//ImGuiCol_ResizeGripHovered,
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);//ImGuiCol_ResizeGripActive,
//ImGuiCol_Tab,
//ImGuiCol_TabHovered,
//ImGuiCol_TabActive,
//ImGuiCol_TabUnfocused,
	style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);																																											//ImGuiCol_TabUnfocusedActive,
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.00f, 0.70f, 1.00f);																																											//ImGuiCol_PlotLines,
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.00f, 0.70f, 1.00f);																																																																																						//ImGuiCol_PlotLinesHovered,
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.60f, 1.00f);																																											//ImGuiCol_PlotHistogram,
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 0.00f, 0.35f);																																											//ImGuiCol_PlotHistogramHovered,
	//style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );																																											//ImGuiCol_TextSelectedBg,
																																											//ImGuiCol_DragDropTarget,
																																											//ImGuiCol_NavHighlight,        
																																											//ImGuiCol_NavWindowingHighlight
																																											//ImGuiCol_NavWindowingDimBg,   
																																											//ImGuiCol_ModalWindowDimBg,    

}



void Style_DarkGraphite()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.12f, 0.90f);
	colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
	colors[ImGuiCol_Separator] = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

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




void PhamApp::Initialize(const dg::SampleInitInfo& InitInfo)
{
	dg::SampleBase::Initialize(InitInfo);


	ImGuiStyle& style = ImGui::GetStyle();
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



	VertPosUV verts;

	std::vector<dg::LayoutElement> layout;

	StructToLayout(verts, &layout);

	const auto vecLayout = ResDGLayout::create(layout);

	ResourceMgr::AddResource("+VertPosUV.layout", vecLayout);

	const auto vecLayoutLU = ResourceMgr::LookupResource("+VertPosUV.layout");


	const auto cubeVerts = grx::gen::createCenteredCubeVertices(0.5f);
	const auto cubeIndicies = grx::gen::createCenteredCubeIndicies();

	ResourceMgr::AddResource("+gen:0.5.verts", cubeVerts);
	ResourceMgr::AddResource("+gen:0.5.indices", cubeIndicies);


	GeoDiligentCfgPtr cfg = ResourceMgr::GetResource<GeoDiligentCfg>("config/geo/test.xml");




	//const auto pso = ResDGPipelineState::create( )

}



void node(const u32 nodeIdBase)
{

	u32 nodeId = nodeIdBase * 10;

	ed::BeginNode(nodeId++);
	ImGui::Text("Node %i", nodeIdBase);
	ed::BeginPin(nodeId++, ed::PinKind::Input);
	ImGui::Text("-> In");
	ed::EndPin();

	ImGui::SameLine();
	ed::BeginPin(nodeId++, ed::PinKind::Output);
	ImGui::Text("Out ->");
	ed::EndPin();


	ed::BeginPin(nodeId++, ed::PinKind::Input);
	ImGui::Text("-> In");
	ed::EndPin();


	ImGui::SameLine();
	ed::BeginPin(nodeId++, ed::PinKind::Output);
	ImGui::Text("Out ->");
	ed::EndPin();

	ed::BeginPin(nodeId++, ed::PinKind::Input);
	ImGui::Text("-> In");
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

void PhamApp::UpdateUI()
{ 

	if (s_showDemoWindow)
	{
		ImGui::ShowDemoWindow(&m_ShowDemoWindow);
	}

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
		ImGui::Begin("Content");
		//*/

		//Application_Frame();

		ImGui::End();

	}
#endif



	{
		ImGui::Begin("Pham");

		ImGui::Checkbox("Show Demo Window", &s_showDemoWindow); // Edit bools storing our window open/close state
		ImGui::SameLine();
		ImGui::Checkbox("Time Blocks", &s_timeBlocks); // Edit bools storing our window open/close state

		if (s_timeBlocks)
		{
			df::timeBlocks(s_numLoops, s_numEnts, &s_timeSoAIndividualuS, &s_timeAoSuS, &s_timeSoA3uS, &s_timeSoA4uS);
		}

		{
			ImGui::SliderInt("s_numLoops", &s_numLoops, 0, 10);
			ImGui::SliderInt("s_numEnts", &s_numEnts, 0, 1000000);

			ImGui::Text("Time in uS, SoAFine %i | AoS %i | SoA 3 %i | SoA 4 %i", s_timeSoAIndividualuS, s_timeAoSuS, s_timeSoA3uS, s_timeSoA4uS);
		}

		ImGui::End();
	}

	{
		ImGui::Begin("Nodes");
		ed::SetCurrentEditor(g_Context);
		ed::Begin("My Editor", ImVec2(0.0, 0.0f));

		u32 uniqueId = 1;

		node(uniqueId++);
		node(uniqueId++);
		node(uniqueId++);
		node(uniqueId++);






		// Submit Links
		for (auto& linkInfo : g_Links)
			ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);


		// Handle creation action, returns true if editor want to create new object (node or link)
		if (ed::BeginCreate())
		{
			ed::PinId inputPinId, outputPinId;
			if (ed::QueryNewLink(&inputPinId, &outputPinId))
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

				if (inputPinId && outputPinId) // both are valid, let's accept link
				{
					// ed::AcceptNewItem() return true when user release mouse button.
					if (ed::AcceptNewItem())
					{
						// Since we accepted new link, lets add one to our list of links.
						g_Links.push_back({ ed::LinkId(g_NextLinkId++), inputPinId, outputPinId });

						// Draw new link.
						ed::Link(g_Links.back().Id, g_Links.back().InputId, g_Links.back().OutputId);
					}

					// You may choose to reject connection between these nodes
					// by calling ed::RejectNewItem(). This will allow editor to give
					// visual feedback by changing link thickness and color.
				}
			}
		}
		ed::EndCreate(); // Wraps up object creation action handling.


		// Handle deletion action
		if (ed::BeginDelete())
		{
			// There may be many links marked for deletion, let's loop over them.
			ed::LinkId deletedLinkId;
			while (ed::QueryDeletedLink(&deletedLinkId))
			{
				// If you agree that link can be deleted, accept deletion.
				if (ed::AcceptDeletedItem())
				{
					// Then remove link from your data.
					for (auto& link : g_Links)
					{
						if (link.Id == deletedLinkId)
						{
							g_Links.erase(&link);
							break;
						}
					}
				}

				// You may reject link deletion by calling:
				// ed::RejectDeletedItem();
			}
		}
		ed::EndDelete(); // Wrap up deletion action










		ed::End();
		ed::SetCurrentEditor(nullptr);
		ImGui::End();

	}





}

// Render a frame
void PhamApp::Render()
{


	auto* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
	m_pImmediateContext->ClearRenderTarget(pRTV, &m_ClearColor.x, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}


void PhamApp::Update(double CurrTime, double ElapsedTime)
{
	SampleBase::Update(CurrTime, ElapsedTime);

	UpdateUI();
}

void PhamApp::WindowResize(dg::Uint32 Width, dg::Uint32 Height)
{
}





