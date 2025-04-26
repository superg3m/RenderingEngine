#include <types.h>
#include <gl/GL.h>

#define ROTATION_SPEED 180.0f

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