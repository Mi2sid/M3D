#include "lab_work_3.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include "utils/random.hpp"
#include "glm/glm.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork3::_shaderFolder = "src/lab_works/lab_work_3/shaders/";

	// Destructor
	LabWork3::~LabWork3() { 
		// Destruction of Program
		glDeleteProgram( _idProgram );
		glDeleteBuffers( 1, &_cube.vbo_vertex );

		glDeleteBuffers( 1, &_cube.vbo_color );

		glDisableVertexArrayAttrib( _cube.vao, 0 );
		glDisableVertexArrayAttrib( _cube.vao, 1 );

		glDeleteVertexArrays( 1, &_cube.vao );

		glDeleteBuffers( 1, &_cube.ebo );
		glDisable( GL_DEPTH_TEST );
	}

	bool LabWork3::init()
	{
		std::cout << "Initializing lab work 3..." << std::endl;

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// Z buffer
		glEnable( GL_DEPTH_TEST );


		// reading shaders files
		const std::string vertexShaderStr = readFile( _shaderFolder + "lw3.vert" );
		const std::string fragShaderStr	  = readFile( _shaderFolder + "lw3.frag" );

		// Create shaders
		GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
		GLuint fragShader	= glCreateShader( GL_FRAGMENT_SHADER );

		// Conversion to GLchar**
		const GLchar * vSrc = vertexShaderStr.c_str();
		const GLchar * fSrc = fragShaderStr.c_str();

		// Associations
		glShaderSource( vertexShader, 1, &vSrc, NULL );
		glShaderSource( fragShader, 1, &fSrc, NULL );

		// Compiled
		glCompileShader( vertexShader );
		glCompileShader( fragShader );

		// Compiled ?
		// for certexShader
		GLint compiled;
		glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertexShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragShader );
			std::cerr << " Error compiling vertex shader : " << log << std::endl;
			return false;
		}
		// for fragShader
		glGetShaderiv( fragShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( fragShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragShader );
			std::cerr << " Error compiling fragment shader : " << log << std::endl;
			return false;
		}

		// Program creation
		_idProgram = glCreateProgram();
		
		// Attach Shaders
		glAttachShader( _idProgram, vertexShader );
		glAttachShader( _idProgram, fragShader );

		// Linking shaders
		glLinkProgram( _idProgram );

		// Check if link is ok.
		GLint linked;
		glGetProgramiv( _idProgram, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glDeleteShader( vertexShader );
			glDeleteShader( fragShader );
			glGetProgramInfoLog( _idProgram, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		// init fov
		_fov = _camera.getFovy();

		// Shaders destruct
		glDeleteShader( vertexShader );
		glDeleteShader( fragShader );


		// uniform init
		_loc		   = glGetUniformLocation( _idProgram, "uTranslationX" );
		_locLight	   = glGetUniformLocation( _idProgram, "uLight" );
		_locMVP		   = glGetUniformLocation( _idProgram, "uMVPMatrix" );


		// init light
		glProgramUniform1f( _idProgram, _locLight, _light );

		_initCamera();
		_createCube();
		
		glUseProgram( _idProgram );
		
		_initBuffers();



		// it's Ok
		std::cout << "Done!" << std::endl;
		
		return true;
	}

	void LabWork3::animate( const float p_deltaTime ) { 
		_cube.model = glm::rotate(
			_cube.model, _roatation_speed * SPEED_COEF * glm::radians( p_deltaTime ), Vec3f( 1.f, 1.f, 1.f ) );
		
		_cube.model2 = glm::translate( _cube.model2, -Vec3f( _distance ) * ( Vec3f( 1.f ) - _sens[ _sens_number ] ) );

		_cube.model2 = glm::rotate(
			_cube.model2, _roatation_speed * SPEED_COEF * glm::radians( p_deltaTime ), _sens[ _sens_number ] );
		
		_cube.model2 = glm::translate( _cube.model2, Vec3f( _distance ) * ( Vec3f( 1.f ) - _sens[ _sens_number ] ) );


		_updateMVPMatrix(0);

	}

	void LabWork3::render() { 
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		
		glBindVertexArray( _cube.vao );
		
		glDrawElements( GL_TRIANGLES, (GLsizei) _cube.vertexIndex.size(), GL_UNSIGNED_INT, 0 );

		_updateMVPMatrix(1);

		glDrawElements( GL_TRIANGLES, (GLsizei) _cube.vertexIndex.size(), GL_UNSIGNED_INT, 0 );

		_updateMVPMatrix( 0 );

		glBindVertexArray( 0 );
	}

	void LabWork3::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera.moveFront( _cameraSpeed );
				_updateMVPMatrix(0);
				break;
			case SDL_SCANCODE_S: // Back
				_camera.moveFront( -_cameraSpeed );
				_updateMVPMatrix(0);
				break;
			case SDL_SCANCODE_A: // Left
				_camera.moveRight( -_cameraSpeed );
				_updateMVPMatrix(0);
				break;
			case SDL_SCANCODE_D: // Right
				_camera.moveRight( _cameraSpeed );
				_updateMVPMatrix(0);
				break;
			case SDL_SCANCODE_R: // Up
				_camera.moveUp( _cameraSpeed );
				_updateMVPMatrix(0);
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera.moveUp( -_cameraSpeed );
				_updateMVPMatrix(0);
				break;
			default: break;
			}
		}
		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
			_updateMVPMatrix(0);
		}
	}

	void LabWork3::displayUI()
	{
		ImGui::Begin( "Settings lab work 3" );
		// slider du fov
		if ( ImGui::SliderFloat( "FOV", &_fov, 0.f, 180.f ) )
		{
			_camera.setFovy( _fov );
			_updateMVPMatrix(0);
		}
		// slider de rotation du cube
		const char * axe[] = { "None", "X", "Y", "Z", "XY", "XZ", "YZ", "XYZ" };

		if ( ImGui::SliderInt( "AXE", &_sens_number, 0, 7, axe[_sens_number] ) )
		{
			_cube.model2
				= glm::translate( Mat4f( 1.f ), Vec3f( _distance ) * ( Vec3f( 1.f ) - _sens[ _sens_number ] ) );
		}

		// slider de distance des 2 cubes
		if ( ImGui::SliderFloat( "distance", &_distance, 0.f, 5.f) ) {
			_cube.model2
				= glm::translate( Mat4f( 1.f ), Vec3f( _distance ) * ( Vec3f( 1.f ) - _sens[ _sens_number ] ) );
		}

		// Détermination de la projection voulu
		const char * projName[] = { "Perspective", "Othographique" };

		if ( ImGui::BeginTable( "projection_radio_table", 2) )
		{
			for (int i = 0; i < 2; i++ )
			{
				ImGui::TableNextColumn();
				ImGui::PushID( i );
				if ( ImGui::RadioButton( projName[i], &_proj_number, i ) )
				{
					_camera.setProjectionP( i == 0 );
					_camera.setFovy( _fov );
				}
				ImGui::PopID();
			}
			ImGui::EndTable();
		}

		// Switch entre 2 modes de caméra
		const char * viewMode[] = { "FreeFly", "Trackball" };

		if ( ImGui::BeginTable( "freeFly_radio_table", 2 ) )
		{
			for ( int i = 0; i < 2; i++ )
			{
				ImGui::TableNextColumn();
				ImGui::PushID( i );
				if ( ImGui::RadioButton( viewMode[ i ], &_freeFly_number, i ) )
				{
					_camera.setFreeFly( i == 0 );
					_camera.moveFront( 0.f );
				}
				ImGui::PopID();
			}
			ImGui::EndTable();
		}

		ImGui::End();
	}

	void LabWork3::_createCube() {
		// setup des position
		_cube.vertexPosition
			= { { 0.5f, 0.5f, 0.5f },  { 0.5f, -0.5f, 0.5f },  { -0.5f, 0.5f, 0.5f },  { -0.5f, -0.5f, 0.5f },
				{ 0.5f, 0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f }, { -0.5f, -0.5f, -0.5f } };

		for ( int i = 0; i < _cube.vertexPosition.size() ; i++ )
			_cube.vertexColor.push_back( Vec4f( getRandomVec3f(), 1.f ) );


		_cube.vertexIndex = {
			3, 0, 1, 3, 0, 2, // z+
			7, 4, 5, 7, 4, 6, // z-
			1, 7, 3, 1, 7, 5, // y-
			0, 6, 2, 0, 6, 4, // y+
			0, 5, 1, 0, 5, 4, // x+
			2, 7, 3, 2, 7, 6, // x-
		};

		_cube.model = glm::scale( _cube.model, Vec3f( 2.f ) );
		_cube.model2 = glm::translate( _cube.model2, Vec3f( _distance ) * ( Vec3f( 1.f ) - _sens[ _sens_number ] ) );
	}

	void LabWork3::_initBuffers() { 

		// vbo
		glCreateBuffers( 1, &_cube.vbo_vertex );
		glNamedBufferData(
			_cube.vbo_vertex, _cube.vertexPosition.size() * sizeof( Vec3f ), _cube.vertexPosition.data(), GL_STATIC_DRAW );

		// vao
		glCreateVertexArrays( 1, &_cube.vao );
		glEnableVertexArrayAttrib( _cube.vao, 0 );
		glEnableVertexArrayAttrib( _cube.vao, 1 );


		// vao, vbo link
		glVertexArrayVertexBuffer( _cube.vao, 0, _cube.vbo_vertex, 0, sizeof( Vec3f ) );
		glVertexArrayAttribFormat( _cube.vao, 0, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayAttribBinding( _cube.vao, 0, 0 );

		// vbo color
		glCreateBuffers( 1, &_cube.vbo_color );
		glNamedBufferData(
			_cube.vbo_color, _cube.vertexColor.size() * sizeof( Vec4f ), _cube.vertexColor.data(), GL_STATIC_DRAW );

		// vao, vbo color link
		glVertexArrayVertexBuffer( _cube.vao, 1, _cube.vbo_color, 0, sizeof( Vec4f ) );
		glVertexArrayAttribFormat( _cube.vao, 1, 4, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayAttribBinding( _cube.vao, 1, 1 );


		// ebo
		glCreateBuffers( 1, &_cube.ebo );
		glNamedBufferData(
			_cube.ebo, _cube.vertexIndex.size() * sizeof( unsigned int ), _cube.vertexIndex.data(), GL_STATIC_DRAW );

		// link vao ebo
		glVertexArrayElementBuffer( _cube.vao, _cube.ebo );
	}

	void LabWork3::_updateMVPMatrix(int cube_index) {
		Mat4f transformation;
		if ( cube_index == 0 )
			transformation = _camera.getProjectionMatrix() * _camera.getViewMatrix() * _cube.model;
		else
			transformation = _camera.getProjectionMatrix() * _camera.getViewMatrix() * _cube.model2;

		glProgramUniformMatrix4fv( _idProgram, _locMVP, 1, GL_FALSE, glm::value_ptr( transformation ) );
	}


	void LabWork3::_initCamera() { 
		_camera.setPosition( Vec3f( 0.f, 1.f, 3.f ) );
		_camera.setScreenSize( 1280, 720 );
		_updateMVPMatrix(0);
	}

} // namespace M3D_ISICG
