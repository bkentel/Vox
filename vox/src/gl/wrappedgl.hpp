#pragma once
#ifndef BKENTEL_VOX_GL_WRAPPED_GL_HPP
#define BKENTEL_VOX_GL_WRAPPED_GL_HPP

#include "exception.hpp"
#include <string>

namespace vox {
	namespace gl {
		template <typename handle_t> struct handle_deleter;

		typedef ::std::string			String;
		typedef ::std::wstring			FileName;
	
		///////////////////////////////////////////////////////////////////////
		// Simple wrapper class for opengl handles
		///////////////////////////////////////////////////////////////////////
		template <
			typename tag_t,				//tag type to uniquely identify the handle
			typename handle_t = GLuint,	//underlying type of the handle
            int default_value = static_cast<typename handle_t>(0)
		>
		struct Handle {
			typedef handle_t				handle_type;
			typedef Handle<tag_t, handle_t>	this_type;
			typedef ::std::unique_ptr<
				this_type,
				handle_deleter<this_type>
			>								unique_t;
            
            static handle_t default() {
                return static_cast<typename handle_t>(default_value);
            }

			Handle() : value(default()) {}
			explicit Handle(handle_type handle) : value(handle) {}

			bool operator<(this_type const& rhs)	const { return value < rhs.value; }
			bool operator>(this_type const& rhs)	const { return value > rhs.value; }
			bool operator==(this_type const& rhs)	const { return value == rhs.value; }
			bool operator!=(this_type const& rhs)	const { return value != rhs.value; }

			handle_t value;
		};

		///////////////////////////////////////////////////////////////////////
		// opengl handle type
		///////////////////////////////////////////////////////////////////////
		typedef Handle<struct tag_gl_program_id> ProgramId;
		typedef Handle<struct tag_gl_shader_id>  ShaderId;
		typedef Handle<struct tag_gl_buffer_id>  BufferId;
		typedef Handle<struct tag_gl_array_id>   ArrayId;
        typedef Handle<struct tag_gl_texture_id> TextureId;
        
        typedef Handle<struct tag_gl_texture_unit, GLenum> TextureUnit;

		typedef Handle<struct tag_gl_attribute_loc, GLint, -1>	AttributeLocation;
		typedef Handle<struct tag_gl_uniform_loc, GLint, -1>	UniformLocation;		
		
		///////////////////////////////////////////////////////////////////////
		// opengl enumerations
		///////////////////////////////////////////////////////////////////////
		enum DrawMode {
            DRAW_MODE_POINTS                   = GL_POINTS,
            DRAW_MODE_LINE_STRIP               = GL_LINE_STRIP,
            DRAW_MODE_LINE_LOOP                = GL_LINE_LOOP,
            DRAW_MODE_LINES                    = GL_LINES,
            DRAW_MODE_LINE_STRIP_ADJACENCY     = GL_LINE_STRIP_ADJACENCY,
            DRAW_MODE_LINES_ADJACENCY          = GL_LINES_ADJACENCY,
            DRAW_MODE_TRIANGLE_STRIP           = GL_TRIANGLE_STRIP,
            DRAW_MODE_TRIANGLE_FAN             = GL_TRIANGLE_FAN,
            DRAW_MODE_TRIANGLES                = GL_TRIANGLES,
            DRAW_MODE_TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY,
            DRAW_MODE_TRIANGLES_ADJACENCY      = GL_TRIANGLES_ADJACENCY,
        };
        
        enum BufferTarget {
			BUFFER_TARGET_ARRAY					= GL_ARRAY_BUFFER,
			BUFFER_TARGET_COPY_READ				= GL_COPY_READ_BUFFER,
			BUFFER_TARGET_COPY_WRITE			= GL_COPY_WRITE_BUFFER,
			BUFFER_TARGET_ELEMENT_ARRAY			= GL_ELEMENT_ARRAY_BUFFER,
			BUFFER_TARGET_PIXEL_PACK			= GL_PIXEL_PACK_BUFFER,
			BUFFER_TARGET_PIXEL_UNPACK			= GL_PIXEL_UNPACK_BUFFER,
			BUFFER_TARGET_TEXTURE				= GL_TEXTURE_BUFFER,
			BUFFER_TARGET_UNIFORM				= GL_UNIFORM_BUFFER,
			BUFFER_TARGET_TRANSFORM_FEEDBACK	= GL_TRANSFORM_FEEDBACK_BUFFER,
		};

		enum BufferUsage {
			BUFFER_USAGE_STREAM_DRAW	= GL_STREAM_DRAW,
			BUFFER_USAGE_STREAM_READ	= GL_STREAM_READ,
			BUFFER_USAGE_STREAM_COPY	= GL_STREAM_COPY, 
			BUFFER_USAGE_STATIC_DRAW	= GL_STATIC_DRAW,
			BUFFER_USAGE_STATIC_READ	= GL_STATIC_READ,
			BUFFER_USAGE_STATIC_COPY	= GL_STATIC_COPY, 
			BUFFER_USAGE_DYNAMIC_DRAW	= GL_DYNAMIC_DRAW,
			BUFFER_USAGE_DYNAMIC_READ	= GL_DYNAMIC_READ,
			BUFFER_USAGE_DYNAMIC_COPY	= GL_DYNAMIC_COPY,
		};

		enum BufferAccess {
			BUFFER_ACCESS_READ_ONLY		= GL_READ_ONLY,
			BUFFER_ACCESS_WRITE_ONLY	= GL_WRITE_ONLY,
			BUFFER_ACCESS_READ_WRITE	= GL_READ_WRITE,
		};

		enum ShaderType {
			SHADER_TYPE_VERTEX		= GL_VERTEX_SHADER, 
			SHADER_TYPE_FRAGMENT	= GL_FRAGMENT_SHADER,
			SHADER_TYPE_GEOMETRY	= GL_GEOMETRY_SHADER,
		};

		enum AttributeSize {
			ATTR_SIZE_1 = 1,
			ATTR_SIZE_2 = 2,
			ATTR_SIZE_3 = 3,
			ATTR_SIZE_4 = 4,
			ATTR_SIZE_BGRA = GL_BGRA,
		};

		enum DataType {
			DATA_TYPE_BYTE			= GL_BYTE,
			DATA_TYPE_UBYTE			= GL_UNSIGNED_BYTE,
			DATA_TYPE_SHORT			= GL_SHORT,
			DATA_TYPE_USHORT		= GL_UNSIGNED_SHORT,
			DATA_TYPE_INT			= GL_INT,
			DATA_TYPE_UINT			= GL_UNSIGNED_INT,
			DATA_TYPE_HALF_FLOAT	= GL_HALF_FLOAT,
			DATA_TYPE_FLOAT			= GL_FLOAT,
			DATA_TYPE_DOUBLE		= GL_DOUBLE,
			DATA_TYPE_INT_2AAA		= GL_INT_2_10_10_10_REV,
			DATA_TYPE_UINT_2AAA		= GL_UNSIGNED_INT_2_10_10_10_REV,
		};

		enum VarType {
			VAR_FLOAT		= GL_FLOAT,
			VAR_FLOAT_VEC2	= GL_FLOAT_VEC2,
			VAR_FLOAT_VEC3	= GL_FLOAT_VEC3,
			VAR_FLOAT_VEC4	= GL_FLOAT_VEC4,
			VAR_INT			= GL_INT,
			VAR_INT_VEC2	= GL_INT_VEC2,
			VAR_INT_VEC3	= GL_INT_VEC3,
			VAR_INT_VEC4	= GL_INT_VEC4,
			VAR_BOOL		= GL_BOOL,
			VAR_BOOL_VEC2	= GL_BOOL_VEC2,
			VAR_BOOL_VEC3	= GL_BOOL_VEC3,
			VAR_BOOL_VEC4	= GL_BOOL_VEC4,
			VAR_FLOAT_MAT22	= GL_FLOAT_MAT2,
			VAR_FLOAT_MAT33	= GL_FLOAT_MAT3,
			VAR_FLOAT_MAT44	= GL_FLOAT_MAT4,
			VAR_FLOAT_MAT23	= GL_FLOAT_MAT2x3,
			VAR_FLOAT_MAT24	= GL_FLOAT_MAT2x4,
			VAR_FLOAT_MAT32	= GL_FLOAT_MAT3x2,
			VAR_FLOAT_MAT34	= GL_FLOAT_MAT3x4,
			VAR_FLOAT_MAT42	= GL_FLOAT_MAT4x2,
			VAR_FLOAT_MAT43	= GL_FLOAT_MAT4x3,
			VAR_SAMPLER_1D	= GL_SAMPLER_1D,
			VAR_SAMPLER_2D	= GL_SAMPLER_2D,
			VAR_SAMPLER_3D	= GL_SAMPLER_3D,
			VAR_SAMPLER_CUBE		= GL_SAMPLER_CUBE,
			VAR_SAMPLER_1D_SHADOW	= GL_SAMPLER_1D_SHADOW,
			VAR_SAMPLER_2D_SHADOW	= GL_SAMPLER_2D_SHADOW,
		};

        enum TextureTarget {        
            TEXTURE_1D   = GL_TEXTURE_1D,
            TEXTURE_2D   = GL_TEXTURE_2D,
            TEXTURE_3D   = GL_TEXTURE_3D,
            TEXTURE_1DA  = GL_TEXTURE_1D_ARRAY,
            TEXTURE_2DA  = GL_TEXTURE_2D_ARRAY,
            TEXTURE_RECT = GL_TEXTURE_RECTANGLE,
            TEXTURE_CUBE = GL_TEXTURE_CUBE_MAP,
            TEXTURE_2DM  = GL_TEXTURE_2D_MULTISAMPLE,
            TEXTURE_2DMA = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
        };
                

		///////////////////////////////////////////////////////////////////////
		// opengl exceptions
		///////////////////////////////////////////////////////////////////////
		namespace error {
			enum ErrorType {
				ERROR_GL_NO_ERROR			= GL_NO_ERROR,
				ERROR_GL_INVALID_ENUM		= GL_INVALID_ENUM,
				ERROR_GL_INVALID_VALUE		= GL_INVALID_VALUE,
				ERROR_GL_INVALID_OPERATION	= GL_INVALID_OPERATION,
				ERROR_GL_STACK_OVERFLOW		= GL_STACK_OVERFLOW,
				ERROR_GL_STACK_UNDERFLOW	= GL_STACK_UNDERFLOW,
				ERROR_GL_OUT_OF_MEMORY		= GL_OUT_OF_MEMORY,
			};

			struct gl_error				: virtual vox::exception {};
			struct api_error			: virtual gl_error {};
			struct compilation_error	: virtual gl_error {};
			struct linker_error			: virtual gl_error {};
			struct invalid_var			: virtual gl_error {};
			struct type_mismatch		: virtual gl_error {};

			typedef ::boost::error_info<struct tag_error_num, ErrorType>			error_num;
			typedef ::boost::error_info<struct tag_program_id, ProgramId>			program_id;
			
			typedef ::boost::error_info<struct tag_shader_id, ShaderId>				shader_id;
			typedef ::boost::error_info<struct tag_shader_type, ShaderType>			shader_type;
			
			typedef ::boost::error_info<struct tag_attr_name, String>				attr_name;
			typedef ::boost::error_info<struct tag_attr_loc, AttributeLocation>		attr_loc;
			
			typedef ::boost::error_info<struct tag_uniform_name, String>			uniform_name;
			typedef ::boost::error_info<struct tag_uniform_loc, UniformLocation>	uniform_loc;

			typedef ::boost::error_info<struct tag_buffer_id, BufferId>				buffer_id;
			typedef ::boost::error_info<struct tag_buffer_target, BufferTarget>		buffer_target;

			typedef ::boost::error_info<struct tag_array_id, ArrayId>				array_id;

			typedef ::boost::error_info<struct tag_info_log, String>				info_log;
			typedef ::boost::error_info<struct tag_file_name, FileName>				file_name;

			typedef ::boost::error_info<struct tag_var_name, String>				var_name;
			typedef ::boost::error_info<struct tag_uniform_var, bool>				uniform_var;
			typedef ::boost::error_info<struct tag_attr_var, bool>					attr_var;
		} // namespace error

		namespace detail {
			struct variable_info {
				unsigned	index;
				unsigned	size;
				gl::VarType	type;
				String		name;

				enum VarQualifier {
					TYPE_UNIFORM,
					TYPE_ATTRIBUTE,
				} qualifier;

				bool isUniform()	const { return qualifier == TYPE_UNIFORM; }
				bool isAttribute()	const { return qualifier == TYPE_ATTRIBUTE; }

				template <typename var_t>
				bool isDefinedAs() const {
					return	isAttribute()	== var_t::traits::qualifier::isAttribute &&
							type			== var_t::traits::variable::type_id;
				}
			};

			void onError(::std::function<void (error::ErrorType glError)> errfunc);

			void deleteTexture(TextureId texture);
            TextureId genTexture();
            void bindTexture(TextureTarget target, TextureId texture);

            ::std::vector<BufferId> genBuffers(unsigned n);
			BufferId genBuffer();
			void deleteBuffers(std::vector<BufferId> const& buffers);
			void deleteBuffer(BufferId buffer);
			void bindBuffer(BufferTarget target, BufferId buffer);
			void unbindBuffer(BufferTarget target);
			void bufferData(BufferTarget target, GLsizeiptr size, const GLvoid* data, BufferUsage usage);
			void bufferSubData(BufferTarget target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
			void getBufferSubData(BufferTarget target, GLintptr offset, GLsizeiptr size, GLvoid* data);

			ProgramId createProgram();
			void deleteProgram(ProgramId program);
			void linkProgram(ProgramId program);
			void useProgram(ProgramId program);
			void useProgram();

			ShaderId createShader(ShaderType shaderType);
			void shaderSource(ShaderId shader, String const& source);
			void compileShader(ShaderId shader);
			void deleteShader(ShaderId shader);
			void attachShader(ProgramId program, ShaderId shader);
			void detachShader(ProgramId program, ShaderId shader);

			::std::vector<ArrayId> genVertexArrays(unsigned n);
			ArrayId genVertexArray();
			void deleteVertexArrays(std::vector<ArrayId>& arrays);
			void deleteVertexArray(ArrayId array);
			void bindVertexArray(ArrayId array);
			void bindVertexArray();
			void vertexAttribPointer(AttributeLocation index, AttributeSize size, DataType type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
			void enableVertexAttribArray(AttributeLocation index);
			void disableVertexAttribArray(AttributeLocation index);

            void drawArrays(DrawMode mode, GLint first, GLsizei count);

			AttributeLocation getAttribLocation(ProgramId program, String const& name);
			
			variable_info getActiveUniform(ProgramId program, GLuint index, GLsizei bufSize);

			variable_info getActiveAttrib(ProgramId program, GLuint index, GLsizei bufSize);

			UniformLocation getUniformLocation(ProgramId program, String const& name);
			void getUniform(ProgramId program, UniformLocation location, GLfloat* params);
			void getUniform(ProgramId program, UniformLocation location, GLint* params);

			namespace {
				template <unsigned rows, unsigned cols, typename T>
				void setUniform(
					UniformLocation	location,
					T const*		data,
					unsigned		size		= 1,
					GLboolean		transpose	= GL_FALSE
				);

				template <>
				void setUniform<4, 4, GLfloat>(UniformLocation location, GLfloat const* data, unsigned size, GLboolean transpose) {
					::glUniformMatrix4fv(location.value, size, transpose, data);

					onError([&location] (error::ErrorType e) {
						BOOST_THROW_EXCEPTION(error::api_error()
							<< boost::errinfo_api_function("glUniformMatrix4fv") << error::error_num(e)
							<< error::uniform_loc(location)
						);
					});
				}
			} // namespace anon

			namespace get {
				struct global {
					static ProgramId currentProgram() {
						return ProgramId(
							get_(GL_CURRENT_PROGRAM)
						);
					}

					template <BufferTarget target_t>
					static BufferId bufferBinding();

					template <> static BufferId bufferBinding<BUFFER_TARGET_ARRAY>() {
						return BufferId(get_(GL_ARRAY_BUFFER_BINDING));
					}
				private:
					static GLint get_(GLenum param) {
						GLint result;
						::glGetIntegerv(param, &result);
						return result;
					}
				};

				struct vertexArray {
					static BufferId binding(AttributeLocation index) {
						return BufferId(get_(index, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING));
					}
				private:
					static GLint get_(AttributeLocation index, GLenum pname) {
						GLint result;
						::glGetVertexAttribiv(index.value, pname, &result);
						return result;
					}
				};

				struct program {
					static bool		isDeleteFlagged(ProgramId program);
					static bool		isLinked(ProgramId program);
					static String	infoLog(ProgramId program);
					static unsigned	activeUniforms(ProgramId program);
					static unsigned activeUniformsMaxLength(ProgramId program);
					static unsigned	activeAttribs(ProgramId program);
					static unsigned activeAttribsMaxLength(ProgramId program);
				private:
					static GLint get_(ProgramId program, GLenum param);
				};

				struct shader {
					static unsigned	logLength(ShaderId shader);
					static bool		isCompiled(ShaderId shader);
					static String	infoLog(ShaderId shader);
				private:
					static GLint get_(ShaderId shader, GLenum param);
				};

				template <BufferTarget target_t>
				struct buffer {
					static BufferAccess access()	{ return static_cast<BufferAccess>(get_(GL_BUFFER_ACCESS)); }
					static bool isMapped()			{ return get_(GL_BUFFER_MAPPED) == GL_TRUE; }
					static unsigned size()			{ return static_cast<unsigned>(get_(GL_BUFFER_SIZE)); }
					static BufferUsage usage()		{ return static_cast<BufferUsage>(get_(GL_BUFFER_USAGE)); }
				private:
					static GLint get_(GLenum param) {
						GLint result;
						::glGetBufferParameteriv(target_t, param, &result);
						return result;
					}
				};
			} //namespace get

		} // namespace detail
	
		//deleter for ShaderId
		template <> struct handle_deleter<ShaderId> {
			typedef ShaderId pointer;

			void operator()(ShaderId const& handle) const {
				detail::deleteShader(handle);
			}
		};

		//deleter for ProgramId
		template <> struct handle_deleter<ProgramId> {
			typedef ProgramId pointer;

			void operator()(ProgramId const& handle) const {
				detail::deleteProgram(handle);
			}
		};

		//deleter for BufferId
		template <> struct handle_deleter<BufferId> {
			typedef BufferId pointer;

			void operator()(BufferId const& handle) const {
				detail::deleteBuffer(handle);
			}
		};

		//deleter for ArrayId
		template <> struct handle_deleter<ArrayId> {
			typedef ArrayId pointer;

			void operator()(ArrayId const& handle) const {
				detail::deleteVertexArray(handle);
			}
		};

		template <typename handle_t>
		struct unique_handle {
			typedef ::std::unique_ptr<handle_t, gl::handle_deleter<handle_t>> type;
		};
	} // namespace gl
} // namespace vox

#endif //BKENTEL_VOX_GL_WRAPPED_GL_HPP