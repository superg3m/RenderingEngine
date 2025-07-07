#include <types.h>
#include <gl/GL.h>

#define ROTATION_SPEED 180.0f

/*
__declspec(dllexport) void initalize(Engine* engine) {
    const char* rect_shader_paths[] = {
        "../shader_source/basic.frag",
        "../shader_source/basic.vert"
    };

    Shader primative_shader = shader_create(rect_shader_paths, ArrayCount(rect_shader_paths));
    ckit_vector_push(primative_shader.attributes, 3);
    ckit_vector_push(primative_shader.attributes, 3);
    ckit_vector_push(primative_shader.attributes, 2);

    Mesh rect_mesh = mesh_create(&primative_shader, vertices_vec, indices_vec, GL_STATIC_DRAW);
    shader_add_texture(&primative_shader, "../assets/container.jpg", "textures[0]", TEXTURE_PIXEL_PERFECT|TEXTURE_VERTICAL_FLIP);
    shader_add_texture(&primative_shader, "../assets/awesomeface.png", "textures[1]", TEXTURE_VERTICAL_FLIP);
    mesh_set_position(&rect_mesh, glm::vec3(0.5f, 0.0f, 0.0f));

    engine_add_mesh(engine, "mesh_key", rect_mesh);
}
*/

__declspec(dllexport) void update_and_render(float delta_time, u32 width, u32 height) {
    float delta_time_seconds = delta_time / 1000.0f;
    static float rotation_angle = 0;
    rotation_angle += ROTATION_SPEED * delta_time_seconds;

    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    glRotatef((float)rotation_angle, 0, 0, 1);
    
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(-0.6f, -0.75f);
    glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(0.6f, -0.75f);
    glColor3f(0.0f, 0.0f, 1.0f); glVertex2f(0.0f, 0.75f);
    glEnd();
}