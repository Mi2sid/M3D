#ifndef __LAB_WORK_3_HPP__
#define __LAB_WORK_3_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include "common/camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>

#define SPEED_COEF 500

namespace M3D_ISICG
{
	struct Mesh
	{
		std::vector<Vec3f> vertexPosition;
		std::vector<Vec4f> vertexColor;
		std::vector<unsigned int> vertexIndex;
		Mat4f					  model = MAT4F_ID;
		Mat4f					  model2 = MAT4F_ID;
		GLuint					  vbo_vertex = GL_INVALID_INDEX;
		GLuint					  vbo_color	 = GL_INVALID_INDEX;
		GLuint					  vao		 = GL_INVALID_INDEX;
		GLuint					  ebo		 = GL_INVALID_INDEX;
	};

	class LabWork3 : public BaseLabWork
	{
	  public:
		LabWork3() : BaseLabWork() {}
		~LabWork3();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

	  private:
		void _createCube();
		void _initBuffers();
		void _updateMVPMatrix(int cube_index);
		void _initCamera();

		// ================ Scene data.
		Mesh   _cube;
		Camera _camera; 

		// ================

		// ================ GL data.
		GLuint _idProgram	  = GL_INVALID_INDEX;
		GLuint _loc			  = GL_INVALID_INDEX;
		GLuint _locLight	  = GL_INVALID_INDEX;
		GLuint _locMVP		  = GL_INVALID_INDEX;
		// ================

		// ================ Settings.
		Vec4f			   _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		float			   _light	= 1.f;
		float			   _fov;
		float			   _roatation_speed	  = 0.5f;
		float			   _cameraSpeed		  = 0.1f;
		float			   _cameraSensitivity = 0.1f;
		std::vector<Vec3f> _sens			  = {
			 Vec3f( 0.f, 0.f, 0.f ),
			 Vec3f( 1.f, 0.f, 0.f ), Vec3f( 0.f, 1.f, 0.f ), Vec3f( 0.f, 0.f, 1.f ),
			 Vec3f( 1.f, 1.f, 0.f ), Vec3f( 1.f, 0.f, 1.f ), Vec3f( 0.f, 1.f, 1.f ), 
			 Vec3f( 1.f, 1.f, 1.f ),
		};
		int	  _sens_number	  = 0;
		int	  _proj_number	  = 0;
		int	  _freeFly_number = 0;
		float _distance		  = 1.f;
		// ================
		
		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_3_HPP__
