#ifndef __LAB_WORK_5_HPP__
#define __LAB_WORK_5_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "common/models/base_model.hpp"
#include "common/models/triangle_mesh.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "common/models/sky_box_model.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork5 : public BaseLabWork
	{
	  public:
		LabWork5() : BaseLabWork() {}

		~LabWork5();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

	  private:
		void _initCamera();

		// ================ Scene data.
		Vec4f _lightPosition = VEC4F_ZERO;

		TriangleMeshModel _myModel;
		Camera			  _camera;

		Mat4f _viewMatrix		= MAT4F_ID;
		Mat4f _projectionMatrix	= MAT4F_ID;
		Mat4f _normalMatrix		= MAT4F_ID;
		Mat4f _MVPMatrix		= MAT4F_ID;


		SkyboxModel _sky;

		// ================ GL data.
		GLuint _idProgram = GL_INVALID_INDEX;
		GLuint _uLightsLoc = GL_INVALID_INDEX;
		GLuint _MVLoc	   = GL_INVALID_INDEX;
		GLuint _MVPLoc	   = GL_INVALID_INDEX;
		GLuint _NormalLoc  = GL_INVALID_INDEX;


		// ================ Settings.
		static const std::string _shaderFolder;
		static const FilePath	 _modelPath;
		static const std::string _modelFolder;
		
		float _cameraSpeed		 = 0.2f;
		float _cameraSensitivity = 0.2f;
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f );
		float _fovy = 60.f;
	};
} // namespace M3D_ISICG

#endif