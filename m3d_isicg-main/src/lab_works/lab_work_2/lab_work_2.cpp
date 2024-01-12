#include "lab_work_2.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include "utils/random.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork2::_shaderFolder = "src/lab_works/lab_work_2/shaders/";

	// Destructor
	LabWork2::~LabWork2() { 
		// Destruction of Program
		glDeleteProgram( _idProgram );
		glDeleteBuffers( 1, &_vbo );

		glDeleteBuffers( 1, &_vbo_color );

		glDisableVertexArrayAttrib( _vao, 0 );
		glDisableVertexArrayAttrib( _vao, 1 );

		glDeleteVertexArrays( 1, &_vao );

		glDeleteBuffers( 1, &_ebo );
	}

	bool LabWork2::init()
	{
		std::cout << "Initializing lab work 2..." << std::endl;

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// reading shaders files
		const std::string vertexShaderStr = readFile( _shaderFolder + "lw2.vert" );
		const std::string fragShaderStr	  = readFile( _shaderFolder + "lw2.frag" );

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


		// Shaders destruct
		glDeleteShader( vertexShader );
		glDeleteShader( fragShader );

		// init of vector
		/* _vertexPosition = { { -0.5f, 0.5f }, { 0.5f, 0.5f }, { -0.5f, -0.5f }, { 0.5f, -0.5f } };
		_vertexIndex			 = { 0, 1, 3, 0, 2, 3 };
		_vertexColor
			= { { 1.f, 0.f, 0.f, 1.f }, { 0.f, 1.f, 0.f, 1.f }, { 1.f, 0.f, 1.f, 1.f }, { 0.f, 0.f, 1.f, 1.f } };*/

		circle( Vec2f( 0.3, -0.2 ), 64, 0.5 );

		// uniform init
		_loc = glGetUniformLocation( _idProgram, "uTranslationX" );
		_locLight = glGetUniformLocation( _idProgram, "uLight" );
		
		glProgramUniform1f( _idProgram, _locLight, _light );

		// vbo
		glCreateBuffers( 1, &_vbo );
		glNamedBufferData( _vbo, _vertexPosition.size() * sizeof( Vec2f ), _vertexPosition.data(), GL_STATIC_DRAW );

		// vao
		glCreateVertexArrays( 1, &_vao);
		glEnableVertexArrayAttrib( _vao, 0 );
		glEnableVertexArrayAttrib( _vao, 1 );
		glVertexArrayAttribFormat( _vao, 0, 2, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayAttribFormat( _vao, 1, 4, GL_FLOAT, GL_FALSE, 0 );


		// vao, vbo link
		glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( Vec2f ) );
		glVertexArrayAttribBinding( _vao, 0, 0 );

		// vbo color
		glCreateBuffers( 1, &_vbo_color );
		glNamedBufferData( _vbo_color, _vertexColor.size() * sizeof( Vec4f ), _vertexColor.data(), GL_STATIC_DRAW );

		// vao, vbo color link
		glVertexArrayVertexBuffer( _vao, 1, _vbo_color, 0, sizeof( Vec4f ) );
		glVertexArrayAttribBinding( _vao, 1, 1 );

		// ebo
		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _ebo, _vertexIndex.size() * sizeof( int ), _vertexIndex.data(), GL_STATIC_DRAW );
		
		// link vao ebo
		glVertexArrayElementBuffer( _vao, _ebo );

		glUseProgram( _idProgram );

		// it's Ok
		std::cout << "Done!" << std::endl;

		
		return true;
	}

	void LabWork2::animate( const float p_deltaTime ) { 
		
		float value = glm::sin( _time ) / 2.f;

		glProgramUniform1f( _idProgram, _loc, value );

		_time += p_deltaTime;
		
	}

	void LabWork2::render() { 
		glClear( GL_COLOR_BUFFER_BIT );
		glBindVertexArray( _vao );
		// dessin
		glDrawElements( GL_TRIANGLES, _vertexIndex.size(), GL_UNSIGNED_INT, 0 );

		glBindVertexArray( 0 );
	}

	void LabWork2::handleEvents( const SDL_Event & p_event )
	{}

	void LabWork2::displayUI()
	{
		ImGui::Begin( "Settings lab work 2" );
		if ( ImGui::SliderFloat( "Light", &_light, 0, 1 ) )
		{
			glProgramUniform1f( _idProgram, _locLight, _light );
		}
		ImGui::End();
	}

	void LabWork2::circle( const Vec2f C, const int N, const float R ) {

		if ( N < 3 )
			return;
	
		// point 1
		Vec2f p = { C.x , C.y + R };

		// round
		float angle = (2 * M_PI) / (float) N;
		for ( int i = 0; i < N; i++ ) {
			_vertexColor.push_back( Vec4f( getRandomVec3f(), 1.f ) );

			float x, y;
			x = ( p.x - C.x ) * cos( angle * i ) + ( p.y - C.y ) * sin( angle * i ) + C.x;
			y = -( p.x - C.x ) * sin( angle * i ) + ( p.y - C.y ) * cos( angle * i ) + C.y;

			_vertexPosition.push_back( Vec2f( x, y ) );

			_vertexIndex.push_back( N );
			_vertexIndex.push_back( i );
			_vertexIndex.push_back( ( i + 1 ) % N );
		}

		// center
		_vertexPosition.push_back( C );
		_vertexColor.push_back( Vec4f( getRandomVec3f(), 1.f ) );

		/* Reponse à la question : 
		 * Les coordonnées de la fenêtre sont entre - 1 et 1, que ce soit verticalement ou horizontalement
		 * les coordonnées verticales et horizontales ont un ratio de 1
		 * la fenêtre n'étant pas de ratio 1 ce n'est pas un cercle mais une élipse que se forme.
		 */

	}

} // namespace M3D_ISICG
