#ifndef __SKY_BOX_MODEL_HPP__
#define __SKY_BOX_MODEL_HPP__

#include "base_model.hpp"
#include "triangle_mesh.hpp"
#include "utils/file_path.hpp"
#include "utils/read_file.hpp"
#include "utils/image.hpp"
#include "GL/gl3w.h"
#include "define.hpp"
#include <assimp/Importer.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cmath>
#include "SDL.h"
#include <vector>

namespace M3D_ISICG
{
	class SkyboxModel : public BaseModel
	{
	  public:
		SkyboxModel()		  = default;
		virtual ~SkyboxModel() = default;

		void load( const std::vector<std::string> & p_texturePaths, std::string p_PathShader );

		void render( const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix, int p_day );
		void render( const GLuint p_glProgram ) const override { return; };


		void cleanGL() override;

	  private:
		GLuint vao, vbo;
		GLuint cubemapTexture, cubemapTexture2;
		GLuint shaderProgram;

		void loadTextures( const std::vector<std::string> & texturePaths );
		void setupShader( std::string p_shaderFolder );
		void setupGeometry();
	};

} // namespace M3D_ISICG

#endif // __BASE_OBJECT_HPP__
