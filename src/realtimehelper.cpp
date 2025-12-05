#include "realtimehelper.h"
#include "realtime.h"
#include "utils/sceneparser.h"
#include "dof.h"
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>

realtimehelper::realtimehelper() {}

void realtimehelper::pass_global_data(GLuint &light_shader, RenderData &realtime_renderdata, glm::mat4 &m_view){
    GLint global_ka_location = glGetUniformLocation(light_shader, "shader_global_ka");
    glErrorCheck(__FILE__, __LINE__);
    glUniform1f(global_ka_location, realtime_renderdata.globalData.ka);
    glErrorCheck(__FILE__, __LINE__);

    GLint global_kd_location = glGetUniformLocation(light_shader, "shader_global_kd");
    glErrorCheck(__FILE__, __LINE__);
    glUniform1f(global_kd_location, realtime_renderdata.globalData.kd);
    glErrorCheck(__FILE__, __LINE__);

    GLint global_ks_location = glGetUniformLocation(light_shader, "shader_global_ks");
    glErrorCheck(__FILE__, __LINE__);
    glUniform1f(global_ks_location, realtime_renderdata.globalData.ks);
    glErrorCheck(__FILE__, __LINE__);

}

void realtimehelper::pass_texture_light_uniforms(GLuint &light_shader, GLuint &fbo_thing, const GLenum &texture_slot, std::string shader_name,const GLint &id) {
    glActiveTexture(texture_slot);
    glErrorCheck(__FILE__, __LINE__);
    glBindTexture(GL_TEXTURE_2D, fbo_thing);
    glErrorCheck(__FILE__, __LINE__);
    GLint location_name = glGetUniformLocation(light_shader, shader_name.c_str());
    glErrorCheck(__FILE__, __LINE__);
    glUniform1i(location_name, id);
    glErrorCheck(__FILE__, __LINE__);
}

int realtimehelper::find_shape_vao_index(PrimitiveType &primitive_type) {
    int index = 0;
    if (primitive_type == PrimitiveType::PRIMITIVE_SPHERE) {
        index = 0;
    } else if (primitive_type == PrimitiveType::PRIMITIVE_CUBE) {
        index = 1;
    } else if (primitive_type == PrimitiveType::PRIMITIVE_CYLINDER) {
        index = 2;
    } else if (primitive_type == PrimitiveType::PRIMITIVE_CONE) {
        index = 3;
    }
    return index;
}

void realtimehelper::pass_pre_lighting_uniforms(GLuint &m_shader, RenderData &realtime_renderdata, std::vector<GLuint> &shape_vaos,
                                                std::vector<std::vector<float>> &shape_datas, glm::mat4 &m_view_proj, glm::mat4 &m_view, GLuint &first_depth_fbo,
float scene_width, float scene_height) {
    glUseProgram(m_shader);
    glErrorCheck(__FILE__,__LINE__);
    pass_texture_light_uniforms(m_shader, first_depth_fbo, GL_TEXTURE6, "depth", 6);
    for (int i = 0; i < realtime_renderdata.shapes.size(); ++ i) {

        int index = find_shape_vao_index(realtime_renderdata.shapes[i].primitive.type);

        GLint sreen_space_resolution_loc = glGetUniformLocation(m_shader, "screen_space_resolution");
        glErrorCheck(__FILE__, __LINE__);
        glUniform2fv(sreen_space_resolution_loc, 1, &glm::vec2(scene_width, scene_height)[0]);
        glErrorCheck(__FILE__, __LINE__);


        glBindVertexArray(shape_vaos[index]);
        glErrorCheck(__FILE__, __LINE__);

        glm::mat4 m_model = realtime_renderdata.shapes[i].ctm;

        glm::mat4 mvp_mat = m_view_proj * m_model;
        GLint mvp_location = glGetUniformLocation(m_shader, "mvp_matrix");
        glErrorCheck(__FILE__, __LINE__);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp_mat[0][0]);
        glErrorCheck(__FILE__, __LINE__);

        glm::vec4 ambient = glm::vec4(realtime_renderdata.shapes[i].primitive.material.cAmbient.x,
                                      realtime_renderdata.shapes[i].primitive.material.cAmbient.y,
                                      realtime_renderdata.shapes[i].primitive.material.cAmbient.z,
                                      realtime_renderdata.shapes[i].primitive.material.cAmbient.w);

        GLint cAmbient_location = glGetUniformLocation(m_shader, "cAmbient");
        glErrorCheck(__FILE__, __LINE__);
        glUniform4fv(cAmbient_location, 1, &ambient[0]);
        glErrorCheck(__FILE__, __LINE__);

        glm::vec4 diffuse = glm::vec4(realtime_renderdata.shapes[i].primitive.material.cDiffuse.x,
                                      realtime_renderdata.shapes[i].primitive.material.cDiffuse.y,
                                      realtime_renderdata.shapes[i].primitive.material.cDiffuse.z,
                                      realtime_renderdata.shapes[i].primitive.material.cDiffuse.w);
        GLint cDiffuse_location = glGetUniformLocation(m_shader, "cDiffuse");
        glErrorCheck(__FILE__, __LINE__);
        glUniform4fv(cDiffuse_location, 1, &diffuse[0]);
        glErrorCheck(__FILE__, __LINE__);

        // Task 14: pass shininess, m_ks, and world-space camera position
        glm::vec4 specular = glm::vec4(realtime_renderdata.shapes[i].primitive.material.cSpecular.x,
                                       realtime_renderdata.shapes[i].primitive.material.cSpecular.y,
                                       realtime_renderdata.shapes[i].primitive.material.cSpecular.z,
                                       realtime_renderdata.shapes[i].primitive.material.cSpecular.w);
        GLint cSpecular_location = glGetUniformLocation(m_shader, "cSpecular");
        glErrorCheck(__FILE__, __LINE__);
        glUniform4fv(cSpecular_location, 1, &specular[0]);
        glErrorCheck(__FILE__, __LINE__);

        glm::mat3 inv_transpose_model_matrix = glm::mat3(inverse(transpose(m_model)));
        GLint inv_transpose_model_loc = glGetUniformLocation(m_shader, "inv_transpose_model_matrix");
        glErrorCheck(__FILE__, __LINE__);
        glUniformMatrix3fv(inv_transpose_model_loc, 1, GL_FALSE, &inv_transpose_model_matrix[0][0]);
        glErrorCheck(__FILE__, __LINE__);

        float shininess = (realtime_renderdata.shapes[i].primitive.material.shininess);
        GLint shininess_location = glGetUniformLocation(m_shader, "shader_shininess");
        glErrorCheck(__FILE__, __LINE__);
        glUniform1f(shininess_location, shininess);
        glErrorCheck(__FILE__, __LINE__);

        glDrawArrays(GL_TRIANGLES, 0, shape_datas[index].size() / 8);
        glErrorCheck(__FILE__, __LINE__);

        // Task 18: Unbind your VAO here
        glBindVertexArray(0);
        glErrorCheck(__FILE__, __LINE__);
    }

    glUseProgram(0);
}


void realtimehelper::pass_uniform_lights(RenderData &realtime_renderdata, dof &dof, GLuint &light_shader, GLuint &position_fbo, GLuint &normal_fbo, GLuint &ambinet_shininess_texture,
                                         GLuint &albedo_sepc_texture, GLuint &depth_fbo, glm::mat4 &m_view, float scene_width, float scene_height, GLuint &fullscreen_vao, GLuint &light_col_fbo,
                                        glm::mat4 &m_model, glm::mat4 &m_proj) {
    glUseProgram(light_shader);
    glErrorCheck(__FILE__, __LINE__);
    glBindVertexArray(fullscreen_vao);
    glErrorCheck(__FILE__, __LINE__);

    pass_texture_light_uniforms(light_shader, position_fbo, GL_TEXTURE0, "color_position", 0);
    pass_texture_light_uniforms(light_shader, normal_fbo, GL_TEXTURE1, "color_normal", 1);
    pass_texture_light_uniforms(light_shader, ambinet_shininess_texture, GL_TEXTURE2, "ambient_shininess", 2);
    pass_texture_light_uniforms(light_shader, albedo_sepc_texture, GL_TEXTURE3, "albedo_spec", 3);
    pass_texture_light_uniforms(light_shader, depth_fbo, GL_TEXTURE5, "depth", 5);

    pass_global_data(light_shader, realtime_renderdata, m_view);

    int lights_num = std::min(static_cast<int>(realtime_renderdata.lights.size()), 8);
    GLint light_num_loc = glGetUniformLocation(light_shader, "shader_light_num");
    glUniform1i(light_num_loc, lights_num);
    glErrorCheck(__FILE__, __LINE__);

    glm::mat3 inv_transpose_model_matrix = glm::mat3(inverse(transpose(m_model)));
    GLint inv_transpose_model_loc = glGetUniformLocation(light_shader, "inv_transpose_model_matrix");
    glErrorCheck(__FILE__, __LINE__);
    glUniformMatrix3fv(inv_transpose_model_loc, 1, GL_FALSE, &inv_transpose_model_matrix[0][0]);
    glErrorCheck(__FILE__, __LINE__);

    GLint view_loc = glGetUniformLocation(light_shader, "m_view");
    glErrorCheck(__FILE__, __LINE__);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &m_view[0][0]);
    glErrorCheck(__FILE__, __LINE__);

    GLint proj_loc = glGetUniformLocation(light_shader, "m_proj");
    glErrorCheck(__FILE__, __LINE__);
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &m_proj[0][0]);
    glErrorCheck(__FILE__, __LINE__);

    GLint camera_pos = glGetUniformLocation(light_shader, "shader_camera_pos");
    glErrorCheck(__FILE__, __LINE__);
    glm::vec4 world_camera_pos = inverse(m_view) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glUniform4fv(camera_pos, 1, &world_camera_pos[0]);
    glErrorCheck(__FILE__, __LINE__);

    GLint screenSize_loc = glGetUniformLocation(light_shader, "screenSize");
    glErrorCheck(__FILE__, __LINE__);
    glUniform1f(screenSize_loc, scene_width * scene_height);
    glErrorCheck(__FILE__, __LINE__);

    for (int i = 0; i < lights_num; ++i) {
        const SceneLightData& light = realtime_renderdata.lights[i];
        std::string shader_light = "shader_light_info[" + std::to_string(i) + "]";

        GLint light_type_loc = glGetUniformLocation(light_shader, (shader_light + ".type").c_str());   // pass light_typ
        glErrorCheck(__FILE__, __LINE__);
        glUniform1i(light_type_loc, static_cast<int>(light.type));      // converts unm to corresponding int
        glErrorCheck(__FILE__, __LINE__);
        // std::cout << "light num: " <<  static_cast<int>(light.type) << std::endl;

        glm::vec4 color = glm::vec4(light.color.x, light.color.y, light.color.z, light.color.w);
        GLint color_loc = glGetUniformLocation(light_shader, (shader_light + ".color").c_str());
        glErrorCheck(__FILE__, __LINE__);
        glUniform4fv(color_loc, 1, &color[0]);
        glErrorCheck(__FILE__, __LINE__);

        // std::cout << "color: " << light.color.x << " " << light.color.y << " "<< light.color.z << " " << std::endl;

        GLint function_loc = glGetUniformLocation(light_shader, (shader_light + ".function").c_str());
        glErrorCheck(__FILE__, __LINE__);

        // std::cout << "function: " << func.x << " " << func.y << " "<< func.z << " " << std::endl;
        glUniform3fv(function_loc, 1, &light.function[0]);
        glErrorCheck(__FILE__, __LINE__);

        glm::vec4 pos = light.pos;
        GLint pos_loc = glGetUniformLocation(light_shader, (shader_light + ".pos").c_str());
        glErrorCheck(__FILE__, __LINE__);
        glUniform4fv(pos_loc, 1, &pos[0]);
        glErrorCheck(__FILE__, __LINE__);
        // std::cout << "pos: " << light.pos.x << " " << light.pos.y << " "<< light.pos.z << " " << std::endl;

        glm::vec4 dir = normalize(light.dir);
        GLint dir_loc = glGetUniformLocation(light_shader, (shader_light + ".dir").c_str());
        glErrorCheck(__FILE__, __LINE__);
        glUniform4fv(dir_loc, 1, &dir[0]);
        glErrorCheck(__FILE__, __LINE__);
        // std::cout << "dir: " << light.dir.x << " " << light.dir.y << " "<< light.dir.z << " " << std::endl;

        float penumbra = light.penumbra;
        GLint penumbra_loc = glGetUniformLocation(light_shader, (shader_light + ".penumbra").c_str());
        glErrorCheck(__FILE__, __LINE__);
        glUniform1f(penumbra_loc, penumbra);
        glErrorCheck(__FILE__, __LINE__);

        float angle = light.angle;
        GLint angle_loc = glGetUniformLocation(light_shader, (shader_light + ".angle").c_str());
        glErrorCheck(__FILE__, __LINE__);
        glUniform1f(angle_loc, angle);
        glErrorCheck(__FILE__, __LINE__);
    };

    glDrawArrays(GL_TRIANGLES, 0, 6); // do not need to draw for each light already drawn to in geometry pass
    glErrorCheck(__FILE__, __LINE__);

    glBindVertexArray(0);
    glErrorCheck(__FILE__, __LINE__);

    glUseProgram(0);
    glErrorCheck(__FILE__, __LINE__);
}

void realtimehelper::pass_final_uniforms(GLuint &final_shader, GLuint &fullscreen_vao, dof &depth_of_field_class,
                                         glm::mat4 &m_view, glm::mat4 &m_proj,float scene_width, float scene_height, GLuint &light_col_texture, GLuint &depth_fbo,
                                         GLuint &normal_fbo) {
    glUseProgram(final_shader);
    glErrorCheck(__FILE__, __LINE__);
    glBindVertexArray(fullscreen_vao);
    glErrorCheck(__FILE__, __LINE__);

    pass_texture_light_uniforms(final_shader, normal_fbo, GL_TEXTURE1, "normal", 1);
    pass_texture_light_uniforms(final_shader, light_col_texture, GL_TEXTURE6, "light_final_col", 6);
    pass_texture_light_uniforms(final_shader, depth_fbo, GL_TEXTURE5, "depth", 5);

    depth_of_field_class.pass_depth_uniforms(final_shader);

    GLint sreen_space_resolution_loc = glGetUniformLocation(final_shader, "screen_space_resolution");
    glErrorCheck(__FILE__, __LINE__);
    glUniform2fv(sreen_space_resolution_loc, 1, &glm::vec2(scene_width, scene_height)[0]);
    glErrorCheck(__FILE__, __LINE__);

    GLint world_cam_pos_loc = glGetUniformLocation(final_shader, "world_cam_pos");
    glErrorCheck(__FILE__, __LINE__);
    glm::vec4 world_camera_pos = inverse(m_view) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glUniform4fv(world_cam_pos_loc, 1, &world_camera_pos[0]);
    glErrorCheck(__FILE__, __LINE__);

    GLint proj_matrix_loc = glGetUniformLocation(final_shader, "proj_matrix");
    glErrorCheck(__FILE__, __LINE__);
    glUniformMatrix4fv(proj_matrix_loc, 1, GL_FALSE, &m_proj[0][0]);
    glErrorCheck(__FILE__, __LINE__);

    GLint view_matrix_loc = glGetUniformLocation(final_shader, "m_view");
    glErrorCheck(__FILE__, __LINE__);
    glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, &m_view[0][0]);
    glErrorCheck(__FILE__, __LINE__);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glErrorCheck(__FILE__, __LINE__);

    glBindVertexArray(0);
    glErrorCheck(__FILE__, __LINE__);

    glUseProgram(0);
    glErrorCheck(__FILE__, __LINE__);
}

void realtimehelper::store_and_pass_texture(GLuint &thing, int m_fbo_width, int m_fbo_height, const GLuint &internalFormat, const GLenum &format, const GLenum &type,
                                            const GLenum &first_parameter_value, const GLenum &second_parameter_value, const GLenum &attachment) {
    glGenTextures(1, &thing);
    glBindTexture(GL_TEXTURE_2D, thing);
    glErrorCheck(__FILE__, __LINE__);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_fbo_width, m_fbo_height, 0, format, type, nullptr);
    glErrorCheck(__FILE__, __LINE__);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, first_parameter_value);
    glErrorCheck(__FILE__, __LINE__);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, second_parameter_value);
    glErrorCheck(__FILE__, __LINE__);
    glBindTexture(GL_TEXTURE_2D, 0);
    glErrorCheck(__FILE__, __LINE__);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, thing, 0);
    glErrorCheck(__FILE__, __LINE__);
}

void realtimehelper::bind_geometry_textures(GLuint &fbo, int fbo_width, int fbo_height, GLuint &position_fbo, GLuint &normal_fbo, GLuint &diffuse_fbo,
                                            GLuint &specular_fbo, GLuint &light_color_texture, GLuint &m_default_fbo, GLuint &first_depth_fbo, GLuint &second_depth_fbo) {
    // generate and bind teh FBO:
    glGenFramebuffers(1, &fbo);
    glErrorCheck(__FILE__, __LINE__);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glErrorCheck(__FILE__, __LINE__);

    store_and_pass_texture(position_fbo, fbo_width, fbo_height, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_COLOR_ATTACHMENT0);
    store_and_pass_texture(normal_fbo, fbo_width, fbo_height, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_COLOR_ATTACHMENT1);
    store_and_pass_texture(diffuse_fbo, fbo_width, fbo_height, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_COLOR_ATTACHMENT2);
    store_and_pass_texture(specular_fbo, fbo_width, fbo_height, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_COLOR_ATTACHMENT3);
    store_and_pass_texture(second_depth_fbo, fbo_width, fbo_height, GL_R32F, GL_RED, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_COLOR_ATTACHMENT4);
    store_and_pass_texture(light_color_texture, fbo_width, fbo_height, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST, GL_COLOR_ATTACHMENT5);
     // <---- Do this for DOF.

    store_and_pass_texture(first_depth_fbo, fbo_width, fbo_height, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_DEPTH_ATTACHMENT);

    unsigned int attachments[6] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5};
    glDrawBuffers(6, attachments);
    glErrorCheck(__FILE__, __LINE__);

}
