#include "arpch.h"
#include "LayerSatck.h"

namespace Aura
{
	LayerSatck::LayerSatck()
	{
		m_LayerInsert = m_Layers.begin();
	}

	LayerSatck::~LayerSatck()
	{
		for(Layer* layer : m_Layers)
			delete layer;
	}

	void LayerSatck::PushLayer(Layer* layer)
	{
		m_LayerInsert = m_Layers.emplace(m_LayerInsert, layer);
	}

	void LayerSatck::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}

	void LayerSatck::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end()) {
			m_Layers.erase(it);
			m_LayerInsert--;
		}
	}

	void LayerSatck::PopOverlay(Layer* overlay)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (it != m_Layers.end()) {
			m_Layers.erase(it);
		}
	}

}
