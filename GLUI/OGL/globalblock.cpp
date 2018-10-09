#include "globalblock.h"
#include "name.h"

namespace GLUI
{
	std::shared_ptr<globalblock> globalblock::k_globalblock;

	globalblock* globalblock::Globalblock()
	{
		if (!k_globalblock.get() || !k_globalblock->is_available())
		{
			k_globalblock.reset(new globalblock());
			if (!k_globalblock->Init())
				return false;
		}
		return k_globalblock.get();
	}

	bool globalblock::Init()
	{
		std::shared_ptr<buffer> buffer(new buffer());
		if (!buffer->generate(NULL, sizeof(Globalstruct),
			                  buffer::DYNAMIC_DRAW, buffer::UNIFORM_BUFFER)
			||!buffer->BindToBinding(buffer->get_target(),GLOBALBLOCK_BINDING_ID))
			return false;

		m_UFBbuffer = buffer;
		return true;
	}

	bool globalblock::Update()
	{
		if (!is_available()) {
			BOOST_LOG_TRIVIAL(error) << " cannot update unable globalblock";
			return false;
		}
		auto fun = [&](char* data, int size)
		{
			memcpy(data, (void*)&m_block, size);
			return true;
		};
		return m_UFBbuffer->AccessData(fun);
	}



}