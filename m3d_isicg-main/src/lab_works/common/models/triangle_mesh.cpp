#include "triangle_mesh.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

namespace M3D_ISICG
{
	TriangleMesh::TriangleMesh( const std::string &				  p_name,
								const std::vector<Vertex> &		  p_vertices,
								const std::vector<unsigned int> & p_indices,
								const Material &				  p_material ) :
		_name( p_name ),
		_vertices( p_vertices ), _indices( p_indices ), _material( p_material )
	{
		_vertices.shrink_to_fit();
		_indices.shrink_to_fit();
		_setupGL();
	}

	void TriangleMesh::render( const GLuint p_glProgram ) const
	{
		glUseProgram( p_glProgram );

		
		GLuint _diffuseLoc = glGetUniformLocation( p_glProgram, "uDiffuseColor" );
		GLuint _ambiantLoc = glGetUniformLocation( p_glProgram, "uAmbiantColor" );
		GLuint _specularLoc = glGetUniformLocation( p_glProgram, "uSpecularColor" );
		GLuint _shininessLoc = glGetUniformLocation( p_glProgram, "uShininess" );

		GLuint _HasDiffuseLoc = glGetUniformLocation( p_glProgram, "uHasDiffuseMap" );
		GLuint _HasAmbiantLoc = glGetUniformLocation( p_glProgram, "uHasAmbiantMap" );
		GLuint _HasSpecularLoc = glGetUniformLocation( p_glProgram, "uHasSpecularMap" );
		GLuint _HasShininessLoc = glGetUniformLocation( p_glProgram, "uHasShininessMap" );
		GLuint _HasNormalLoc	= glGetUniformLocation( p_glProgram, "uHasNormalMap" );


		// Color
		glProgramUniform3fv( p_glProgram,
							 _diffuseLoc,
							 1,
							 glm::value_ptr( _material._diffuse ) );
		glProgramUniform3fv( p_glProgram, _ambiantLoc,
							 1,
							 glm::value_ptr( _material._ambient ) );
		glProgramUniform3fv( p_glProgram, _specularLoc,
							 1,
							 glm::value_ptr( _material._specular ) );
		glProgramUniform1f( p_glProgram, _shininessLoc, _material._shininess );

		// Textures

		glProgramUniform1i( p_glProgram, _HasDiffuseLoc, _material._hasDiffuseMap );
		if ( _material._hasDiffuseMap )
		{
			glBindTextureUnit( 1, _material._diffuseMap._id );
		}

		glProgramUniform1i( p_glProgram, _HasAmbiantLoc, _material._hasAmbientMap );
		if ( _material._hasAmbientMap )
		{
			glBindTextureUnit( 0, _material._ambientMap._id );
		}


		glProgramUniform1i( p_glProgram, _HasSpecularLoc, _material._hasSpecularMap );
		if ( _material._hasSpecularMap )
		{
			glBindTextureUnit( 2, _material._specularMap._id );
		}

		glProgramUniform1i( p_glProgram, _HasShininessLoc, _material._hasShininessMap );
		if ( _material._hasShininessMap )
		{
			glBindTextureUnit( 3, _material._shininessMap._id );
		}

		glProgramUniform1i( p_glProgram, _HasNormalLoc, _material._hasNormalMap );
		if ( _material._hasNormalMap )
		{
			glBindTextureUnit( 4, _material._normalMap._id );
		}

		glBindVertexArray( _vao );
		glDrawElements( GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0 );
		glBindVertexArray( 0 );

	}

	void TriangleMesh::cleanGL()
	{
		glDisableVertexArrayAttrib( _vao, 0 );
		glDisableVertexArrayAttrib( _vao, 1 );
		glDisableVertexArrayAttrib( _vao, 2 );
		glDisableVertexArrayAttrib( _vao, 3 );
		glDisableVertexArrayAttrib( _vao, 4 );


		glDeleteVertexArrays( 1, &_vao );
		glDeleteBuffers( 1, &_ebo );
		glDeleteBuffers( 1, &_vbo );
	}

	void TriangleMesh::_setupGL()
	{
		// creation vao
		glCreateVertexArrays( 1, &_vao );

		// creation ebo
		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _ebo, _indices.size() * sizeof( int ), _indices.data(), GL_STATIC_DRAW );

		// creation vbo
		glCreateBuffers( 1, &_vbo );
		glNamedBufferData( _vbo, _vertices.size() * sizeof( Vertex ), _vertices.data(), GL_STATIC_DRAW );

		glEnableVertexArrayAttrib( _vao, 0 ); // Position
		glEnableVertexArrayAttrib( _vao, 1 ); // Normal
		glEnableVertexArrayAttrib( _vao, 2 ); // TexCoords
		glEnableVertexArrayAttrib( _vao, 3 ); // Tangent
		glEnableVertexArrayAttrib( _vao, 4 ); // Bitangent

		glVertexArrayAttribBinding( _vao, 0, 0 );
		glVertexArrayAttribBinding( _vao, 1, 0 );
		glVertexArrayAttribBinding( _vao, 2, 0 );
		glVertexArrayAttribBinding( _vao, 3, 0 );
		glVertexArrayAttribBinding( _vao, 4, 0 );


		glVertexArrayAttribFormat( _vao, 0, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayAttribFormat( _vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _normal ) );
		glVertexArrayAttribFormat( _vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof( Vertex, _texCoords ) );
		glVertexArrayAttribFormat( _vao, 3, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _tangent ) );
		glVertexArrayAttribFormat( _vao, 4, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _bitangent ) );

		// vao - ebo
		glVertexArrayElementBuffer( _vao, _ebo );

		// vao - vbo
		glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( Vertex ) );
	}
} // namespace M3D_ISICG
