#include "MyView.hpp"
#include "MyScene.hpp"
#include "FileHelper.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>

MyView::
MyView()
{
}

MyView::
~MyView() {
}

void MyView::
setScene(std::shared_ptr<const MyScene> scene)
{
    scene_ = scene;
}

void MyView::
windowViewWillStart(std::shared_ptr<tyga::Window> window)
{
    assert(scene_ != nullptr);

	/*
	*	This section is where we set up the
	*	Shader Program. The vertex and fragment
	*	shaders are compiled from the GLSL files
	*	in the Shader Files folder and attached
	*	to the Shader Program.
	*/

	GLint compile_status = 0;

	// Compile the Vertex Shader

	sponza_shader_program_.vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	std::string vertex_shader_string = tyga::stringFromFile("sponza_vs.glsl");
	const char *vertex_shader_code = vertex_shader_string.c_str();
	glShaderSource(sponza_shader_program_.vertex_shader, 1,
					(const GLchar **) &vertex_shader_code, NULL);
	glCompileShader(sponza_shader_program_.vertex_shader);
	glGetShaderiv(sponza_shader_program_.vertex_shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE)
	{
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(sponza_shader_program_.vertex_shader, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	// Compile the Fragment Shader

	sponza_shader_program_.fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragment_shader_string = tyga::stringFromFile("sponza_fs.glsl");
	const char *fragment_shader_code = fragment_shader_string.c_str();
	glShaderSource(sponza_shader_program_.fragment_shader, 1,
					(const GLchar **) &fragment_shader_code, NULL);
	glGetShaderiv(sponza_shader_program_.fragment_shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE)
	{
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(sponza_shader_program_.fragment_shader, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	// Create the shader program

	sponza_shader_program_.program = glCreateProgram();
	glAttachShader(sponza_shader_program_.program, sponza_shader_program_.vertex_shader);
	glBindAttribLocation(sponza_shader_program_.program, 0, "position");
	glBindAttribLocation(sponza_shader_program_.program, 1, "normal");
	glBindAttribLocation(sponza_shader_program_.program, 2, "texture_coord");
	glAttachShader(sponza_shader_program_.program, sponza_shader_program_.fragment_shader);
	glBindAttribLocation(sponza_shader_program_.program, 0, "fragment_colour");
	glLinkProgram(sponza_shader_program_.program);

	GLint link_status = 0;
	glGetProgramiv(sponza_shader_program_.program, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE)
	{
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetProgramInfoLog(sponza_shader_program_.program, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	//																							//
	//																							//
	/********************************************************************************************/
	//																							//
	//																							//

	// Create the mesh vector list from the scene data

	meshes_.resize(scene_->meshCount()); // Extend for extra models

	for(unsigned int m = 0; m < meshes_.size(); m++)
	{
		// Take the vertices from the scene mesh
		std::vector<Vertex> vertices(scene_->mesh(m).position_array.size());
		for(unsigned int i = 0; i < vertices.size(); i++)
		{
			vertices[i].position = scene_->mesh(m).position_array[i];
			vertices[i].normal = scene_->mesh(m).normal_array[i];
			vertices[i].texCoord = scene_->mesh(m).texcoord_array[i];
		}

		// Take the element from the scene mesh
		std::vector<unsigned int> elements(scene_->mesh(m).element_array.size());
		for(unsigned int i = 0; i < elements.size(); i++)
		{
			elements[i] = scene_->mesh(m).element_array[i];
		}

		// Generate a vertex buffer object for the mesh's vertices

		glGenBuffers(1, &meshes_[m].vertex_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, meshes_[m].vertex_vbo);
		glBufferData(GL_ARRAY_BUFFER,
						vertices.size() * sizeof(Vertex),
						&vertices[0],
						GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Generate a vertex buffer object for the mesh's elements

		glGenBuffers(1, &meshes_[m].element_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes_[m].element_vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
						elements.size() * sizeof(unsigned int),
						&elements[0],
						GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		meshes_[m].element_count = elements.size();

		// Generate the Vertex Array Object for the mesh

		glGenVertexArrays(1, &meshes_[m].vao);
		glBindVertexArray(meshes_[m].vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes_[m].element_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, meshes_[m].vertex_vbo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
								sizeof(Vertex), TGL_BUFFER_OFFSET(0));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
								sizeof(Vertex), TGL_BUFFER_OFFSET(sizeof(glm::vec3)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
								sizeof(Vertex), TGL_BUFFER_OFFSET((sizeof(glm::vec3)) * 2));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	/*
	*	This section is setting up the pyramids
	*	required for this assignment. The data
	*	is set up via creating a list of vertices
	*	and elements.
	*
	*	One of the most difficult tasks is to
	*	work out the normals of the vertices.
	*	In this piece of work, I went back to a
	*	an old piece of DirectX11 work I've done
	*	in the past. The idea is to add all of the
	*	face normals together, adding one to that
	*	vertex's count each time it is used. Then
	*	the overall normals is divided by the count
	*	and normalized to give an accurate vertex
	*	normal.
	*	
	*/
	
	const unsigned int triangle_count = 6;
	const unsigned int vertex_count = 16;
	const unsigned int element_count = triangle_count * 3;

	std::vector<Vertex> pyramid_vertices(vertex_count);
	pyramid_vertices[0].position = glm::vec3(0.0f, 1.0f, 0.0f);
	pyramid_vertices[1].position = glm::vec3(-1.0f, -1.0f, -1.0f);
	pyramid_vertices[2].position = glm::vec3(1.0f, -1.0f, -1.0f);
	pyramid_vertices[3].position = glm::vec3(0.0f, 1.0f, 0.0f);
	pyramid_vertices[4].position = glm::vec3(1.0f, -1.0f, -1.0f);
	pyramid_vertices[5].position = glm::vec3(1.0f, -1.0f, 1.0f);
	pyramid_vertices[6].position = glm::vec3(0.0f, 1.0f, 0.0f);
	pyramid_vertices[7].position = glm::vec3(1.0f, -1.0f, 1.0f);
	pyramid_vertices[8].position = glm::vec3(-1.0f, -1.0f, 1.0f);
	pyramid_vertices[9].position = glm::vec3(0.0f, 1.0f, 0.0f);
	pyramid_vertices[10].position = glm::vec3(-1.0f, -1.0f, 1.0f);
	pyramid_vertices[11].position = glm::vec3(-1.0f, -1.0f, -1.0f);
	pyramid_vertices[12].position = glm::vec3(-1.0f, -1.0f, -1.0f);
	pyramid_vertices[13].position = glm::vec3(1.0f, -1.0f, -1.0f);
	pyramid_vertices[14].position = glm::vec3(-1.0f, -1.0f, 1.0f);
	pyramid_vertices[15].position = glm::vec3(1.0f, -1.0f, 1.0f);
	
	std::vector<unsigned int> elements(element_count);
	elements[0] = 2;
	elements[1] = 1;
	elements[2] = 0;
	elements[3] = 5;
	elements[4] = 4;
	elements[5] = 3;
	elements[6] = 8;
	elements[7] = 7;
	elements[8] = 6;
	elements[9] = 11;
	elements[10] = 10;
	elements[11] = 9;
	elements[12] = 13;
	elements[13] = 14;
	elements[14] = 12;
	elements[15] = 15;
	elements[16] = 14;
	elements[17] = 13;

	std::vector<int> pyramid_count(vertex_count);

	// calculate vertex normals for pyramid

	for (unsigned int i = 0; i < 16; i++)
	{
		pyramid_vertices[i].normal = glm::vec3(0.0f, 0.0f, 0.0f);
		pyramid_vertices[i].texCoord = glm::vec2(0.0f, 0.0f);
		pyramid_count[i] = 0;
	}

	for (unsigned int i = 0; i < element_count; i+=3)
	{
		unsigned int i1, i2, i3;
		Vertex v1, v2, v3;

		i1 = elements[i];
		i2 = elements[i+1];
		i3 = elements[i+2];

		v1 = pyramid_vertices[i1];
		v2 = pyramid_vertices[i2];
		v3 = pyramid_vertices[i3];

		glm::vec3 vec1 = (v2.position - v1.position);
		glm::vec3 vec2 = (v3.position - v1.position);

		glm::vec3 normal = glm::cross(vec1, vec2);

		pyramid_vertices[i1].normal += normal;
		pyramid_vertices[i2].normal += normal;
		pyramid_vertices[i3].normal += normal;

		pyramid_count[i1]++;
		pyramid_count[i2]++;
		pyramid_count[i3]++;
	}

	for (unsigned int i = 0; i < vertex_count; i++)
	{
		pyramid_vertices[i].normal / (float)pyramid_count[i];

		pyramid_vertices[i].normal = glm::normalize(pyramid_vertices[i].normal);
	}
	
	// Set up the texture coordinates

	pyramid_vertices[0].texCoord = glm::vec2(0.5f, 0.0f);
	pyramid_vertices[1].texCoord = glm::vec2(0.0f, 1.0f);
	pyramid_vertices[2].texCoord = glm::vec2(1.0f, 1.0f);
	pyramid_vertices[3].texCoord = glm::vec2(0.5f, 0.0f);
	pyramid_vertices[4].texCoord = glm::vec2(0.0f, 1.0f);
	pyramid_vertices[5].texCoord = glm::vec2(1.0f, 1.0f);
	pyramid_vertices[6].texCoord = glm::vec2(0.5f, 0.0f);
	pyramid_vertices[7].texCoord = glm::vec2(0.0f, 1.0f);
	pyramid_vertices[8].texCoord = glm::vec2(1.0f, 1.0f);
	pyramid_vertices[9].texCoord = glm::vec2(0.5f, 0.0f);
	pyramid_vertices[10].texCoord = glm::vec2(0.0f, 1.0f);
	pyramid_vertices[11].texCoord = glm::vec2(1.0f, 1.0f);
	pyramid_vertices[12].texCoord = glm::vec2(0.0f, 0.0f);
	pyramid_vertices[13].texCoord = glm::vec2(1.0f, 0.0f);
	pyramid_vertices[14].texCoord = glm::vec2(0.0f, 1.0f);
	pyramid_vertices[15].texCoord = glm::vec2(1.0f, 1.0f);

	// Creating the model transforms for the pyramids
	// Instead of creating 2 instances of a pyramid,
	// I created 1 instance and gave them different
	// model transforms to put them in the appropriate
	// place and make them the right size in Sponza.

	glm::mat4x4 scale = glm::mat4x4(10.0f, 0.0f, 0.0f, 0.0f,
								    0.0f, 5.0f, 0.0f, 0.0f,
								    0.0f, 0.0f, 10.0f, 0.0f,
								    0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4x4 translate = glm::mat4x4(1.0f, 0.0f, 0.0f, 0.0f,
										0.0f, 1.0f, 0.0f, 0.0f,
										0.0f, 0.0f, 1.0f, 0.0f,
										-40.0f, 5.0f, 5.0f, 1.0f);

	big_model_xform = translate * scale;

	scale = glm::mat4x4(5.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 2.5f, 0.0f, 0.0f,
						0.0f, 0.0f, 5.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f);

	translate = glm::mat4x4(1.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 1.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 0.0f,
							80.0f, 2.5f, -15.0f, 1.0f);

	small_model_xform = translate * scale;

	// Generate a vertex buffer object for the pyramid's vertices

	glGenBuffers(1, &pyramid_mesh_.vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, pyramid_mesh_.vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER,
					pyramid_vertices.size() * sizeof(Vertex),
					&pyramid_vertices[0],
					GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// Generate a vertex buffer object for the pyramid's elements
	glGenBuffers(1, &pyramid_mesh_.element_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramid_mesh_.element_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					elements.size() * sizeof(unsigned int),
					&elements[0],
					GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	pyramid_mesh_.element_count = elements.size();

	// Generate the Vertex Array Object for the pyramid

	glGenVertexArrays(1, &pyramid_mesh_.vao);
	glBindVertexArray(pyramid_mesh_.vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramid_mesh_.element_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, pyramid_mesh_.vertex_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
							sizeof(Vertex), TGL_BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
							sizeof(Vertex), TGL_BUFFER_OFFSET(sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
							sizeof(Vertex), TGL_BUFFER_OFFSET((sizeof(glm::vec3)) * 2));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Load specular maps into texture files

	for(unsigned int i = 0; i < 3; i++)
	{
		tyga::Image shininess_image = tyga::imageFromPNG("shin" + std::to_string(i+1) + ".png");
		if(shininess_image.containsData())
		{
			glGenTextures(1, &shininess_textures_[i]);
			glBindTexture(GL_TEXTURE_2D, shininess_textures_[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
											GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D,
						 0,
						 GL_RGBA,
						 shininess_image.width(),
						 shininess_image.height(),
						 0,
						 shininess_image.componentsPerPixel() == 4 ? GL_RGBA : GL_RGB,
						 shininess_image.bytesPerComponent() == 1 ? GL_UNSIGNED_BYTE
																: GL_UNSIGNED_SHORT,
						 shininess_image.pixels());
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Enable depth test and cull face test

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void MyView::
windowViewDidReset(std::shared_ptr<tyga::Window> window,
                   int width,
                   int height)
{
    glViewport(0, 0, width, height);
}

void MyView::
windowViewDidStop(std::shared_ptr<tyga::Window> window)
{
	if (sponza_shader_program_.vertex_shader != 0)
		glDeleteShader(sponza_shader_program_.vertex_shader);
	if (sponza_shader_program_.fragment_shader != 0)
		glDeleteShader(sponza_shader_program_.fragment_shader);
	if (sponza_shader_program_.program != 0)
		glDeleteProgram(sponza_shader_program_.program);

	for(unsigned int m = 0; m < meshes_.size(); m++)
	{
		glDeleteBuffers(1, &meshes_[m].vertex_vbo);
		glDeleteBuffers(1, &meshes_[m].element_vbo);
		glDeleteVertexArrays(1, &meshes_[m].vao);
	}

	glDeleteBuffers(1, &pyramid_mesh_.vertex_vbo);
	glDeleteBuffers(1, &pyramid_mesh_.element_vbo);
	glDeleteVertexArrays(1, &pyramid_mesh_.vao);

	for(unsigned int i = 0; i < 3; i++)
	{
		glDeleteTextures(1, &shininess_textures_[i]);
	}
}

void MyView::
windowViewRender(std::shared_ptr<tyga::Window> window)
{
    assert(scene_ != nullptr);

	// Clear the screen

    glClearColor(0.f, 0.f, 0.25f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Calculate Aspect Ratio

	GLint viewport_rect[4];
	glGetIntegerv(GL_VIEWPORT, viewport_rect);
	const float aspectRatio = viewport_rect[2] / (float)viewport_rect[3];

	// Calculate transformation to see Sponza

	glm::mat4 projection = glm::perspective(scene_->camera().vertical_field_of_view_degrees, aspectRatio,
								scene_->camera().near_plane_distance, scene_->camera().far_plane_distance);
	glm::mat4 view = glm::lookAt(scene_->camera().position, scene_->camera().position + scene_->camera().direction, scene_->upDirection());

	// Set up the Shader Program

	glUseProgram(sponza_shader_program_.program);
	
	// Apply uniforms for Ambient Intensity and the Camera Position
	glUniform3fv(
		glGetUniformLocation(sponza_shader_program_.program, "ambient_intensity"),
		1, glm::value_ptr(scene_->ambientLightIntensity()));

	glUniform3fv(
			glGetUniformLocation(sponza_shader_program_.program, "camera_position"),
			1, glm::value_ptr(scene_->camera().position));

	glUniform1i(
		glGetUniformLocation(sponza_shader_program_.program, "checkered"), 0);

	// String locals for iterating through light source uniform data

	std::string pos = "].position";
	std::string ran = "].range";
	std::string its = "].intensity";

	for(int j = 0; j < scene_->lightCount(); j++)
	{
		std::string str = "lights[" + std::to_string(j);
		std::string posStr = str + pos;

		// Attach light position and range to uniform variables in light structure
		glUniform3fv(
			glGetUniformLocation(sponza_shader_program_.program, posStr.c_str()),// + "].position")),
			1, glm::value_ptr(scene_->light(j).position));

		std::string ranStr = str + ran;
		GLfloat range = scene_->light(j).range;
		GLfloat* p_range = &range;
		glUniform1fv(
			glGetUniformLocation(sponza_shader_program_.program, ranStr.c_str()),
			1, p_range);

		std::string itsStr = str + its;
		glUniform3fv(
			glGetUniformLocation(sponza_shader_program_.program, itsStr.c_str()),
			1, glm::value_ptr(scene_->light(j).position));
	}

	// Apply model specific uniforms, such as the model and combined transforms,
	// select the specular texture to use and draw the sponza models
	for(unsigned int i = 0; i < scene_->modelCount(); i++)
	{
		// Get the model's transform
		glm::mat4 model_xform = glm::mat4(scene_->model(i).xform);
		
		// Make the combined pipeline transformation
		glm::mat4 combined_xform = projection * view * model_xform;

		// Attach the two above variables and
		// the model's material colour to the
		// shader program
		glUniformMatrix4fv(
			glGetUniformLocation(sponza_shader_program_.program, "model_xform"),
			1, GL_FALSE, glm::value_ptr(model_xform));

		glUniformMatrix4fv(
			glGetUniformLocation(sponza_shader_program_.program, "combined_xform"),
			1, GL_FALSE, glm::value_ptr(combined_xform));

		glUniform3fv(
			glGetUniformLocation(sponza_shader_program_.program, "material_colour"),
			1, glm::value_ptr(scene_->material(scene_->model(i).material_index).colour));

		// Choose the specific texture needed
		// for specular lighting
		if(scene_->material(scene_->model(i).material_index).shininess_map == "shin1.png")
		{
			glUniform1i(
				glGetUniformLocation(sponza_shader_program_.program, "specularOn"),
				1);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, shininess_textures_[0]);
			glUniform1i(glGetUniformLocation(sponza_shader_program_.program, "shininess_texture"), 0);
		}
		else if (scene_->material(scene_->model(i).material_index).shininess_map == "shin2.png")
		{
			glUniform1i(
				glGetUniformLocation(sponza_shader_program_.program, "specularOn"),
				1);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, shininess_textures_[1]);
			glUniform1i(glGetUniformLocation(sponza_shader_program_.program, "shininess_texture"), 0);
		}
		else if (scene_->material(scene_->model(i).material_index).shininess_map == "shin3.png")
		{
			glUniform1i(
				glGetUniformLocation(sponza_shader_program_.program, "specularOn"),
				1);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, shininess_textures_[2]);
			glUniform1i(glGetUniformLocation(sponza_shader_program_.program, "shininess_texture"), 0);
		}
		else
		{
			glUniform1i(
				glGetUniformLocation(sponza_shader_program_.program, "specularOn"),
				0);
		}

		// Bind the vertex array and draw the model
		glBindVertexArray(meshes_[scene_->model(i).mesh_index].vao);
		glDrawElements(GL_TRIANGLES, meshes_[scene_->model(i).mesh_index].element_count, GL_UNSIGNED_INT, 0);
	}

	// Drawing the Pyramids

	// Set the uniform boolean to make the pyramids checkered
	glUniform1i(
		glGetUniformLocation(sponza_shader_program_.program, "checkered"), 1);

	// Big Pyramid
	glm::mat4 model_xform = big_model_xform;
		
	glm::mat4 combined_xform = projection * view * model_xform;

	glUniformMatrix4fv(
		glGetUniformLocation(sponza_shader_program_.program, "model_xform"),
		1, GL_FALSE, glm::value_ptr(model_xform));

	glUniformMatrix4fv(
		glGetUniformLocation(sponza_shader_program_.program, "combined_xform"),
		1, GL_FALSE, glm::value_ptr(combined_xform));

	glUniform3fv(
		glGetUniformLocation(sponza_shader_program_.program, "material_colour"),
		1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));

	glUniform1i(
				glGetUniformLocation(sponza_shader_program_.program, "specularOn"),
				0);

	// Draw the big pyramid
	glBindVertexArray(pyramid_mesh_.vao);
	glDrawElements(GL_TRIANGLES, pyramid_mesh_.element_count, GL_UNSIGNED_INT, 0);

	// Small Pyramid
	model_xform = small_model_xform;
		
	combined_xform = projection * view * model_xform;

	glUniformMatrix4fv(
		glGetUniformLocation(sponza_shader_program_.program, "model_xform"),
		1, GL_FALSE, glm::value_ptr(model_xform));

	glUniformMatrix4fv(
		glGetUniformLocation(sponza_shader_program_.program, "combined_xform"),
		1, GL_FALSE, glm::value_ptr(combined_xform));

	// Draw the small pyramid
	glBindVertexArray(pyramid_mesh_.vao);
	glDrawElements(GL_TRIANGLES, pyramid_mesh_.element_count, GL_UNSIGNED_INT, 0);
}
