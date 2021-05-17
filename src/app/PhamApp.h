//////////////////////////////////////////////////////////////////////////////
//
//   C u b i t
//
// copyright 2005-2021 Marc Hernandez

#pragma once

#include "SampleBase.hpp"

#include "BasicMath.hpp"
#include "FirstPersonCamera.hpp"

#include "vox/cubit.h"

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

    vox::CubitPlanePtr    m_cubit;


};

