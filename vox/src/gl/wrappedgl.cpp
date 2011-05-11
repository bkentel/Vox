#include "common.hpp"
#include "wrappedgl.hpp"

namespace detail	= ::vox::gl::detail;
namespace gl		= ::vox::gl;

#define THROW_GL_ERROR(API_NAME, ERROR_CODE)				\
	BOOST_THROW_EXCEPTION(									\
		::vox::gl::error::api_error()						\
			<< ::boost::errinfo_api_function(API_NAME)		\
			<< ::vox::gl::error::error_num(ERROR_CODE)		\
	)														\

#define THROW_GL_ERROR_INFO(API_NAME, ERROR_CODE, INFO)		\
	BOOST_THROW_EXCEPTION(									\
		::vox::gl::error::api_error()						\
			<< ::boost::errinfo_api_function(API_NAME)		\
			<< ::vox::gl::error::error_num(ERROR_CODE)		\
			<< INFO											\
	)														\

void
detail::onError(::std::function<void (gl::error::ErrorType glError)> errfunc)
{
	auto const glError = ::glGetError();
	if (glError != gl::error::ERROR_GL_NO_ERROR) {
		gl::error::ErrorType const e = static_cast<gl::error::ErrorType>(glError);
		errfunc(e);
	}
}

std::vector<gl::ArrayId>
detail::genVertexArrays(unsigned n)
{
	assert(n > 0);
			
	std::vector<ArrayId> result;
			
	::glGenVertexArrays(
		n,
		reinterpret_cast<GLuint*>(&result[0])
	);

	return result;
}

gl::ArrayId
detail::genVertexArray()
{		
	GLuint id = 0;
	::glGenVertexArrays(1, &id);

	return ArrayId(id);
}

void
detail::deleteVertexArrays(std::vector<gl::ArrayId>& arrays)
{
	::glDeleteVertexArrays(
		arrays.size(),
		reinterpret_cast<GLuint const*>(&arrays[0])
	);

	onError([] (error::ErrorType e) {
		THROW_GL_ERROR("glDeleteVertexArrays", e);
	});
}

void
detail::deleteVertexArray(gl::ArrayId array)
{
	::glDeleteVertexArrays(1, &array.value);

	onError([&array] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glDeleteVertexArrays", e, error::array_id(array));
	});
}

void
detail::bindVertexArray(gl::ArrayId array)
{
	::glBindVertexArray(array.value);

	onError([&array] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glBindVertexArray", e, error::array_id(array));
	});
}

void
detail::bindVertexArray()
{
	bindVertexArray(ArrayId(0));
}

void
detail::drawArrays(gl::DrawMode mode, GLint first, GLsizei count)
{
	::glDrawArrays(mode, first, count);

	onError([] (error::ErrorType e) {
		THROW_GL_ERROR("drawArrays", e);
	});
}

void
detail::vertexAttribPointer(
	gl::AttributeLocation	index,
	gl::AttributeSize		size,
	gl::DataType			type,
	GLboolean				normalized,
	GLsizei					stride,
	const GLvoid*			pointer
)
{
	::glVertexAttribPointer(index.value, size, type, normalized, stride, pointer);

	onError([&index] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glVertexAttribPointer", e, error::attr_loc(index));
	});
}

void
detail::enableVertexAttribArray(gl::AttributeLocation index)
{
	::glEnableVertexAttribArray(index.value);

	onError([&index] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glEnableVertexAttribArray", e, error::attr_loc(index));
	});
}
		
void
detail::disableVertexAttribArray(gl::AttributeLocation index)
{
	::glDisableVertexAttribArray(index.value);

	onError([&index] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glEnableVertexAttribArray", e, error::attr_loc(index));
	});
}

void
detail::deleteTexture(gl::TextureId texture)
{
    ::glDeleteTextures(1, &texture.value);
    //TODO
}

gl::TextureId
detail::genTexture()
{
    TextureId result;
    ::glGenTextures(1, &result.value); //TODO

    return result;
}

void
detail::bindTexture(gl::TextureTarget target, gl::TextureId texture)
{
    ::glBindTexture(target, texture.value); //TODO
}

std::vector<gl::BufferId>
detail::genBuffers(unsigned n)
{
	assert(n > 0);

	std::vector<gl::BufferId> result(n);
	
	::glGenBuffers(
		n,
		reinterpret_cast<GLuint*>(&result[0])
	);

	return result;
}

gl::BufferId
detail::genBuffer()
{
	GLuint id = 0;
	::glGenBuffers(1, &id);

	return gl::BufferId(id);
}

void
detail::deleteBuffers(std::vector<gl::BufferId> const& buffers)
{
	::glDeleteBuffers(
		buffers.size(),
		reinterpret_cast<GLuint const*>(&buffers[0])
	);

	onError([] (error::ErrorType e) {
		THROW_GL_ERROR("glDeleteBuffers", e);
	});
}

void
detail::deleteBuffer(gl::BufferId buffer)
{
	::glDeleteBuffers(1, &buffer.value);

	onError([&buffer] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glDeleteBuffers", e, error::buffer_id(buffer));
	});
}

void
detail::bindBuffer(gl::BufferTarget target, gl::BufferId buffer)
{
	::glBindBuffer(target, buffer.value);

	onError([&buffer, &target] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glBindBuffer", e,
			error::buffer_id(buffer) << error::buffer_target(target)
		);
	});
}

void
detail::unbindBuffer(gl::BufferTarget target)
{
	bindBuffer(target, BufferId(0));
}

void
detail::bufferData(gl::BufferTarget target, GLsizeiptr size, const GLvoid* data, gl::BufferUsage usage)
{
	::glBufferData(target, size, data, usage);

	onError([&target] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glBufferData", e, error::buffer_target(target));
	});
}

void
detail::bufferSubData(
	gl::BufferTarget	target,
	GLintptr			offset,
	GLsizeiptr			size,
	const GLvoid*		data
) {
	::glBufferSubData(target, offset, size, data);

	onError([&target] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glBufferSubData", e, error::buffer_target(target));
	});
}

void
detail::getBufferSubData(
	gl::BufferTarget	target,
	GLintptr			offset,
	GLsizeiptr			size,
	GLvoid*				data
) {
	::glGetBufferSubData(target, offset, size, data);

	onError([&target] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glGetBufferSubData", e, error::buffer_target(target));
	});
}

void
detail::shaderSource(
	gl::ShaderId		shader,
	gl::String const&	source
) {
	GLsizei const	count	= 1;
 	GLchar const*	string	= source.c_str();
	GLint const		length	= source.length();
	
	::glShaderSource(shader.value, count, &string, &length);
	
	onError([&shader] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glShaderSource", e, error::shader_id(shader));
	});
}

void
detail::deleteProgram(gl::ProgramId program)
{
	::glDeleteProgram(program.value);

	onError([&program] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glDeleteProgram", e, error::program_id(program));
	});
}

void
detail::useProgram(gl::ProgramId program)
{
	::glUseProgram(program.value);

	onError([&program] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glUseProgram", e, error::program_id(program));
	});
}

void
detail::useProgram()
{
	useProgram(ProgramId(0));
}

void
detail::linkProgram(gl::ProgramId program)
{
	::glLinkProgram(program.value);

	onError([&program] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glLinkProgram", e, error::program_id(program));
	});
}

void
detail::compileShader(gl::ShaderId shader)
{
	::glCompileShader(shader.value);

	onError([&shader] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glCompileShader", e, error::shader_id(shader));
	});
}

void
detail::attachShader(
	gl::ProgramId	program,
	gl::ShaderId	shader
) {
	::glAttachShader(program.value, shader.value);
			
	onError([&shader, &program] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glAttachShader", e,
			error::shader_id(shader) << error::program_id(program)
		);
	});
}

void
detail::detachShader(
	gl::ProgramId program,
	gl::ShaderId shader
) {
	::glDetachShader(program.value, shader.value);

	onError([&shader, &program] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glDetachShader", e,
			error::shader_id(shader) << error::program_id(program)
		);
	});
}

void
detail::deleteShader(gl::ShaderId shader)
{
	::glDeleteShader(shader.value);

	onError([&shader] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glDeleteShader", e, error::shader_id(shader));
	});
}

gl::AttributeLocation
detail::getAttribLocation(
	gl::ProgramId		program,
	gl::String const&	name
) {
	GLint const location = ::glGetAttribLocation(program.value, name.c_str());

	onError([&name, &program] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glGetAttribLocation", e,
			error::program_id(program) << error::attr_name(name)
		);
	});

	if (location == -1) {
		BOOST_THROW_EXCEPTION(::vox::gl::error::invalid_var()
			<< error::program_id(program)
			<< error::var_name(name)
			<< error::uniform_var(false)
			<< error::attr_var(true)
		);		
	}

	return AttributeLocation(static_cast<GLuint>(location));
}

gl::UniformLocation
detail::getUniformLocation(
	gl::ProgramId		program,
	gl::String const&	name
) {
	GLint const location = ::glGetUniformLocation(program.value, name.c_str());
			
	onError([&name, &program] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glGetUniformLocation", e,
			error::program_id(program) << error::uniform_name(name)
		);
	});
			
	if (location == -1) {
		BOOST_THROW_EXCEPTION(::vox::gl::error::invalid_var()
			<< error::program_id(program)
			<< error::var_name(name)
			<< error::uniform_var(true)
			<< error::attr_var(false)
		);		
	}

	return UniformLocation(static_cast<GLuint>(location));
}
		
gl::ShaderId
detail::createShader(gl::ShaderType shaderType)
{
	ShaderId const shader(::glCreateShader(shaderType));

	onError([&shaderType] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glCreateShader", e, error::shader_type(shaderType));
	});

	return shader;
}

gl::ProgramId
detail::createProgram()
{
	ProgramId const program(::glCreateProgram());

	onError([] (error::ErrorType e) {
		THROW_GL_ERROR("glCreateProgram", e);
	});

	if (program.value == 0) {
		BOOST_THROW_EXCEPTION(gl::error::gl_error()
			<< ::boost::errinfo_api_function("glCreateProgram")
		);
	}

	return program;
}

void
detail::getUniform(gl::ProgramId program, gl::UniformLocation location, GLfloat* params)
{
	::glGetUniformfv(program.value, location.value, params);

	onError([&program, &location] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glGetUniformfv", e,
			error::program_id(program) << error::uniform_loc(location)
		);
	});
}

void
detail::getUniform(gl::ProgramId program, gl::UniformLocation location, GLint* params)
{
	::glGetUniformiv(program.value, location.value, params);

	onError([&program, &location] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glGetUniformiv", e,
			error::program_id(program) << error::uniform_loc(location)
		);
	});
}

GLint
detail::get::shader::get_(gl::ShaderId shader, GLenum param) {
	GLint result;
	::glGetShaderiv(shader.value, param, &result);
					
	onError([&shader] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glGetShaderiv", e,
			error::shader_id(shader)
		);
	});

	return result;
}

unsigned
detail::get::shader::logLength(gl::ShaderId shader)
{
	return static_cast<unsigned>(
		get_(shader, GL_INFO_LOG_LENGTH)
	);
}

bool
detail::get::shader::isCompiled(gl::ShaderId shader)
{
	return get_(shader, GL_COMPILE_STATUS) == GL_TRUE;
}

gl::String
detail::get::shader::infoLog(gl::ShaderId shader)
{
	GLsizei const		maxLength = logLength(shader);
	GLsizei				length = 0;
	std::vector<GLchar>	buffer(maxLength);
	GLchar*	const		infoLog = &buffer[0];

	::glGetShaderInfoLog(shader.value, maxLength, &length, infoLog);

	onError([&shader] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glGetShaderInfoLog", e,
			error::shader_id(shader)
		);
	});	

	return String(infoLog, length);
}

GLint
detail::get::program::get_(gl::ProgramId program, GLenum param)
{
	GLint result;
	::glGetProgramiv(program.value, param, &result);
					
	onError([&program] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glGetProgramiv", e,
			error::program_id(program)
		);
	});	

	return result;
}

bool
detail::get::program::isDeleteFlagged(gl::ProgramId program)
{
	return get_(program, GL_DELETE_STATUS) == GL_TRUE;
}

bool
detail::get::program::isLinked(gl::ProgramId program)
{
	return get_(program, GL_LINK_STATUS) == GL_TRUE;
}

unsigned
detail::get::program::activeUniforms(gl::ProgramId program) {
	return get_(program, GL_ACTIVE_UNIFORMS);
}
unsigned
detail::get::program::activeUniformsMaxLength(gl::ProgramId program) {
	return get_(program, GL_ACTIVE_UNIFORM_MAX_LENGTH);
}

unsigned
detail::get::program::activeAttribs(gl::ProgramId program) {
	return get_(program, GL_ACTIVE_ATTRIBUTES);
}
unsigned
detail::get::program::activeAttribsMaxLength(gl::ProgramId program) {
	return get_(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH);
}

gl::String
detail::get::program::infoLog(gl::ProgramId program)
{
	GLsizei const		maxLength = get_(program, GL_INFO_LOG_LENGTH);
	GLsizei				length = 0;
	std::vector<GLchar>	buffer(maxLength);
	GLchar* const		infoLog = &buffer[0];

	::glGetProgramInfoLog(program.value, maxLength, &length, infoLog);
	
	onError([&program] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glGetProgramInfoLog", e,
			error::program_id(program)
		);
	});	

	return String(infoLog, length);			
}

namespace {
	typedef void (GLAPIENTRY *get_f)(GLuint, GLuint, GLsizei, GLsizei*, GLint*, GLenum*, GLchar* name);

	detail::variable_info getInfo(get_f get, gl::ProgramId program, GLuint index, GLsizei bufSize)
	{
		assert(bufSize > 0);
		assert(get != nullptr);

		GLsizei	length;
		GLint	size;
		GLenum	type;
		::std::vector<GLchar> name(bufSize);

		get(
			program.value, index, bufSize,
			&length, &size, &type, &name[0]
		);

		detail::variable_info const result = {index, size, (gl::VarType)type, &name[0]};

		return result;
	}

} //namespace anon

detail::variable_info
detail::getActiveUniform(gl::ProgramId program, GLuint index, GLsizei bufSize)
{
	auto result = getInfo(::glGetActiveUniform, program, index, bufSize);

	onError([&program] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glGetActiveUniform", e,
			error::program_id(program)
		);
	});

	result.qualifier = variable_info::TYPE_UNIFORM;

	return result;
}

detail::variable_info
detail::getActiveAttrib(gl::ProgramId program, GLuint index, GLsizei bufSize)
{
	auto result = getInfo(::glGetActiveAttrib, program, index, bufSize);

	onError([&program] (error::ErrorType e) {
		THROW_GL_ERROR_INFO("glGetActiveAttrib", e,
			error::program_id(program)
		);
	});

	result.qualifier = variable_info::TYPE_ATTRIBUTE;

	return result;
}