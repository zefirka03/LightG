#include "compute_shader.h"

ComputeShader::ComputeShader() {
	m_inited = false;
}

ComputeShader::ComputeShader(const char* path) {
	load_from_file(path);
}

ComputeShader::~ComputeShader() {
	_destroy();
}

void ComputeShader::use() {
    AIR_ASSERT(m_inited, "ComputeShader not inited!");
	glUseProgram(m_prog_id);
}

void ComputeShader::unuse() {
	glUseProgram(0);
}

bool ComputeShader::is_inited() {
	return m_inited;
}

GLuint ComputeShader::get_id() {
	return m_prog_id;
}

void ComputeShader::load_from_file(const char* path) {
	if(m_prog_id)
		_destroy();
	
	m_prog_id = glCreateProgram();

	int type = 0;
	std::ifstream of;
	of.open(path);
	AIR_ASSERT(of.good(), "ComputeShader file is not loaded!");
	std::string curr;

	std::string source = "";

	while (std::getline(of, curr)) 
		source += curr + "\n";

	const char* source_c = source.c_str();
    
	GLuint compute_id = glCreateShader(GL_COMPUTE_SHADER);

	glShaderSource(compute_id, 1, &source_c, 0);
	glCompileShader(compute_id);

#ifdef AIR_DEBUG
	std::cout << "ComputeShader::Debugging\n";
	GLchar info[2048];
	glGetShaderInfoLog(compute_id, 2048, 0, info);
	std::cout << info;
#endif

	glAttachShader(m_prog_id, compute_id);
	glLinkProgram(m_prog_id);

	glDeleteShader(compute_id);

	m_inited = true;
}

void ComputeShader::load_from_string(const char* string) {
	if(m_prog_id)
		_destroy();
		
	m_prog_id = glCreateProgram();

	int type = 0;
	std::string curr;
	std::string source = "";
	std::istringstream sa(string);

	while (std::getline(sa, curr)) 
		source += curr + "\n";

	const char* source_c = source.c_str();

	GLuint compute_id = glCreateShader(GL_COMPUTE_SHADER);
    
	glShaderSource(compute_id, 1, &source_c, 0);
	glCompileShader(compute_id);

#ifdef AIR_DEBUG
		GLchar info[2048];
		glGetShaderInfoLog(compute_id, 2048, 0, info);
		std::cout << info;
#endif

	glAttachShader(m_prog_id, compute_id);
	glLinkProgram(m_prog_id);

	glDeleteShader(compute_id);

	m_inited = true;
}

GLuint ComputeShader::_request_location(const char* path) {
	GLuint loc;
	if (m_locations_cache.find(path) != m_locations_cache.end())
		loc = m_locations_cache[path];
	else loc = m_locations_cache.insert({ path, glGetUniformLocation(m_prog_id, path) }).second;
	return loc;
}

void ComputeShader::_destroy(){
	if(m_prog_id)
		glDeleteProgram(m_prog_id);
	m_inited = false;
	m_locations_cache.clear();
}

void ComputeShader::set_matrix4f(glm::mat4 val, const char* path) {
	this->use();
	glUniformMatrix4fv(_request_location(path), 1, GL_FALSE, glm::value_ptr(val));
	this->unuse();
}

void ComputeShader::set_float(GLfloat val, const char* path) {
	this->use();
	glUniform1f(glGetUniformLocation(_request_location(path), path), val);
	this->unuse();
}

void ComputeShader::set_vector2f(glm::vec2 val, const char* path) {
	this->use();
	glUniform2f(glGetUniformLocation(_request_location(path), path), val.x, val.y);
	this->unuse();
}

void ComputeShader::set_vector4f(glm::vec4 val, const char* path) {
	this->use();
	glUniform4f(glGetUniformLocation(_request_location(path), path), val.r, val.g, val.b, val.a);
	this->unuse();
}