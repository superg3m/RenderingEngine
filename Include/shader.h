#pragam once
// #include <
#include "./External_Libraries/glad/include/glad/glad.h"
#include "./External_Libraries/stb_image.h"

#define TEXTURE_MAX 32
typedef enum ShaderType{
    VERTEX_SHADER,
    FRAGMENT_SHADER
} ShaderType;

typedef struct ShaderDescriptor {
    ShaderType type;
    const char* path; 
} ShaderDescriptor;

typedef struct Shader {
    u32* textures;
    u32 id;
} Shader;

void shader_check_compile_erros(u32 shaderID, const char* type);
Shader shader_create(ShaderDescriptor* shader_descriptor, u32 shader_descriptor_count);
void shader_add_texture(Shader* shader, const char* texture_path);
void shader_use(Shader* shader);
void shader_bind_textures(Shader* shader);
void wgl_context_create(HDC dc_handle);