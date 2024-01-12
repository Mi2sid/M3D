#include "lab_work_project.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWorkProject::_shaderFolder = "src/lab_works/lab_work_project/shaders/";
	const std::string LabWorkProject::_modelFolder  = "sponza.obj";
	const FilePath	  LabWorkProject::_modelPath	= "data/model/sponza/sponza.obj";
	const float		  LabWorkProject::_scaler		= 0.02f;

	LabWorkProject::~LabWorkProject()
	{
		glDeleteProgram( _idProgram );
		_sky.cleanGL();
		_myModel.cleanGL();
	}

	bool LabWorkProject::init()
	{
		std::cout << "Initializing lab work 5..." << std::endl;

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// Z buffer
		glEnable( GL_DEPTH_TEST );

		// reading shaders files
		const std::string vertexShaderStr = readFile( _shaderFolder + "mesh_texture.vert" );
		const std::string fragShaderStr	  = readFile( _shaderFolder + "mesh_texture.frag" );

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
		_myModel._transformation = glm::scale( _myModel._transformation, glm::vec3( _scaler ) );

		// Uniform ModelMatrix
		glProgramUniformMatrix4fv( _idProgram,
								   glGetUniformLocation( _idProgram, "uModelMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( _myModel._transformation ) );

		// glEnable( GL_BLEND );

		// glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		_sky.SkyboxModel::load( { "right.png", "left.png", "top.png", "bottom.png", "front.png", "back.png" },
									  _shaderFolder );

		_MVPLoc = glGetUniformLocation( _idProgram, "uMVPMatrix" );
		_MVLoc = glGetUniformLocation( _idProgram, "uMVMatrix" );
		_NormalLoc = glGetUniformLocation( _idProgram, "uNormalMatrix" );
		_dayLoc	   = glGetUniformLocation( _idProgram, "uIsDay" );
		_colorLoc  = glGetUniformLocation( _idProgram, "uColorLight" );
		glProgramUniform1i( _idProgram, _dayLoc, _day );
		glProgramUniform1i( _idProgram, _colorLoc, _colorLight );

		_initCamera();
		_initLights();
		std::cout << "Done!" << std::endl;
		return true;
	}
	void LabWorkProject::_initLights() { 

		//Setup des toutes les lumière
		// format en mat3:
		// posx posy posz
		// filtreR filtreG filtreB
		// distance distance distance
		// 
		// lumière caméra
		_sceneLights.push_back( Mat3f(
			Vec3f( _camera.getPosition() * Vec3f( 1 / _scaler ) ), Vec3f( 0.8f, 0.7f, 0.3f ), Vec3f( _torchPower ) ) ); 
		
		// colored light :
		// bleu
		_sceneLights.push_back( 
			Mat3f( Vec3f( 9.75f, 2.7f, 2.8f ) * Vec3f( 1.f / _scaler ), Vec3f( 0.f, 0.f, 1.f ), Vec3f( 10.f ) ) ); 
		// vert
		_sceneLights.push_back(
			Mat3f( Vec3f( 9.75f, 2.7f, -4.4f ) * Vec3f( 1.f / _scaler ), Vec3f( 0.f, 1.f, 0.f ), Vec3f( 10.f ) ) ); 
		// rouge
		_sceneLights.push_back(
			Mat3f( Vec3f( -12.4f, 2.7f, 2.8f ) * Vec3f( 1.f / _scaler ), Vec3f( 1.f, 0.f, 0.f ), Vec3f( 10.f ) ) ); 
		// cyan
		_sceneLights.push_back( Mat3f(
			Vec3f( Vec3f( -12.4f, 2.7f, -4.4f ) ) * Vec3f( 1.f / _scaler ), Vec3f( 0.f, 1.f, 1.f ), Vec3f( 10.f ) ) ); 
	
		// lune
		_sceneLights.push_back( Mat3f( Vec3f( 100.f, 0.f, 20.f ) * Vec3f( 1.f / _scaler ),
									   Vec3f( 0.3f, 0.3f, 0.6f ),
									   Vec3f( 1000.f ) * Vec3f( 1.f / _scaler ) ) ); 
		// soleil
		_sceneLights.push_back( Mat3f( Vec3f( 100.f, 0.f, 20.f ) * Vec3f( 1.f / _scaler ),
									   Vec3f( 0.8f, 0.8f, 0.8f ),
									   Vec3f( 1000.f ) * Vec3f( 1.f / _scaler ) ) ); 

		_uLightsLoc = glGetUniformLocation( _idProgram, "uLights" );

		Mat4f rotateMatrix	   = glm::rotate( MAT4F_ID, glm::radians( _sunPos ), Vec3f( 0.f, 0.f, 1.f ) );
		_sceneLights[ 5 ][ 0 ] = _sceneLights[ 6 ][ 0 ]
			= Vec3f( rotateMatrix * Vec4f( Vec3f( 100.f, 0.f, 20.f ) * Vec3f( 1.f / _scaler ), 1.f ) );
	}


	void LabWorkProject::_initCamera()
	{
		_camera.setScreenSize( 1280, 720 );
		_camera.setPosition( VEC3F_ZERO );
	}

	void LabWorkProject::animate( const float p_deltaTime ) {
		// pass
	}

	void LabWorkProject::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// getter
		_viewMatrix		  = _camera.getViewMatrix();
		_projectionMatrix = _camera.getProjectionMatrix();

		_sceneLights[ 0 ][ 0 ] = _camera.getPosition() * Vec3f( 1 / _scaler );
	


		// set the light pos
		glProgramUniformMatrix3fv( _idProgram,
									_uLightsLoc,
								   7,
								   GL_FALSE,
								   glm::value_ptr( _sceneLights[0] ) );



		// MV matrix
		glProgramUniformMatrix4fv( _idProgram,
								   _MVLoc,
								   1,
								   GL_FALSE,
								   glm::value_ptr( _viewMatrix * _myModel._transformation )
								 );

		// MVP matrix
		_MVPMatrix = _projectionMatrix * _viewMatrix * _myModel._transformation;
		glProgramUniformMatrix4fv( _idProgram, 
									_MVPLoc,
									1,
									GL_FALSE, 
									glm::value_ptr( _MVPMatrix )
								 );

		// Normal matrix
		_normalMatrix = glm::transpose( glm::inverse( _viewMatrix * _myModel._transformation ) );
		glProgramUniformMatrix4fv( _idProgram,
								   _NormalLoc,
								   1,
								   GL_FALSE,
								   glm::value_ptr( _normalMatrix ) );

		// Model render
		_sky.render( _camera.getViewMatrix(), _camera.getProjectionMatrix(), _day );
		_myModel.render( _idProgram );
	}

	void LabWorkProject::handleEvents( const SDL_Event & p_event )
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

	void LabWorkProject::displayUI()
	{
		ImGui::Begin( "Settings lab work projet" );
		// slider du fov
		if ( ImGui::SliderFloat( "Fovy", &_fovy, 0.f, 180.f ) )
		{
			_camera.setFovy( _fovy );
		}

		// définition du type de lumière et de skybox pour la scene
		const char * projName[] = { "Jour", "Nuit" };

		if ( ImGui::BeginTable( "day_radio_table", 2 ) )
		{
			for ( int i = 0; i < 2; i++ )
			{
				ImGui::TableNextColumn();
				ImGui::PushID( i );
				if ( ImGui::RadioButton( projName[ i ], &_day, i ) )
				{
					glProgramUniform1i( _idProgram, _dayLoc, _day );
				}
				ImGui::PopID();
			}
			ImGui::EndTable();
		}

		// Temporalité dans la demi journé, inclinaison de l'astre
		if ( ImGui::SliderFloat( "Inclinaison", &_sunPos, 0.f, 180.f ) )
		{
			Mat4f rotateMatrix = glm::rotate( MAT4F_ID ,glm::radians( _sunPos ), Vec3f(0.f, 0.f, 1.f) );
			_sceneLights[ 5 ][ 0 ] = _sceneLights[ 6 ][ 0 ]
				= Vec3f( rotateMatrix * Vec4f( Vec3f( 100.f, 0.f, 20.f ) * Vec3f( 1.f / _scaler ), 1.f ) );
			
		}

		// Puissance de la lumière caméra
		if ( ImGui::SliderFloat( "Puissance de la torche", &_torchPower, 0.f, 100.f ) )
		{
			_sceneLights[ 0 ][ 2 ] = Vec3f( _torchPower );
		}

		// Activation ou non des lumières colorées
		if (ImGui::Checkbox("Lumieres colorees", &_colorLight)) {

			glProgramUniform1i( _idProgram, _colorLoc, _colorLight );
		}
		ImGui::End();
	}

} // namespace M3D_ISICG
