#ifndef REALTIMEHELPER_H
#define REALTIMEHELPER_H
#include "utils/sceneparser.h"
#include "dof.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

class realtimehelper
{
public:
    realtimehelper();
    void pass_uniform_lights(RenderData &realtime_renderdata, dof &dof, GLuint &light_shader, GLuint &position_fbo, GLuint &normal_fbo, GLuint &ambient_fbo,
                             GLuint &color_fbo, GLuint &depth_fbo, glm::mat4 &m_view, float scene_width, float scene_height, GLuint &fullscreen_vao, GLuint &light_col_fbo,
                            glm::mat4 &m_model, glm::mat4 &m_proj);
    int find_shape_vao_index(PrimitiveType &primitive_type);
    void pass_pre_lighting_uniforms(GLuint &m_shader, RenderData &realtime_renderdata, std::vector<GLuint> &shape_vaos,
                                    std::vector<std::vector<float>> &shape_datas, glm::mat4 &m_view_proj, glm::mat4 &m_view,GLuint &first_depth_fbo, float scene_width, float scene_height);
    void pass_texture_light_uniforms(GLuint &light_shader, GLuint &fbo_thing, const GLenum &texture_slot, std::string shader_name, const GLint &id);
    void pass_global_data(GLuint &m_shader, RenderData &realtime_renderdata, glm::mat4 &m_view);
    void bind_geometry_textures(GLuint &fbo, int fbo_width, int fbo_height, GLuint &position_fbo, GLuint &normal_fbo, GLuint &diffuse_fbo,
                                GLuint &specular_fbo, GLuint &light_color, GLuint &m_default_fbo, GLuint &first_depth_fbo, GLuint &second_depth_fbo);
    void bind_light_textures(GLuint &fbo, int fbo_width, int fbo_height, GLuint &diffuse_fbo, GLuint &specular_fbo, GLuint &m_default_fbo);
    void bind_final_texture(GLuint &fbo, int fbo_width, int fbo_height, GLuint &light_color, GLuint &m_defaultFBO);
    void set_geometry2_uniforms(GLuint &m_shader, RenderData &realtime_renderdata, glm::mat4 &m_view_proj,
                                glm::mat4 &m_view, std::vector<GLuint> &shape_vaos, std::vector<std::vector<float>> &shape_datas,
                                GLuint &position_fbo, GLuint &ambient_fbo, GLuint &color_fbo, GLuint &normal_fbo, float scene_width, float scene_height, GLuint &depth_fbo, dof &depth_of_field_class);
    void store_and_pass_texture(GLuint &thing, int m_fbo_width, int m_fbo_height, const GLuint &internalFormat,
                                const GLenum &format, const GLenum &type, const GLenum &first_parameter_value,
                                const GLenum &second_parameter_value, const GLenum &attachment);
    void pass_final_uniforms(GLuint &final_shader, GLuint &fullscreen_vao, dof &depth_of_field_class,
                             glm::mat4 &m_view, glm::mat4 &m_proj,float scene_width, float scene_height, GLuint &light_col_texture, GLuint &depth_fbo
                             , GLuint &normal_fbo);
    void store_and_pass_render(GLuint &fbo, GLuint &m_fbo_renderbuffer, int m_fbo_width, int m_fbo_height);
    inline void glErrorCheck(const char* file, int line_number) {
        GLenum errorNumber = glGetError();
        while (errorNumber != GL_NO_ERROR) {
            // Task 2: Edit this print statement to be more descriptive
            std::cout << errorNumber << "in file: " << file << " - at line: " << line_number << std::endl;

            errorNumber = glGetError();
        }
    }

};

#endif // REALTIMEHELPER_H
