//////////////////////////////////////////////////////////////////////////////
//
//   D i l i g e n t
//
// copyright 2005-2008 Marc Hernandez

#pragma once

#include "SampleBase.hpp"

#include "BasicMath.hpp"
#include "FirstPersonCamera.hpp"



namespace Diligent
{

#include "../assets/shaders/BasicStructures.fxh"

}




class PhamApp final : public dg::SampleBase
{
public:
    ~PhamApp();

    virtual void Initialize(const dg::SampleInitInfo& InitInfo) override final;

    virtual void GetEngineInitializationAttribs( dg::RENDER_DEVICE_TYPE DeviceType,
      dg::EngineCreateInfo &Attribs,
      dg::SwapChainDesc &SCDesc ) override final;


    virtual void Render() override final;
    virtual void Update(double CurrTime, double ElapsedTime) override final;

    virtual const dg::Char* GetSampleName() const override final { return "Pham"; }

    virtual void WindowResize(dg::Uint32 Width, dg::Uint32 Height) override final;

private:
    void UpdateUI();

    bool   m_ShowDemoWindow    = true;
    bool   m_ShowAnotherWindow = false;
    dg::float4 m_ClearColor        = {0.45f, 0.55f, 0.60f, 1.00f};


    dg::LightAttribs      m_LightAttribs;
    dg::FirstPersonCamera m_Camera;
    dg::MouseState        m_LastMouseState;

    dg::RefCntAutoPtr<dg::IBuffer>                             m_CameraAttribsCB;
    dg::RefCntAutoPtr<dg::IBuffer>                             m_LightAttribsCB;
    std::vector<dg::Uint32>                                    m_PSOIndex;
    std::vector<dg::RefCntAutoPtr<dg::IPipelineState>>         m_RenderMeshPSO;
    std::vector<dg::RefCntAutoPtr<dg::IPipelineState>>         m_RenderMeshShadowPSO;
    std::vector<dg::RefCntAutoPtr<dg::IShaderResourceBinding>> m_SRBs;
    std::vector<dg::RefCntAutoPtr<dg::IShaderResourceBinding>> m_ShadowSRBs;

    dg::RefCntAutoPtr<dg::ISampler> m_pComparisonSampler;
    dg::RefCntAutoPtr<dg::ISampler> m_pFilterableShadowMapSampler;


};

