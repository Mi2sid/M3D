#include "lab_work_1.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork1::_shaderFolder = "src/lab_works/lab_work_1/shaders/";

	// Destructor
	LabWork1::~LabWork1() { 
		// Destruction of Program
		glDeleteProgram( _idProgram );
		glDeleteBuffers( 1, &_vbo );

		glDisableVertexArrayAttrib( _vao, 0 );
		glDeleteVertexArrays( 1, &_vao );
	}

	bool LabWork1::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// reading shaders files
		const std::string vertexShaderStr = readFile( _shaderFolder + "lw1.vert" );
		const std::string fragShaderStr	  = readFile( _shaderFolder + "lw1.frag" );

		// Create shaders
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragShader	= glCreateShader(GL_FRAGMENT_SHADER);

		// Conversion to GLchar**
		const GLchar *vSrc = vertexShaderStr.c_str();
		const GLchar *fSrc = fragShaderStr.c_str();

		// Associations 
		glShaderSource( vertexShader, 1, &vSrc, NULL);
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

		// Shaders destruct
		glDeleteShader( vertexShader );
		glDeleteShader( fragShader );


		// init of vector position
		_vertexPosition = { { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.5f, -0.5f } };

		// vbo
		glCreateBuffers( 1, &_vbo );
		glNamedBufferData( _vbo, _vertexPosition.size() * sizeof( Vec2f ), _vertexPosition.data(), GL_STATIC_DRAW );

		// vao
		glCreateVertexArrays( 1, &_vao);
		glEnableVertexArrayAttrib( _vao, 0 );
		glVertexArrayAttribFormat( _vao, 0, 2, GL_FLOAT, GL_FALSE, 0 );

		// vao, vbo link
		glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( Vec2f ) );
		glVertexArrayAttribBinding( _vao, 0, 0 );

		glUseProgram( _idProgram );

		// it's Ok
		std::cout << "Done!" << std::endl;

		
		return true;
	}

	void LabWork1::animate( const float p_deltaTime ) {}

	void LabWork1::render() { 
		glClear( GL_COLOR_BUFFER_BIT );
		glBindVertexArray( _vao );
		glDrawArrays( GL_TRIANGLES, 0, _vertexPosition.size() );
		glBindVertexArray( 0 );
	}

	void LabWork1::handleEvents( const SDL_Event & p_event )
	{}

	void LabWork1::displayUI()
	{
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

} // namespace M3D_ISICG
