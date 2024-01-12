#ifndef __LAB_WORK_6_HPP__
#define __LAB_WORK_6_HPP__

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
	class LabWorkProject : public BaseLabWork
	{
	  public:
		LabWorkProject() : BaseLabWork() {}

		~LabWorkProject();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

	  private:
		void _initCamera();
		void _initLights();

		// ================ Scene data.
		TriangleMeshModel _myModel;
		Camera			  _camera;

		Mat4f _viewMatrix		= MAT4F_ID;
		Mat4f _projectionMatrix = MAT4F_ID;
		Mat4f _normalMatrix		= MAT4F_ID;
		Mat4f _MVPMatrix		= MAT4F_ID;

		SkyboxModel _sky;

		std::vector<Mat3f> _sceneLights;

		// ================ GL data.
		GLuint				 _idProgram = GL_INVALID_INDEX;
		GLuint				 _glBufferFBO = GL_INVALID_INDEX;
		GLuint				_uLightsLoc = GL_INVALID_INDEX;
		GLuint				 _MVLoc  = GL_INVALID_INDEX;
		GLuint				 _MVPLoc  = GL_INVALID_INDEX;
		GLuint				 _NormalLoc  = GL_INVALID_INDEX;
		GLuint				 _dayLoc	  = GL_INVALID_INDEX;
		GLuint				 _colorLoc	  = GL_INVALID_INDEX;

		std::vector<Texture> _glBufferTextures;

		// ================ Settings.
		static const std::string _shaderFolder;
		static const FilePath	 _modelPath;
		static const std::string _modelFolder;
		static const float		 _scaler;

		float _cameraSpeed		 = 0.2f;
		float _cameraSensitivity = 0.2f;
		Vec4f _bgColor			 = Vec4f( 0.8f, 0.8f, 0.8f, 1.f );
		float _fovy				 = 60.f;
		float _sunPos			 = 60.f;
		int	  _day				 = 0;
		float _torchPower		 = 40.f;
		bool  _colorLight	 = true;
	};
} // namespace M3D_ISICG

#endif