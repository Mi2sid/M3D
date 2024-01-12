#include "lab_work_4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork4::_shaderFolder = "src/lab_works/lab_work_4/shaders/";
	const std::string LabWork4::_modelFolder  = "conference.obj";
	const FilePath	  LabWork4::_modelPath	  = "data/model/conference/conference.obj";

	LabWork4::~LabWork4()
	{
		glDeleteProgram( _idProgram );
		_myModel.cleanGL();
	}

	bool LabWork4::init()
	{
		std::cout << "Initializing lab work 4..." << std::endl;

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// Z buffer
		glEnable( GL_DEPTH_TEST );

		// reading shaders files
		const std::string vertexShaderStr = readFile( _shaderFolder + "mesh.vert" );
		const std::string fragShaderStr	  = readFile( _shaderFolder + "mesh.frag" );

		// Create shaders
		GLuint vertexShader	  = glCreateShader( GL_VERTEX_SHADER );
		GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		// Conversion to GLchar**
		const GLchar * vSrc = vertexShaderStr.c_str();
		const GLchar * fSrc = fragShaderStr.c_str();

		// Associations
		glShaderSource( vertexShader, 1, &vSrc, NULL );
		glShaderSource( fragmentShader, 1, &fSrc, NULL );

		// Compiled
		glCompileShader( vertexShader );
		glCompileShader( fragmentShader );

		// Compiled ?
		// for vertexShader
		GLint compiled;
		glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertexShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}
		// for fragShader
		glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( fragmentShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std ::cerr << " Error compiling fragment shader : " << log << std ::endl;
			return false;
		}

		// Programme creation
		_idProgram = glCreateProgram();

		// Attach Shaders
		glAttachShader( _idProgram, vertexShader );
		glAttachShader( _idProgram, fragmentShader );

		// Linking shaders
		glLinkProgram( _idProgram );

		// Check if link is ok.
		GLint linked;
		glGetProgramiv( _idProgram, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _idProgram, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}


		// Shaders destruct
		glDeleteShader( vertexShader );
		glDeleteShader( fragmentShader );


		// Model loading
		_myModel.TriangleMeshModel::load( _modelFolder, _modelPath );

		// Scaling init
		_myModel._transformation = glm::scale( _myModel._transformation, glm::vec3( 0.003f ) );


		_uLightsLoc = glGetUniformLocation( _idProgram, "uLights" );
		_MVPLoc		= glGetUniformLocation( _idProgram, "uMVPMatrix" );
		_MVLoc		= glGetUniformLocation( _idProgram, "uMVMatrix" );
		_NormalLoc	= glGetUniformLocation( _idProgram, "uNormalMatrix" );


		// Uniform ModelMatrix
		glProgramUniformMatrix4fv( _idProgram,
								   glGetUniformLocation( _idProgram, "uModelMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( _myModel._transformation ) );
	

		_initCamera();

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork4::_initCamera()
	{
		_camera.setScreenSize( 1280, 720 );
		_camera.setPosition( Vec3f(-1.8f, 1.2f, 1.f) );
		_camera.rotate( 50.f, 20.f);
	}

	void LabWork4::animate( const float p_deltaTime ) {
		// pass
	}

	void LabWork4::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// getter
		_viewMatrix		  = _camera.getViewMatrix();
		_projectionMatrix = _camera.getProjectionMatrix();

		// set the light pos
		glProgramUniform3fv( _idProgram, _uLightsLoc,
							GL_FALSE,
							glm::value_ptr( VEC3F_ZERO ) 
						   );

		// MV matrix
		glProgramUniformMatrix4fv(
			_idProgram, _MVLoc, 1, GL_FALSE, glm::value_ptr( _viewMatrix * _myModel._transformation ) );

		// MVP matrix
		_MVPMatrix = _projectionMatrix * _viewMatrix * _myModel._transformation;
		glProgramUniformMatrix4fv( _idProgram, _MVPLoc, 1, GL_FALSE, glm::value_ptr( _MVPMatrix ) );

		// Normal matrix
		_normalMatrix = glm::transpose( glm::inverse( _viewMatrix * _myModel._transformation ) );
		glProgramUniformMatrix4fv( _idProgram, _NormalLoc, 1, GL_FALSE, glm::value_ptr( _normalMatrix ) );

		// Model render
		_myModel.render( _idProgram );
	}

	void LabWork4::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera.moveFront( _cameraSpeed );
				break;
			case SDL_SCANCODE_S: // Back
				_camera.moveFront( -_cameraSpeed );
				break;
			case SDL_SCANCODE_A: // Left
				_camera.moveRight( -_cameraSpeed );
				break;
			case SDL_SCANCODE_D: // Right
				_camera.moveRight( _cameraSpeed );
				break;
			case SDL_SCANCODE_R: // Up
				_camera.moveUp( _cameraSpeed );
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera.moveUp( -_cameraSpeed );
				break;
			default: break;
			}
		}

		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
		}

		// Pad motion / Mousewheel case
		if ( p_event.type == SDL_MOUSEWHEEL )
		{
			if ( p_event.wheel.y > 0 ) // scroll up
			{
				_camera.moveFront( _cameraSpeed );
			}
			else if ( p_event.wheel.y < 0 ) // scroll down
			{
				_camera.moveFront( -_cameraSpeed );
			}

			if ( p_event.wheel.x > 0 ) // scroll right
			{
				_camera.moveRight( _cameraSpeed );
			}
			else if ( p_event.wheel.x < 0 ) // scroll left
			{
				_camera.moveRight( -_cameraSpeed );
			}
		}
	}

	void LabWork4::displayUI()
	{
		ImGui::Begin( "Settings lab work 4" );
		if ( ImGui::SliderFloat( "Fovy", &_fovy, 0.f, 180.f ) )
		{
			_camera.setFovy( _fovy );
		}
		ImGui::End();
	}

} // namespace M3D_ISICG
