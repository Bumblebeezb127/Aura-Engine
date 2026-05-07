#pragma once

#include "Aura/Core.h"
#include <string>

namespace Aura
{
	class Texture
	{
	public:
		//virtual ~Texture() {};
		virtual void Bind(uint32_t slot = 0) const = 0;
		//virtual void Unbind() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		//static Texture* Create(const std::string& path);
	};

	class Texture2D :public Texture
	{
	public:
		static Ref<Texture2D> Create(const std::string& path);
	};

}