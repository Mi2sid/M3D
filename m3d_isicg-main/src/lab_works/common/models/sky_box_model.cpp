#include "sky_box_model.hpp"

namespace M3D_ISICG
{
	void SkyboxModel::load( const std::vector<std::string> & p_texturePaths, std::string p_PathShader )
	{
		loadTextures( p_texturePaths );
		setupShader( p_PathShader );
		setupGeometry();
	}

	void SkyboxModel::cleanGL()
	{
		glDeleteProgram( shaderProgram );
		// clean textures
		glDeleteTextures( 1, &cubemapTexture );
		glDeleteTextures( 1, &cubemapTexture2 );

		glDisableVertexArrayAttrib( vao, 0 );
		glDeleteBuffers( 1, &vbo );
		glDeleteVertexArrays( 1, &vao );
		
	}

	void SkyboxModel::loadTextures( const std::vector<std::string> & texturePaths )
	{
		// creation de la texture
		glCreateTextures( GL_TEXTURE_CUBE_MAP, 1, &cubemapTexture );

		int width, height, nrChannels;
		Image image;
		// pour chaque image
		for ( GLuint i = 0; i < texturePaths.size(); ++i )
		{
			std::string filePath = "data/skybox/day/" + texturePaths[ i ];

			image.load( filePath.c_str() );
			unsigned char * data = image._pixels;
			// definition du format
			if ( data )
			{
				GLenum format		  = GL_INVALID_ENUM;
				GLenum internalFormat = GL_INVALID_ENUM;

				std::cerr << "load CUBE: " << texturePaths[ i ] << std::endl;
				if ( image._nbChannels == 1 )
				{
					format		   = GL_RED;
					internalFormat = GL_R32F;
				}
				else if ( image._nbChannels == 2 )
				{
					format		   = GL_RG;
					internalFormat = GL_RG32F;
				}
				else if ( image._nbChannels == 3 )
				{
					format		   = GL_RGB;
					internalFormat = GL_RGB32F;
				}
				else
				{
					format		   = GL_RGBA;
					internalFormat = GL_RGBA32F;
				}
				if ( i == 0 ) // allocation
				{
					int mipmapLevels = std::floor( std::log2( std::max( image._width, image._height ) ) );
					glTextureStorage2D( cubemapTexture, mipmapLevels, internalFormat, image._width, image._height );
				}
				// ajout dans le cube de la texture
				glTextureSubImage3D(
					cubemapTexture, 0, 0, 0, i, image._width, image._height, 1, format, GL_UNSIGNED_BYTE, data );
			}
			else
			{
				std::cerr << "Failed to load texture: " << texturePaths[ i ] << std::endl;
			}
		}
		// Génération des mipmaps après avoir chargé toutes les faces de la cubemap
		glTextureParameteri( cubemapTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTextureParameteri( cubemapTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTextureParameteri( cubemapTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTextureParameteri( cubemapTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTextureParameteri( cubemapTexture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

		glGenerateTextureMipmap( cubemapTexture );

		// DE NUIT MTN pareil

		glCreateTextures( GL_TEXTURE_CUBE_MAP, 1, &cubemapTexture2 );


		for ( GLuint i = 0; i < texturePaths.size(); ++i )
		{
			std::string filePath = "data/skybox/night/" + texturePaths[ i ];

			image.load( filePath.c_str() );
			unsigned char * data = image._pixels;
			if ( data )
			{
				GLenum format		  = GL_INVALID_ENUM;
				GLenum internalFormat = GL_INVALID_ENUM;

				std::cerr << "load CUBE: " << texturePaths[ i ] << std::endl;
				if ( image._nbChannels == 1 )
				{
					format		   = GL_RED;
					internalFormat = GL_R32F;
				}
				else if ( image._nbChannels == 2 )
				{
					format		   = GL_RG;
					internalFormat = GL_RG32F;
				}
				else if ( image._nbChannels == 3 )
				{
					format		   = GL_RGB;
					internalFormat = GL_RGB32F;
				}
				else
				{
					format		   = GL_RGBA;
					internalFormat = GL_RGBA32F;
				}
				if ( i == 0 )
				{
					int mipmapLevels = std::floor( std::log2( std::max( image._width, image._height ) ) );
					glTextureStorage2D( cubemapTexture2, mipmapLevels, internalFormat, image._width, image._height );
				}
				glTextureSubImage3D(
					cubemapTexture2, 0, 0, 0, i, image._width, image._height, 1, format, GL_UNSIGNED_BYTE, data );
			}
			else
			{
				std::cerr << "Failed to load texture: " << texturePaths[ i ] << std::endl;
			}
		}
		// Génération des mipmaps après avoir chargé toutes les faces de la cubemap

		glTextureParameteri( cubemapTexture2, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTextureParameteri( cubemapTexture2, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTextureParameteri( cubemapTexture2, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTextureParameteri( cubemapTexture2, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTextureParameteri( cubemapTexture2, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

		glGenerateTextureMipmap( cubemapTexture2 );

	}

	void SkyboxModel::setupShader( std::string p_shaderFolder )
	{
		// Assume you have a shader loader class

		const std::string vertexShaderStr = readFile( p_shaderFolder + "skybox.vert" );
		const std::string fragShaderStr	  = readFile( p_shaderFolder + "skybox.frag" );

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
			return ;
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
			return ;
		}

		// Programme creation
		shaderProgram = glCreateProgram();

		// Attach Shaders
		glAttachShader( shaderProgram, vertexShader );
		glAttachShader( shaderProgram, fragmentShader );

		// Linking shaders
		glLinkProgram( shaderProgram );

		// Check if link is ok.
		GLint linked;
		glGetProgramiv( shaderProgram, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( shaderProgram, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return ;
		}

		// Shaders destruct
		glDeleteShader( vertexShader );
		glDeleteShader( fragmentShader );

		glUseProgram( shaderProgram );

		glUseProgram( 0 );
	}

	void SkyboxModel::setupGeometry()
	{
		GLfloat skyboxVertices[] = { // Face avant
									 -1.0f,
									 1.0f,
									 -1.0f,
									 -1.0f,
									 -1.0f,
									 -1.0f,
									 1.0f,
									 -1.0f,
									 -1.0f,
									 1.0f,
									 -1.0f,
									 -1.0f,
									 1.0f,
									 1.0f,
									 -1.0f,
									 -1.0f,
									 1.0f,
									 -1.0f,

									 // Face droite
									 1.0f,
									 1.0f,
									 -1.0f,
									 1.0f,
									 -1.0f,
									 -1.0f,
									 1.0f,
									 -1.0f,
									 1.0f,
									 1.0f,
									 -1.0f,
									 1.0f,
									 1.0f,
									 1.0f,
									 1.0f,
									 1.0f,
									 1.0f,
									 -1.0f,

									 // Face arrière
									 1.0f,
									 1.0f,
									 1.0f,
									 1.0f,
									 -1.0f,
									 1.0f,
									 -1.0f,
									 -1.0f,
									 1.0f,
									 -1.0f,
									 -1.0f,
									 1.0f,
									 -1.0f,
									 1.0f,
									 1.0f,
									 1.0f,
									 1.0f,
									 1.0f,

									 // Face gauche
									 -1.0f,
									 1.0f,
									 1.0f,
									 -1.0f,
									 -1.0f,
									 1.0f,
									 -1.0f,
									 -1.0f,
									 -1.0f,
									 -1.0f,
									 -1.0f,
									 -1.0f,
									 -1.0f,
									 1.0f,
									 -1.0f,
									 -1.0f,
									 1.0f,
									 1.0f,

									 // Face supérieure
									 -1.0f,
									 1.0f,
									 1.0f,
									 -1.0f,
									 1.0f,
									 -1.0f,
									 1.0f,
									 1.0f,
									 -1.0f,
									 1.0f,
									 1.0f,
									 -1.0f,
									 1.0f,
									 1.0f,
									 1.0f,
									 -1.0f,
									 1.0f,
									 1.0f,

									 // Face inférieure
									 -1.0f,
									 -1.0f,
									 -1.0f,
									 -1.0f,
									 -1.0f,
									 1.0f,
									 1.0f,
									 -1.0f,
									 1.0f,
									 1.0f,
									 -1.0f,
									 1.0f,
									 1.0f,
									 -1.0f,
									 -1.0f,
									 -1.0f,
									 -1.0f,
									 -1.0f
		};

		// vbo
		glCreateBuffers( 1, &vbo );
		glNamedBufferData( vbo, sizeof( skyboxVertices ), &skyboxVertices, GL_STATIC_DRAW );

		// vao
		glCreateVertexArrays( 1, &vao );
		glEnableVertexArrayAttrib( vao, 0 );
		glVertexArrayAttribFormat( vao, 0, 3, GL_FLOAT, GL_FALSE, 0 );

		// vao, vbo link
		glVertexArrayVertexBuffer( vao, 0, vbo, 0, sizeof( GLfloat ) * 3 );
		glVertexArrayAttribBinding( vao, 0, 0 );
	}

	void SkyboxModel::render( const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix, int _day )
	{
		// retrait du masque de profondeur
		glDepthMask( GL_FALSE );
		glUseProgram( shaderProgram );

		// on passe les matrices
		glm::mat4 view = glm::mat4( glm::mat3( viewMatrix ) );
		glProgramUniformMatrix4fv(
			shaderProgram, glGetUniformLocation( shaderProgram, "uVM" ), 1, GL_FALSE, glm::value_ptr( view ) );
		glProgramUniformMatrix4fv( shaderProgram,
		glGetUniformLocation( shaderProgram, "uPM" ), 1, GL_FALSE, glm::value_ptr( projectionMatrix ) );

		// Choix de la skybox (projet)
		if ( ! _day )
		{
			glBindTextureUnit( 0, cubemapTexture );

			glBindVertexArray( vao );
			glActiveTexture( GL_TEXTURE0 );

			glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapTexture );
		}
		 else {
			glBindTextureUnit( 0, cubemapTexture2 );

			glBindVertexArray( vao );
			glActiveTexture( GL_TEXTURE0 );

			glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapTexture2 );
		}
		// dessin
		glDrawArrays( GL_TRIANGLES, 0, 36 );
		glBindVertexArray( 0 );

		glUseProgram( 0 );

		// On reactive le masque de profondeur
		glDepthMask( GL_TRUE );
	}

} // namespace M3D_ISICG
