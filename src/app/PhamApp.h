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

#include "ent/free.h"

namespace Diligent
{

#include "../assets/shaders/BasicStructures.fxh"

}




class PhamApp final: public dg::SampleBase
{
public:


	static PhamApp &Info();


	~PhamApp();

	virtual void Initialize( const dg::SampleInitInfo &InitInfo ) override final;

	void spawnBallisticsFrame();

	/*
	virtual void GetEngineInitializationAttribs( dg::RENDER_DEVICE_TYPE DeviceType,
		dg::EngineCreateInfo &Attribs,
		dg::SwapChainDesc &SCDesc ) override final;
		*/

	virtual void ModifyEngineInitInfo( const ModifyEngineInitInfoAttribs &Attribs ) override;


	virtual void Render() override final;
	virtual void Update( double CurrTime, double ElapsedTime ) override final;

	virtual const dg::Char *GetSampleName() const override final { return "Pham"; }

	virtual void WindowResize( dg::Uint32 Width, dg::Uint32 Height ) override final;


private:
	void UpdateUI();

	bool   m_ShowDemoWindow = true;
	bool   m_ShowAnotherWindow = false;
	dg::float4 m_ClearColor = { 0.45f, 0.55f, 0.60f, 1.00f };

	double m_nextSpawnTime = 0.0;

	dg::LightAttribs      m_LightAttribs;
	dg::FirstPersonCamera m_Camera;
	dg::MouseState        m_LastMouseState;

	vox::CubitPlanePtr    m_cubit;

	FreefallPlanePtr      m_freefall;

	bool									m_findSpawnLoc = true;

	bool                  m_continueWorldgen = true;

	ent::EntityId					m_playerId;
	ent::EntityId					m_forwardId;

};

