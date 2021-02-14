//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
//
// copyright 2005-2008 Marc Hernandez

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

void PhamApp::Initialize(const dg::SampleInitInfo& InitInfo)
{
    dg::SampleBase::Initialize(InitInfo);
}

void PhamApp::UpdateUI()
{
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        ImGui::Begin("Pham"); // Create a window called "Hello, world!" and append into it.

        ImGui::Text("Pham");          // Display some text (you can use a format strings too)
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

void PhamApp::WindowResize( dg::Uint32 Width, dg::Uint32 Height )
{
}

