#pragma once

#include "WindowViewDelegate.hpp"
#include "tgl.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class MyScene;

class MyView : public tyga::WindowViewDelegate
{
public:
	
    MyView();
	
    ~MyView();

    void
    setScene(std::shared_ptr<const MyScene> scene);

private:

    void
    windowViewWillStart(std::shared_ptr<tyga::Window> window);
	
    void
    windowViewDidReset(std::shared_ptr<tyga::Window> window,
                       int width,
                       int height);

    void
    windowViewDidStop(std::shared_ptr<tyga::Window> window);
    
    void
    windowViewRender(std::shared_ptr<tyga::Window> window);

    std::shared_ptr<const MyScene> scene_;

	GLuint shininess_textures_[3];

    struct ShaderProgram
    {
        GLuint vertex_shader;
        GLuint fragment_shader;
        GLuint program;

        ShaderProgram() : vertex_shader(0),
                          fragment_shader(0),
                          program(0) {}
    };
	ShaderProgram sponza_shader_program_;

    struct Vertex
    {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
    };

    struct Mesh
    {
        GLuint vertex_vbo;
        GLuint element_vbo;
        GLuint vao;
        int element_count;

        Mesh() : vertex_vbo(0),
                 element_vbo(0),
                 vao(0),
                 element_count(0) {}
    };
	std::vector<Mesh> meshes_;
	Mesh pyramid_mesh_;
	glm::mat4x4 big_model_xform, small_model_xform;
};
