#include "light.h"
#include "name.h"
#include "globalblock.h"

namespace GLUI 
{
	bool lightbuffer::init(int size)
	{
		std::shared_ptr<GLUI::buffer> buffer;
		if (!SimpleBufferArray<lightmodel>::generatebuffer(size,
			GLUI::buffer::DYNAMIC_COPY, GLUI::buffer::SHADER_STORAGE_BUFFER, buffer))
			return false;
		if (!m_lightmodelsbuffer.bindbuffer(buffer, 0, size))
			return false;
		return true;
	}

	bool lightbuffer::init(lightmodel* models, int size)
	{
		if (!init(size))
			return false;
		if (!m_lightmodelsbuffer.Update(models, size, 0, true))
			return false;
		return true;
	}

	bool lightbuffer::update(lightmodel& model, int index)
	{
		if (index >= m_lightmodelsbuffer.size()) {
			BOOST_LOG_TRIVIAL(error) << "light number is less than:" << index+1;
			return false;
		}
		return m_lightmodelsbuffer.Update(&model, 1, index, false);
	}


	bool lightbuffer::Bind(shader* shader)
	{
		if (!is_available()) {
			BOOST_LOG_TRIVIAL(error) << "lightbuffer::Bind: cannot bind unavailable";
			return false;
		}

		shader->Use();
		//m_lightmodelsbuffer.buffer()->BindToBinding(m_lightmodelsbuffer.buffer()->get_target(),1);
		m_lightmodelsbuffer.buffer()->BindBufferRange(GLUI::buffer::SHADER_STORAGE_BUFFER,1,0, m_lightmodelsbuffer.bytesize());
		return true;
	}

	bool lightbuffer::insert(lightmodel* model, unsigned int size)
	{
		std::shared_ptr<GLUI::buffer>& buffer = m_lightmodelsbuffer.buffer();
		if (!buffer.get() || !buffer->is_available())
			return false;

		if (!buffer->ensuresize(m_lightmodelsbuffer.bytesize() + size * sizeof(lightmodel)))
			return false;

		int oldsize = m_lightmodelsbuffer.size();
		int newsize = oldsize + size;
		if (!m_lightmodelsbuffer.bindbuffer(buffer, 0, newsize))
			return false;

		return m_lightmodelsbuffer.Update(model, size, oldsize, false);
		
	}





	















}