#include "lab_work_6.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork6::_shaderFolder = "src/lab_works/lab_work_6/shaders/";
	const std::string LabWork6::_modelFolder  = "sponza.obj";
	const FilePath	  LabWork6::_modelPath	  = "data/model/sponza/sponza.obj";

	LabWork6::~LabWork6()
	{
		glDeleteProgram( _idProgram );
		_myModel.cleanGL();
	}

	bool LabWork6::init()
	{
		std::cout << "Initializing lab work 6..." << std::endl;

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// Z buffer
		glEnable( GL_DEPTH_TEST );

		// reading shaders files
		const std::string vertexShaderStr = readFile( _shaderFolder + "geometry_pass.vert" );
		const std::string fragShaderStr	  = readFile( _shaderFolder + "geometry_pass.frag" );

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
		_myModel._transformation = glm::scale( _myModel._transformation, glm::vec3( 0.02f ) );


		// glEnable( GL_BLEND );

		// glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		_initCamera();

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork6::_initCamera()
	{
		_camera.setScreenSize( 1280, 720 );
		_camera.setPosition( {0.f, 0.f, 0.2} );
	}

	void LabWork6::animate( const float p_deltaTime ) {
		// pass
	}

	void LabWork6::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );



		// Model render
		_myModel.render( _idProgram );
	}

	void LabWork6::handleEvents( const SDL_Event & p_event )
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

	void LabWork6::displayUI()
	{
		ImGui::Begin( "Settings lab work 6" );
		if ( ImGui::SliderFloat( "Fovy", &_fovy, 0.f, 180.f ) )
		{
			_camera.setFovy( _fovy );
		}
		ImGui::End();
	}

	void prepareGBuffer( int screenWidth, int screenHeight )
	{
		// Créer le FBO
		//glGenFramebuffers( 1, &_glBufferFBO );
		//glBindFramebuffer( GL_FRAMEBUFFER, _glBufferFBO );

		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
								 GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };

		for ( int i = 0; i < 6; ++i )
		{
			GLuint textureID;
			glGenTextures( 1, &textureID );
			glBindTexture( GL_TEXTURE_2D, textureID );

			// glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureID, 0 );
			//_glBufferTextures.push_back(
		}

		if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		{
			std::cerr << "Erreur : Échec de la création du FBO" << std::endl;
		}

		glDrawBuffers( 6, drawBuffers );
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}


} // namespace M3D_ISICG
