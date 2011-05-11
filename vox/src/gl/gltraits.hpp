#pragma once
#ifndef BKENTEL_VOX_GL_TRAITS_HPP
#define BKENTEL_VOX_GL_TRAITS_HPP

#pragma warning(disable : 4127)
#	include <Eigen/geometry>
#pragma warning(default : 4127)

#include "wrappedgl.hpp"

namespace vox {
	namespace gl {
		namespace traits {
////////////////////////////////////////////////////////////////////////////////
            struct tag_gl_uniform   {};
            struct tag_gl_attribute {};

            template <typename tag_t> struct qualifier_t;

            //uniform functions
            template <> struct qualifier_t<tag_gl_uniform> {
                static UniformLocation getLocation(ProgramId program, String const& name) {
                    return detail::getUniformLocation(program, name);
                }
                
                static void setLocation(UniformLocation location) {
                }
            };

            //attribute functions
            template <> struct qualifier_t<tag_gl_attribute> {
                static AttributeLocation getLocation(ProgramId program, String const& name) {
                    return detail::getAttribLocation(program, name);
                }
                
                static void setLocation(AttributeLocation location) {
                }
            };


            typedef qualifier_t<tag_gl_uniform>   qualifier_uniform;
            typedef qualifier_t<tag_gl_attribute> qualifier_attribute;
////////////////////////////////////////////////////////////////////////////////
            template <typename tag_t> struct category_t {};

            typedef category_t<struct tag_gl_scalar>  category_scalar;
            typedef category_t<struct tag_gl_vector>  category_vector;
            typedef category_t<struct tag_gl_matrix>  category_matrix;
            typedef category_t<struct tag_gl_sampler> category_sampler;
            
            template <typename tag_t> struct data_t;

////////////////////////////////////////////////////////////////////////////////
            struct tag_gl_float  {};
            struct tag_gl_int    {};
            struct tag_gl_uint   {};
            struct tag_gl_bool   {};
            struct tag_gl_double {};

            template <> struct data_t<tag_gl_float> {
                static GLenum const id = GL_FLOAT;
                typedef GLfloat type;
            };

            template <> struct data_t<tag_gl_int> {
                static GLenum const id = GL_INT;
                typedef GLint type;
            };

            template <> struct data_t<tag_gl_uint> {
                static GLenum const id = GL_UNSIGNED_INT;
                typedef GLuint type;
            };

            template <> struct data_t<tag_gl_bool> {
                static GLenum const id = GL_BOOL;
                typedef GLboolean type;
            };

            typedef data_t<tag_gl_float> data_float;
            typedef data_t<tag_gl_int>   data_int;
            typedef data_t<tag_gl_uint>  data_uint;
            typedef data_t<tag_gl_bool>  data_bool;

////////////////////////////////////////////////////////////////////////////////
            template <typename data_t>
            struct scalar_t {
                typedef data_t type;

                static_assert(
                    type::id == GL_FLOAT        ||
                    type::id == GL_INT          ||
                    type::id == GL_UNSIGNED_INT ||
                    type::id == GL_BOOL,
                    "invalid scalar data type"
                );
            };
////////////////////////////////////////////////////////////////////////////////
            template <typename data_t, unsigned size_t>
            struct vector_t {
                static unsigned const size = size_t;
                typedef typename data_t type;

                static_assert(size > 1 && size <= 4, "invalid vector size");
                static_assert(
                    type::id == GL_FLOAT        ||
                    type::id == GL_INT          ||
                    type::id == GL_UNSIGNED_INT ||
                    type::id == GL_BOOL,
                    "invalid vector data type"
                );
            };

////////////////////////////////////////////////////////////////////////////////

            template <typename category_t, typename data_t, unsigned cols, unsigned rows>
            struct make_variable;

            //make scalar_t
            template <typename data_t, unsigned cols, unsigned rows>
            struct make_variable<category_scalar, data_t, cols, rows> {
                static_assert(cols == 1 && rows == 1, "scalars must be 1x1");
                typedef typename scalar_t<data_t> type;
            };

            //make vector_t
            template <typename data_t, unsigned cols, unsigned rows>
            struct make_variable<category_vector, data_t, cols, rows> {
                static_assert(cols == 1, "vectors 1xn");
                typedef typename vector_t<data_float, rows> type;
            };


////////////////////////////////////////////////////////////////////////////////

            template <
                typename qualifier_t,
                typename category_t,
                typename data_t,
                unsigned cols_t,
                unsigned rows_t,
                unsigned count_t     = 1,
                bool     transpose_t = false
            > struct set_variable;

            template <
                typename category_t,
                typename data_t,
                unsigned cols_t,
                unsigned rows_t,
                unsigned count_t     = 1,
                bool     transpose_t = false
            > struct set_uniform;

            template <
                typename data_t,
                unsigned count_t = 1
            > struct set_uniform_scalar;

            template <
                typename data_t,
                unsigned size_t,
                unsigned count_t = 1
            > struct set_uniform_vector;

            template <
                typename data_t,
                unsigned cols_t,
                unsigned rows_t,
                unsigned count_t     = 1,
                bool     transpose_t = false
            > struct set_uniform_matrix;

            ////////////////////////////////////////////////////////////////////
            // Uniform qualifier specializations
            ////////////////////////////////////////////////////////////////////
            template <
                typename category_t,
                typename data_t,
                unsigned cols_t,
                unsigned rows_t,
                unsigned count_t,
                bool     transpose_t
            >
            struct set_variable<qualifier_uniform, category_t, data_t, cols_t, rows_t, count_t, transpose_t> {
                typedef typename data_t::type type;
                typedef typename set_uniform<category_t, data_t, cols_t, rows_t, count_t, transpose_t> setter;

                static void set(UniformLocation location, type x) {
                    setter::set(location, x);
                }
                static void set(UniformLocation location, type x, type y) {
                    setter::set(location, x, y);
                }
                static void set(UniformLocation location, type x, type y, type z) {
                    setter::set(location, x, y, z);
                }
                static void set(UniformLocation location, type x, type y, type z, type w) {
                    setter::set(location, x, y, z, w);
                }
                static void set(UniformLocation location, type const* data) {
                    setter::set(location, data);
                }
            };

            ////////////////////////////////////////////////////////////////////
            // Matrix category specializations
            ////////////////////////////////////////////////////////////////////
            template <
                typename data_t,
                unsigned cols_t,
                unsigned rows_t,
                unsigned count_t,
                bool     transpose_t
            >
            struct set_uniform<category_matrix, data_t, cols_t, rows_t, count_t, transpose_t> {
                typedef typename data_t::type type;
                
                static_assert(cols_t >= 2 && cols_t <= 4, "matricies must have between 2 and 4 cols");
                static_assert(rows_t >= 2 && rows_t <= 4, "matricies must have between 2 and 4 rows");
                static_assert(count_t >= 1, "count must be at least 1");

                static void set(UniformLocation location, type const* data) {
                    set_uniform_matrix<data_t, cols_t, rows_t>::set(location, data);
                }
            };

            //uniform mat2
            template <unsigned count_t, bool transpose_t>
            struct set_uniform_matrix<data_float, 2, 2, count_t, transpose_t> {
                static void set(UniformLocation location, GLfloat const* data) {
                    ::glUniformMatrix2fv(location.value, count_t, transpose_t ? GL_TRUE : GL_FALSE, data);
                }
            };

            //uniform mat3
            template <unsigned count_t, bool transpose_t>
            struct set_uniform_matrix<data_float, 3, 3, count_t, transpose_t> {
                static void set(UniformLocation location, GLfloat const* data) {
                    ::glUniformMatrix3fv(location.value, count_t, transpose_t ? GL_TRUE : GL_FALSE, data);
                }
            };

            //uniform mat4
            template <unsigned count_t, bool transpose_t>
            struct set_uniform_matrix<data_float, 4, 4, count_t, transpose_t> {
                static void set(UniformLocation location, GLfloat const* data) {
                    ::glUniformMatrix4fv(location.value, count_t, transpose_t ? GL_TRUE : GL_FALSE, data);
                }
            };

            ////////////////////////////////////////////////////////////////////
            // Vector category specializations
            ////////////////////////////////////////////////////////////////////
            template <
                typename data_t,
                unsigned cols_t,
                unsigned rows_t,
                unsigned count_t
            >
            struct set_uniform<category_vector, data_t, cols_t, rows_t, count_t> {
                typedef typename data_t::type type;
                typedef typename set_uniform_vector<data_t, cols_t*rows_t, count_t> setter;

                static_assert(cols_t == 1, "vectors must have 1 column");
                static_assert(rows_t >= 2 && rows_t <= 4, "vectors must have between 2 and 4 rows");
                static_assert(count_t >= 1, "count must be at least 1");

                static void set(UniformLocation location, type x, type y) {
                    static_assert(size_t == 2, "size mismatch");
                    setter::set(location, x, y);
                }
                static void set(UniformLocation location, type x, type y, type z) {
                    static_assert(size_t == 3, "size mismatch");
                    setter::set(location, x, y, z);
                }
                static void set(UniformLocation location, type x, type y, type z, type w) {
                    static_assert(size_t == 4, "size mismatch");
                    setter::set(location, x, y, z, w);
                }
                static void set(UniformLocation location, GLfloat const* data) {
                    setter::set(location, data);
                }
            };
            
            //uniform vec2
            template <unsigned count_t>
            struct set_uniform_vector<data_float, 2, count_t> {
                static void set(UniformLocation location, GLfloat x, GLfloat y) {
                    ::glUniform2f(location.value, x, y);
                }
                static void set(UniformLocation location, GLfloat const* data) {
                    ::glUniform2fv(location, count_t, data);
                }
            };

            //uniform vec3
            template <unsigned count_t>
            struct set_uniform_vector<data_float, 3, count_t> {
                static void set(UniformLocation location, GLfloat x, GLfloat y, GLfloat z) {
                    ::glUniform3f(location.value, x, y, z);
                }
                static void set(UniformLocation location, GLfloat const* data) {
                    ::glUniform3fv(location, count_t, data);
                }
            };

            //uniform vec4
            template <unsigned count_t>
            struct set_uniform_vector<data_float, 4, count_t> {
                static void set(UniformLocation location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
                    ::glUniform4f(location.value, x, y, z, w);
                }
                static void set(UniformLocation location, GLfloat const* data) {
                    ::glUniform4fv(location, count_t, data);
                }
            };

            ////////////////////////////////////////////////////////////////////
            // Scalar category specializations
            ////////////////////////////////////////////////////////////////////
            template <typename data_t, unsigned cols_t, unsigned rows_t>
            struct set_uniform<category_scalar, data_t, cols_t, rows_t> {
                typedef typename data_t::type type;

                static_assert(cols_t*rows_t == 1, "scalars must be of size 1");

                static void set(UniformLocation location, type x) {
                    set_uniform_scalar<data_t>::set(location, x);
                }
            };

            //uniform float
            template <> struct set_uniform_scalar<data_float> {
                static void set(UniformLocation location, GLfloat x) {
                    ::glUniform1f(location.value, x);
                }
            };

            //uniform int
            template <> struct set_uniform_scalar<data_int> {
                static void set(UniformLocation location, GLint x) {
                    ::glUniform1i(location.value, x);
                }
            };

            //uniform uint
            template <> struct set_uniform_scalar<data_uint> {
                static void set(UniformLocation location, GLuint x) {
                    ::glUniform1ui(location.value, x);
                }
            };

            //uniform bool
            template <> struct set_uniform_scalar<data_bool> {
                static void set(UniformLocation location, GLboolean x) {
                    ::glUniform1i(location.value, x);
                }
            };

            ////////////////////////////////////////////////////////////////////
            // Sampler category specializations
            ////////////////////////////////////////////////////////////////////
            // uniform sampler
            template <typename data_t, unsigned cols_t, unsigned rows_t>
            struct set_uniform<category_sampler, data_t, cols_t, rows_t> {
                static_assert(cols_t*rows_t == 1, "samplers must be of size 1");
                
                static void set(UniformLocation location, GLuint x) {
                    ::glUniform1ui(location.value, x);
                }
            };

			///////////////////////////////////////////////////////////////////
			// basic data type traits
			///////////////////////////////////////////////////////////////////
			template <
				gl::DataType id_t //enum for basic opengl data types
			>
			struct element;
		
			///////////////////////////////////////////////////////////////////
			// shader variable traits
			///////////////////////////////////////////////////////////////////
			template <
				gl::VarType	id_t,					//enum for opengl shader variable type
				GLboolean	transpose_t = GL_FALSE	//transpose values?
			>
			struct variable;
		
			///////////////////////////////////////////////////////////////////
			// vertex array traits
			///////////////////////////////////////////////////////////////////
			template <
				gl::DataType		id_t,								//element data type id
				gl::AttributeSize	elements_t		= gl::ATTR_SIZE_4,	//number of elements
				unsigned			stride_t		= 0,				//stride between values
				GLboolean			normalized_t	= GL_FALSE			//normalize values?
			>
			struct vertex_array {
				static_assert(
					(elements_t > 0 && elements_t <= 4) || elements_t == GL_BGRA,
					"elements must be [0, 4] or GL_BGRA"
				);

				static_assert(
					id_t == GL_BYTE			|| id_t == GL_UNSIGNED_BYTE			||
					id_t == GL_SHORT		|| id_t == GL_UNSIGNED_SHORT		||
					id_t == GL_INT			|| id_t == GL_UNSIGNED_INT			||
					id_t == GL_HALF_FLOAT	|| id_t == GL_FLOAT					||
					id_t == GL_DOUBLE		|| id_t == GL_INT_2_10_10_10_REV	||
					id_t == GL_UNSIGNED_INT_2_10_10_10_REV,
					"invalid data type"
				);

				typedef element<id_t> element_t;

				static gl::DataType const	type		= id_t;			//opengl data type id
				static AttributeSize const	size		= elements_t;	//number of components
				static unsigned const		stride		= stride_t;		//stride between values
				static GLboolean const		normalized	= normalized_t;	//normalize values?

				static void attributePointer(AttributeLocation location, GLvoid const* offset = nullptr) {
					detail::vertexAttribPointer(location, size, type, normalized, stride, offset);
				}
			};

			///////////////////////////////////////////////////////////////////
			// buffer object traits
			///////////////////////////////////////////////////////////////////
			template <
				gl::BufferTarget	target_t,	//buffer target type
				gl::BufferUsage		usage_t		//buffer usage type
			>
			struct buffer {
				static gl::BufferTarget const	target	= target_t;
				static gl::BufferUsage const	usage	= usage_t;
			};

			///////////////////////////////////////////////////////////////////
			// element<> specializations
			///////////////////////////////////////////////////////////////////
			
			//opengl float
			template <> struct element<gl::DATA_TYPE_FLOAT> {
				typedef GLfloat type;
			
				static gl::DataType const	type_id		= gl::DATA_TYPE_FLOAT;
				static bool const			integral	= false;
			};
		
			///////////////////////////////////////////////////////////////////
			// variable<> specializations
			///////////////////////////////////////////////////////////////////
		
			// float
			template <GLboolean transpose_t> struct variable<gl::VAR_FLOAT, transpose_t> {
				typedef element<gl::DATA_TYPE_FLOAT> element_t;

				static gl::VarType const	type_id		= gl::VAR_FLOAT;
				static unsigned const		rows		= 1;
				static unsigned const		cols		= 1;
				static unsigned const		elements	= rows*cols;
				static GLboolean const		transpose	= transpose_t;

				typedef element_t::type math_t;
			};

			// vec3f
			template <GLboolean transpose_t> struct variable<gl::VAR_FLOAT_VEC3, transpose_t> {
				typedef element<gl::DATA_TYPE_FLOAT> element_t;

				static gl::VarType const	type_id		= gl::VAR_FLOAT_VEC3;
				static unsigned const		rows		= 3;
				static unsigned const		cols		= 1;
				static unsigned const		elements	= rows*cols;
				static GLboolean const		transpose	= transpose_t;

				typedef ::Eigen::Vector3f math_t;
			};
			typedef variable<gl::VAR_FLOAT_VEC3, GL_FALSE> vec3f;

			// mat4
			template <GLboolean transpose_t> struct variable<gl::VAR_FLOAT_MAT44, transpose_t> {
				typedef element<gl::DATA_TYPE_FLOAT> element_t;

				static gl::VarType const	type_id		= gl::VAR_FLOAT_MAT44;
				static unsigned const		rows		= 4;
				static unsigned const		cols		= 4;
				static unsigned const		elements	= rows*cols;
				static GLboolean const		transpose	= transpose_t;

				//typedef ::Eigen::Affine3f math_t;
                typedef ::Eigen::Matrix4f math_t;
			};
			typedef variable<gl::VAR_FLOAT_MAT44, GL_FALSE> mat4;

			///////////////////////////////////////////////////////////////////
			// opengl uniform traits
			///////////////////////////////////////////////////////////////////
			struct uniform {
				typedef UniformLocation	location_t;
			
				static bool const isUniform		= true;
				static bool const isAttribute	= false;

				static detail::variable_info getInfo(ProgramId program, GLuint index, GLsizei bufSize) {
					return detail::getActiveUniform(program, index, bufSize);
				}
				
				static unsigned activeCount(ProgramId program) {
					return detail::get::program::activeUniforms(program);
				}

				static unsigned activeMaxLength(ProgramId program) {
					return detail::get::program::activeUniformsMaxLength(program);
				}

				static UniformLocation getLocation(ProgramId program, String const& name) {
					return detail::getUniformLocation(program, name);
				}

				static void getData(ProgramId program, UniformLocation location, GLfloat* data) {
					detail::getUniform(program, location, data);
				}
			
				static void getData(ProgramId program, UniformLocation location, GLint* data) {
					detail::getUniform(program, location, data);
				}

				template <typename traits>
				static void setData(UniformLocation location, typename traits::element_t::type const* data, unsigned size = 1) {
					detail::setUniform<
						traits::rows,
						traits::cols,
						traits::element_t::type
					>(location, data, size, traits::transpose);
				}
			};
		
			///////////////////////////////////////////////////////////////////
			// opengl attribute traits
			///////////////////////////////////////////////////////////////////
			struct attribute {
				typedef AttributeLocation location_t;
			
				static bool const isUniform		= false;
				static bool const isAttribute	= true;
			
				static AttributeLocation getLocation(ProgramId program, String const& name) {
					return detail::getAttribLocation(program, name);
				}

				static detail::variable_info getInfo(ProgramId program, GLuint index, GLsizei bufSize) {
					return detail::getActiveAttrib(program, index, bufSize);
				}

				static unsigned activeCount(ProgramId program) {
					return detail::get::program::activeAttribs(program);
				}

				static unsigned activeMaxLength(ProgramId program) {
					return detail::get::program::activeAttribsMaxLength(program);
				}

			};
		} // namespace traits
	} // namespace gl
} // namespace vox

#endif //BKENTEL_VOX_GL_TRAITS_HPP