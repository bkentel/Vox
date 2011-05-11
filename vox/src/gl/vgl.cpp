#include "common.hpp"
#include "vgl.hpp"

namespace gl = ::vox::gl;

namespace {
    std::vector<char>
    readFile(std::wstring const& fileName)
    {
	    std::ifstream in(fileName, std::ios::binary);
	    in.exceptions(std::ios::eofbit | std::ios::failbit | std::ios::badbit);

	    in.seekg(0, std::ios::end);
	    auto const len = in.tellg();
	    in.seekg(0, std::ios::beg);

	    std::vector<char> result(static_cast<unsigned>(len.seekpos() + 1));

	    in.read(&result[0], len.seekpos());

	    return result;
    }
} //namespace anon

gl::Shader::Shader(
	FileName const& filename,
	ShaderType shaderType
)
	: id_(detail::createShader(shaderType))
	, type_(shaderType)
{	
	//translate the exception
	try {
		detail::shaderSource(id(), &readFile(filename)[0]); //TODO
	} catch (std::runtime_error& e) {
        throw;
		//BOOST_THROW_EXCEPTION(vox::error::file_error()
		//	<< vox::error::wfile_name(filename)
		//);
	}

	detail::compileShader(id());

	if (!detail::get::shader::isCompiled(id())) {
		auto const log = detail::get::shader::infoLog(id());

		BOOST_THROW_EXCEPTION(error::compilation_error()
			<< error::shader_type(shaderType)
			<< error::info_log(log)
			<< error::file_name(filename)
		);
	}
}
		
gl::Shader::~Shader()
try {
} catch (error::gl_error&) {
	assert(0);
}

gl::Program::Program()
	: program_(detail::createProgram())
	, shaders_()
{
}

gl::Program::~Program()
try {
	detachAllShaders();

	if (detail::get::global::currentProgram() == id()) {
		detail::useProgram();
	}
} catch (error::gl_error& ) {
	assert(0 && "TODO log");
}


void
gl::Program::attachShader(std::shared_ptr<Shader> shader)
{
	auto it = shaders_.find(shader);
		
	//if shader is already attached, don't attach it again 
	if (it != shaders_.end()) {
		return;
	}

	shaders_.insert(shader);
	detail::attachShader(id(), shader->id());
}

void
gl::Program::detachShader(std::shared_ptr<Shader> shader)
{
	auto it = shaders_.find(shader);

	//if shader hasn't been attached, do nothing
	if (it == shaders_.end()) {
		return;
	}

	shaders_.erase(it);
	detail::detachShader(id(), shader->id());
}

void
gl::Program::detachAllShaders()
{
	while (!shaders_.empty()) {
		detachShader(*shaders_.begin());
	}
}

void
gl::Program::link()
{
	detail::linkProgram(id());

	if (!detail::get::program::isLinked(id())) {
		auto const log = detail::get::program::infoLog(id());

		BOOST_THROW_EXCEPTION(error::linker_error()
			<< error::program_id(id())
			<< error::info_log(log)
		);
	}

	vars_.enumerate(id());
}

void
gl::Program::use()
{
	detail::useProgram(id());
}

namespace {
	template <typename qualifier>
	struct info_getter : private boost::noncopyable {
		explicit info_getter(gl::ProgramId program)
			: program(program)
			, count(qualifier::activeCount(program))
			, maxLength(qualifier::activeMaxLength(program))
		{
		}
		
		gl::detail::variable_info get(unsigned index) const {
			return qualifier::getInfo(program, index, maxLength);
		}

		gl::ProgramId const	program;
		unsigned const		count;
		unsigned const		maxLength;
	};

} //namespace anon

void
gl::VariableSet::enumerate(ProgramId program)
{
	vars_.clear();

	auto inserter = [this](info_t info) -> void {
		auto const result = vars_.insert(
			::vox::gl::VariableSet::info_container_t::value_type(info.name, info)
		);

		if (result.second == false) {
			assert(0 && "TODO throw");
		}		
	};

	info_getter<gl::traits::attribute> attributes(program);
	for (unsigned i = 0; i < attributes.count; ++i) {
		inserter(attributes.get(i));
	}

	info_getter<gl::traits::uniform> uniforms(program);
	for (unsigned i = 0; i < uniforms.count; ++i) {
		inserter(uniforms.get(i));
	}
}

bool
gl::VariableSet::isUniform(String const& name) const
{
	auto const result = vars_.find(name);

	return	result != vars_.end() &&
			result->second.qualifier == info_t::TYPE_UNIFORM;
}

bool
gl::VariableSet::isAttrib(String const& name) const
{
	auto const result = vars_.find(name);

	return	result != vars_.end() &&
			result->second.qualifier == info_t::TYPE_ATTRIBUTE;
}

bool
gl::VariableSet::isDefined(String const& name) const
{
	return vars_.find(name) != vars_.end();
}

gl::VariableSet::info_t
gl::VariableSet::getInfo(String const& name) const
{
	auto const result = vars_.find(name);

	if (result == vars_.end()) {
		assert(0 && "TODO throw");
	}

	return result->second;
}
