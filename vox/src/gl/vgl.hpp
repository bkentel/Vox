#pragma once
#ifndef BKENTEL_VOX_GL_VGL_HPP
#define BKENTEL_VOX_GL_VGL_HPP

#include "wrappedgl.hpp"
#include "gltraits.hpp"

namespace vox {
	namespace gl {
		class Shader;
		class Program;
		class VariableSet;

		template <
			typename qualifier_t,
			typename data_t
		> class Variable;
		
		template <
			unsigned stride_t = 0,
			GLboolean normalized_t = GL_FALSE
		> class VertexArray;

		template <
			gl::BufferUsage		usage_t,
			gl::BufferTarget	target_t = gl::BUFFER_TARGET_ARRAY
		> class Buffer;


        class Texture {
        };

        template <
            typename qualifier_t,
            typename category_t,
            typename data_t,
            unsigned cols_t,
            unsigned rows_t,
            unsigned count_t     = 1,
            bool     transpose_t = false
        >
        class var {
        public:
            struct traits {
                typedef qualifier_t   qualifier;
                typedef category_t    category;
                typedef data_t        data;
                static unsigned const cols      = cols_t;
                static unsigned const rows      = rows_t;
                static unsigned const count     = count_t;
                static bool const     transpose = transpose_t;
            };

            var(ProgramId program, String const& name)
                : location_(
                    traits::qualifier::getLocation(program, name)
                )
            {
            }

            void set(typename traits::data::type const* data) const {
                gl::traits::set_variable<qualifier_t, category_t, data_t, cols_t, rows_t>::set(
                    location_, data
                );
            }


        private:
            UniformLocation location_;
        };

        typedef var<traits::qualifier_uniform, traits::category_matrix, traits::data_float, 4, 4> mat4;
        typedef var<traits::qualifier_uniform, traits::category_sampler, traits::data_int, 1, 1> sampler2D;

		////////////////////////////////////////////////////////////////////////////////
		// Represents an opengl shader program variable
		// qualifier_t: uniform or attribute
		// variable_t: variable<>
		////////////////////////////////////////////////////////////////////////////////
		template <typename qualifier_t, typename variable_t>
		class Variable {
		public:
			struct traits {
				typedef variable_t	variable;
				typedef qualifier_t	qualifier;
			};
			typedef typename traits::qualifier::location_t	location_t;
			typedef typename traits::variable::math_t		math_t;

            Variable()
                : program_()
                , location_()
            {
            }

			Variable(ProgramId program, String const& name)
				: program_(program)
				, location_(traits::qualifier::getLocation(program, name))
			{}

			location_t location() const {
				return location_;
			}

			void get(math_t& out) const {
				traits::qualifier::getData(program_, location_, out.data());
			}

			void set(math_t const& value) {
				traits::qualifier::setData<traits::variable>(location_, value.data());
			}

			bool isType(gl::detail::variable_info const& info) const {
				return	traits::variable::type_id		== info.type &&
						traits::qualifier::isAttribute	== info.isAttribute();
			}
		private:
			ProgramId	program_;
			location_t	location_;
		};

		//uniform variable typedefs
		namespace uniform {
			typedef Variable<traits::uniform, traits::mat4>		mat4f;
			typedef Variable<traits::uniform, traits::vec3f>	vec3f;
		} //namespace uniform

		//attribute variable typedefs
		namespace attribute {
			typedef Variable<traits::attribute, traits::vec3f> vec3f;
		} //namespace uniform

		////////////////////////////////////////////////////////////////////////////////
		// Represents an opengl Shader object
		////////////////////////////////////////////////////////////////////////////////
		class Shader : private ::boost::noncopyable {
		public:
			//Load a shader of type [shaderType] from file [filename]
			Shader(FileName const& filename, ShaderType shaderType);
			~Shader();

			ShaderId	id()	const { return id_.get(); }
			ShaderType	type()	const { return type_; }
		private:
			unique_handle<ShaderId>::type	id_;
			ShaderType						type_;
		};
	
		////////////////////////////////////////////////////////////////////////////////
		// Represents an opengl Buffer object bound to target_t with usage usage_t
		////////////////////////////////////////////////////////////////////////////////
		template <gl::BufferUsage usage_t, gl::BufferTarget target_t>
		class Buffer : private ::boost::noncopyable {
			template <gl::BufferUsage U, gl::BufferTarget T> friend class Buffer;
		public:
			typedef traits::buffer<target_t, usage_t> traits;

			Buffer()
				: id_(detail::genBuffer())
			{
			}

			Buffer(Buffer&& other)
				: id_(std::move(other.id_))
			{
			}

			Buffer& operator=(Buffer&& rhs) {				
				id_ = std::move(rhs.id_);
				return *this;
			}

			//adopt from another buffer type
			template <typename buffer_t>
			Buffer(buffer_t&& other)
				: id_(std::move(other.id_))
			{
			}

			void bind() const {
				detail::bindBuffer(traits::target, id());
			}
			
			void unbind() const {
				detail::unbindBuffer(traits::target);
			}

			void allocate(unsigned size) {
				allocateAndSet(size, nullptr);
			}
			
			void allocateAndSet(unsigned size, const GLvoid* data) {
				assert(isBound());
				detail::bufferData(traits::target, size, data, traits::usage);
			}
			
			void setData(unsigned offset, unsigned size, const GLvoid* data) {
				assert(data);
				detail::bufferSubData(traits::target, offset, size, data);
			}
			
			void getData(unsigned offset, unsigned size, GLvoid* out) const {
				assert(isBound());
				assert(out);
				detail::getBufferSubData(traits::target, offset, size, out);
			};

			void mapData();

			bool isBound() const {
				return detail::get::global::bufferBinding<traits::target>() == id();
			}

			bool isMapped() const {
				assert(isBound());
				return detail::get::buffer<traits::target>::isMapped();
			}

			unsigned size() const {
				assert(isBound());
				return detail::get::buffer<traits::target>::size();
			}

			BufferId id() const { return id_.get(); }
		private:
			BufferId::unique_t id_;
		};

		////////////////////////////////////////////////////////////////////////////////
		// Represents the set of all defined opengl shader variables
		////////////////////////////////////////////////////////////////////////////////
		class VariableSet {
		public:
			typedef detail::variable_info info_t;

			void enumerate(ProgramId program);

			bool isUniform(String const& name) const;
			bool isAttrib(String const& name) const;
			bool isDefined(String const& name) const;

			info_t getInfo(String const& name) const;
		private:
			typedef ::std::map<String, info_t> info_container_t;

			info_container_t vars_;
		};

		////////////////////////////////////////////////////////////////////////////////
		// Represents an opengl Program object
		////////////////////////////////////////////////////////////////////////////////
		class Program : private ::boost::noncopyable {
		public:
			//var_t: traits::uniform or traits::attribute
			template <typename var_t>
			var_t variable(String const& name) const {
				auto const result = var_t(id(), name);
				
				if (!result.isType(vars_.getInfo(name))) {
					BOOST_THROW_EXCEPTION(
						error::type_mismatch() << error::program_id(id())
					);
				}

				return result;
			}

			detail::variable_info getVarInfo(String const& name) const {
				return vars_.getInfo(name);
			}

            template <typename variable_t>
            variable_t getVariable(String const& name) const {
                return var<
                    typename variable_t::traits::qualifier,
                    typename variable_t::traits::category,
                    typename variable_t::traits::data,
                    variable_t::traits::cols,
                    variable_t::traits::rows
                >(id(), name);
            }

			explicit Program();
			~Program();

			void link();
			void use();

			void attachShader(std::shared_ptr<Shader> shader);
			void detachShader(std::shared_ptr<Shader> shader);
			void detachAllShaders();

			ProgramId id() const { return program_.get(); }
		private:
			typedef ::std::shared_ptr<Shader> shaders_key_t;
			struct shaders_less_t {
				bool operator()(shaders_key_t const& lhs, shaders_key_t const& rhs) const {
					return lhs->id() < rhs->id();
				}
			};
			typedef ::std::set<shaders_key_t, shaders_less_t> shaders_container_t;
			
			ProgramId::unique_t	program_;
			shaders_container_t	shaders_; //shaders attached to the program
			VariableSet			vars_;		//set of all variables
		};

		////////////////////////////////////////////////////////////////////////////////
		// Represents an opengl vertex array object
		////////////////////////////////////////////////////////////////////////////////
		template <unsigned stride_t, GLboolean normalized_t>
		class VertexArray : private boost::noncopyable {
		public:
			struct traits {
				static unsigned const	stride		= stride_t;
				static GLboolean const	normalized	= normalized_t;
			};

			VertexArray()
				: array_(detail::genVertexArray())
			{
			}

            //Move
            VertexArray(VertexArray&& other)
                : array_(std::move(other.array_))
            {
            }

            //Move
            VertexArray& operator=(VertexArray&& right) {
                array_ = std::move(right.array_);
                return *this;
            }

			~VertexArray() {
			}

			void bind()		{ detail::bindVertexArray(id()); }
			void unbind()	{ detail::bindVertexArray(); }

			ArrayId id() const {
				return array_.get();
			}

			BufferId binding(AttributeLocation index) const {
				return detail::get::vertexArray::binding(index);
			}

			bool hasBinding(AttributeLocation index) const {
				binding(index).value == 0;
			}

			template <typename var_t>
			void setAttributePtr(
				Variable<gl::traits::attribute, var_t> const& var,
				GLvoid const* offset = nullptr
			) {
				gl::traits::vertex_array<
					var_t::element_t::type_id,
					static_cast<gl::AttributeSize>(var_t::elements),
					traits::stride,
					traits::normalized
				>::attributePointer(var.location(), offset);
			}

			template <typename var_t>
			void enableAttribute(Variable<gl::traits::attribute, var_t> const& var) {		
				gl::detail::enableVertexAttribArray(var.location());
			}

			template <typename var_t>
			void disableAttribute(Variable<gl::traits::attribute, var_t> const& var) {
				gl::detail::disableVertexAttribArray(var.location());
			}

            void draw(gl::DrawMode mode, unsigned count, unsigned first = 0) {
                gl::detail::drawArrays(mode, first, count);
            }
		private:
			ArrayId::unique_t array_;
		};
		typedef VertexArray<0> SimpleVertexArray;

	} //namespace gl
} //namespace vox

#endif // BKENTEL_VOX_GL_VGL_HPP