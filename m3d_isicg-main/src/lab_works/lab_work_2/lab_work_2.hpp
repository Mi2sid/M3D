#ifndef __LAB_WORK_2_HPP__
#define __LAB_WORK_2_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork2 : public BaseLabWork
	{
	  public:
		LabWork2() : BaseLabWork() {}
		~LabWork2();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;
		void circle( const Vec2f C, const int N, const float R );

	  private:
		// ================ Scene data.
		std::vector<Vec2f>	_vertexPosition;
		std::vector<int>	_vertexIndex;
		std::vector<Vec4f>	_vertexColor;
		GLuint				_loc	  = GL_INVALID_INDEX;
		GLuint				_locLight = GL_INVALID_INDEX;
		// ================

		// ================ GL data.
		GLuint _ebo		  = GL_INVALID_INDEX;
		GLuint _vbo		  = GL_INVALID_INDEX;
		GLuint _vbo_color = GL_INVALID_INDEX;
		GLuint _vao		  = GL_INVALID_INDEX;
		GLuint _idProgram = GL_INVALID_INDEX;
		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		float _time	   = 0;
		float _light   = 1;
		// ================
		
		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_2_HPP__
