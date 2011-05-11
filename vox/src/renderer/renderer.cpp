#include "common.hpp"
#include "renderer.hpp"

#include "../gl/vgl.hpp"
#include "../util/util.hpp"

namespace vgl = ::vox::gl;

Eigen::Matrix4f
perspectiveMatrix(
	GLfloat left,	GLfloat right,
	GLfloat top,	GLfloat bottom,
	GLfloat nearz,	GLfloat farz
) {
	GLfloat const A =  (right + left)    / (right - left);
	GLfloat const B =  (top + bottom)    / (top - bottom);
	GLfloat const C = -(farz + nearz)    / (farz - nearz);
	GLfloat const D = -(2.0f*farz*nearz) / (farz - nearz);

	GLfloat const matrix[] = {
		2.0f * nearz / (right - left),	0.0f,							A,		0.0f,	
		0.0f,							2.0f * nearz / (top - bottom),	B,		0.0f,	
		0.0f,							0.0f,							C,		D,
		0.0f,							0.0f,							-1.0f,	0.0f,
	};

	Eigen::Matrix4f result;
	::memcpy(result.data(), matrix, sizeof(matrix));
	result.matrix().transposeInPlace();

	return result;
}

Eigen::Matrix4f
orthoMatrix(
	GLfloat left,  GLfloat right,
	GLfloat top,   GLfloat bottom,
	GLfloat nearz, GLfloat farz
) {
	GLfloat const tx = -((right + left) / (right - left));
	GLfloat const ty = -((top + bottom) / (top - bottom));
	GLfloat const tz = -((farz + nearz) / (farz - nearz));

	GLfloat const matrix[] = {
		2.0f / (right - left),	0.0f,					0.0f,					tx,	
		0.0f,					2.0f / (top - bottom),	0.0f,					ty,	
		0.0f,					0.0f,					-2.0f / (farz - nearz),	tz,
		0.0f,					0.0f,					0.0f,					1.0f,
	};

	Eigen::Matrix4f result;
	::memcpy(result.data(), matrix, sizeof(matrix));
	result.matrix().transposeInPlace();

	return result;
}

class Cube {
public:
    void bufferData(vgl::Program& program) {
        GLfloat const posData[] = {
	        // front
	        1.0, 1.0, 0.0,
	        0.0, 1.0, 0.0,
	        1.0, 0.0, 0.0,
	        0.0, 0.0, 0.0,
	        // back
	        0.0, 1.0, 1.0,
	        1.0, 1.0, 1.0,
	        0.0, 0.0, 1.0,
	        1.0, 0.0, 1.0,
	        // right
	        1.0, 1.0, 1.0,
	        1.0, 1.0, 0.0,
	        1.0, 0.0, 1.0,
	        1.0, 0.0, 0.0,
	        // left
	        0.0, 1.0, 0.0,
	        0.0, 1.0, 1.0,
	        0.0, 0.0, 0.0,
	        0.0, 0.0, 1.0,
	        // top
	        1.0, 1.0, 1.0,
	        0.0, 1.0, 1.0,
	        1.0, 1.0, 0.0,
	        0.0, 1.0, 0.0,
	        // bottom
	        0.0, 0.0, 1.0,
	        1.0, 0.0, 1.0,
	        0.0, 0.0, 0.0,
	        1.0, 0.0, 0.0,
        };

        GLfloat const colorData[] = {
	        1.0, 0.0, 0.0, 1.0,
	        1.0, 0.0, 0.0, 1.0,
	        1.0, 0.0, 0.0, 1.0,
	        1.0, 0.0, 0.0, 1.0,

	        0.0, 1.0, 0.0, 1.0,
	        0.0, 1.0, 0.0, 1.0,
	        0.0, 1.0, 0.0, 1.0,
	        0.0, 1.0, 0.0, 1.0,

	        0.0, 0.0, 1.0, 1.0,
	        0.0, 0.0, 1.0, 1.0,
	        0.0, 0.0, 1.0, 1.0,
	        0.0, 0.0, 1.0, 1.0,

	        1.0, 1.0, 1.0, 1.0,
	        1.0, 1.0, 1.0, 1.0,
	        1.0, 1.0, 1.0, 1.0,
	        1.0, 1.0, 1.0, 1.0,

	        1.0, 0.0, 1.0, 1.0,
	        1.0, 0.0, 1.0, 1.0,
	        1.0, 0.0, 1.0, 1.0,
	        1.0, 0.0, 1.0, 1.0,

	        1.0, 1.0, 0.0, 1.0,
	        1.0, 1.0, 0.0, 1.0,
	        1.0, 1.0, 0.0, 1.0,
	        1.0, 1.0, 0.0, 1.0,
        };

        array_.bind();

        positionBuffer_.bind();
        positionBuffer_.allocateAndSet(sizeof(posData), posData);

        auto pos = program.variable<vgl::attribute::vec3f>("in_Position");
        array_.setAttributePtr(pos);
        array_.enableAttribute(pos);

        colorBuffer_.bind();
        colorBuffer_.allocateAndSet(sizeof(colorData), colorData);

        auto col = program.variable<vgl::attribute::vec3f>("in_Color");
        array_.setAttributePtr(col);
        array_.enableAttribute(col);
    }

    void draw() {
        array_.bind();
        
        array_.draw(vgl::DRAW_MODE_TRIANGLE_STRIP, 4, 0);
        array_.draw(vgl::DRAW_MODE_TRIANGLE_STRIP, 4, 4);
        array_.draw(vgl::DRAW_MODE_TRIANGLE_STRIP, 4, 8);
        array_.draw(vgl::DRAW_MODE_TRIANGLE_STRIP, 4, 12);
        array_.draw(vgl::DRAW_MODE_TRIANGLE_STRIP, 4, 16);
        array_.draw(vgl::DRAW_MODE_TRIANGLE_STRIP, 4, 20);
    }
private:
    vgl::SimpleVertexArray                      array_;
    vgl::Buffer<vgl::BUFFER_USAGE_DYNAMIC_DRAW> positionBuffer_;
    vgl::Buffer<vgl::BUFFER_USAGE_DYNAMIC_DRAW> colorBuffer_;
};


class Scene {
public:
    void bufferData(vgl::Program& program)
    {
        array_.bind();
    
        GLfloat const posData[] = {
            0.0, 1.0, 0.0,
            1.0, 1.0, 0.0,
            0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
        };

        positionBuffer_.bind();
        positionBuffer_.allocateAndSet(sizeof(posData), posData);
    
        auto pos = program.variable<vgl::attribute::vec3f>("in_Position");
        array_.setAttributePtr(pos);
        array_.enableAttribute(pos);

        GLfloat const colData[] = {
            0.0, 1.0, 0.0, 1.0,
            1.0, 1.0, 0.0, 1.0,
            0.0, 0.0, 0.0, 1.0,
            1.0, 0.0, 0.0, 1.0,
        };

        colorBuffer_.bind();
        colorBuffer_.allocateAndSet(sizeof(colData), colData);
    
        auto col = program.variable<vgl::attribute::vec3f>("in_Color");
        array_.setAttributePtr(col);
        array_.enableAttribute(col);
    }

    void prepareScene(vgl::Program& program)
    {
        using namespace vgl;

        auto mv = program.variable<uniform::mat4f>("mModelView");
    
        uniform::mat4f::math_t mvMatrix;
        mvMatrix.setIdentity();
        mv.set(mvMatrix);

        auto proj = program.variable<uniform::mat4f>("mProjection");
        uniform::mat4f::math_t projMatrix = orthoMatrix(-1.0, 1.0, -1.0, 1.0, -10.0, 10.0);
        proj.set(projMatrix);
    }

    void drawScene()
    {
        array_.bind();
        array_.draw(vgl::DRAW_MODE_TRIANGLE_STRIP, 4);
    }    
private:
    vgl::SimpleVertexArray                      array_;
    vgl::Buffer<vgl::BUFFER_USAGE_DYNAMIC_DRAW> positionBuffer_;
    vgl::Buffer<vgl::BUFFER_USAGE_DYNAMIC_DRAW> colorBuffer_;
};

////////////////////////////////////////////////////////////////////////////////

void
vox::RenderTask::start() {
    boost::unique_lock<boost::mutex> lock(mutex_);
        
    switch (state_) {
    case STATE_STARTING :
    case STATE_STARTED :
        return;
    case STATE_PAUSED :
        return;
    case STATE_STOPPING :
        return;
    case STATE_STOPPED :
        assert(!thread_);

        state_ = STATE_STARTING;
        stateCondition_.notify_all();

        thread_.reset(new boost::thread(&RenderTask::main_, this));
            
        while (state_ != STATE_STARTED) {
            stateCondition_.wait(lock);
        }
            
        return;
    default :
        assert(0); //TODO
    };

}
    
void
vox::RenderTask::stop() {
    boost::unique_lock<boost::mutex> lock(mutex_);
        
    state_ = STATE_STOPPING;
    stateCondition_.notify_all();

    while (state_ != STATE_STOPPED) {
        stateCondition_.wait(lock);
    }

    thread_->join();
}
    
void
vox::RenderTask::state()
{
}

vox::RenderTask::RenderTask(std::shared_ptr<RenderWindow> window)
    : window_(window)
    , glProgram_()
    , state_(STATE_STOPPED)
    , thread_()
{
}

void
vox::RenderTask::setViewport_(unsigned width, unsigned height) {
    GLfloat const aspect = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);

    ::glViewport(0, 0, width, height);

    projOrtho_ = orthoMatrix(0, width, 0, height, -10.0, 10.0);
    projPersp_ = perspectiveMatrix(-1.0*aspect, 1.0*aspect, -1.0, 1.0, 1.0, 1000.0);
}

void
vox::RenderTask::initProgram_()
{
    glProgram_.reset(
        new gl::Program()
    );
    
    gl::Program& program = *glProgram_;
    
    program.attachShader(
        std::shared_ptr<gl::Shader>(
            new gl::Shader(L"./data/shader.frag", gl::SHADER_TYPE_FRAGMENT)
        )
    );
    
    program.attachShader(
        std::shared_ptr<gl::Shader>(
            new gl::Shader(L"./data/shader.vert", gl::SHADER_TYPE_VERTEX)
        )
    );

    program.link();
    program.use();

    projMatrix_ = program.variable<gl::uniform::mat4f>("mProjection");
    mvMatrix_ = program.variable<gl::uniform::mat4f>("mModelView");

    gl::mat4 mat = program.getVariable<gl::mat4>("mProjection");
    gl::sampler2D tex = program.getVariable<gl::sampler2D>("texture");

	glClearColor(0.0, 0.5, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void
vox::RenderTask::main_() {
    //Set the state to STARTED
    {//lock
        boost::lock_guard<boost::mutex> lock(mutex_);
            
        state_ = STATE_STARTED;
        stateCondition_.notify_all();
    }//unlock

    auto context = window_->acquireGl();
    
    //This thread needs to be the one to clean up opengl -- do it after main_() ends
    util::on_scope_exit exit_f([this]() -> void {
        boost::lock_guard<boost::mutex> lock(mutex_);
            
        glProgram_.release();
        state_ = STATE_STOPPED;
        stateCondition_.notify_all();
    });
        
    //Setup opengl shaders, variables, etc
    initProgram_();

    Scene testScene;
    testScene.prepareScene(*glProgram_);
    testScene.bufferData(*glProgram_);

    Cube cube;
    cube.bufferData(*glProgram_);

    while (state_ == STATE_STARTED) {
        while (!tasks_.isEmpty()) {
            tasks_.dequeue()();
        }
        
        ::glClear( GL_COLOR_BUFFER_BIT   |
                   GL_DEPTH_BUFFER_BIT   |
                   GL_STENCIL_BUFFER_BIT
        );

       
 	    mvMatrix_.set(
		    Eigen::Affine3f(Eigen::Translation3f(1.0, 1.0, -5.0)).matrix()
	    );

        projMatrix_.set(projPersp_);      
        cube.draw();

 	    mvMatrix_.set(
		    (Eigen::Translation3f(10.0f, 10.0f, 0.0f)*
             Eigen::Scaling(100.0f, 100.0f, 1.0f)).matrix()
	    );

        projMatrix_.set(projOrtho_);
        testScene.drawScene();

        window_->swap();
    }
}

/*
void
vox::Renderer::main_()
{
    //make gl active for this thread
    auto glContext = window_->acquireGl();
  
    std::shared_ptr<gl::Shader> fragment(
        new gl::Shader(L"./data/shader.frag", gl::SHADER_TYPE_FRAGMENT)
    );
    std::shared_ptr<gl::Shader> vertex(
        new gl::Shader(L"./data/shader.vert", gl::SHADER_TYPE_VERTEX)
    );

    gl::Program program;

    program.attachShader(fragment);
    program.attachShader(vertex);

    program.link();
    program.use();

    Scene testScene;
    
    testScene.prepareScene(program);
    testScene.bufferData(program);

    while (state_ == STATE_RUNNING) {
        try {
            //do any pending work
            while (!workQueue_.isEmpty()) {
                workQueue_.dequeue()();
            }

            //render
            ::glClear(
                GL_COLOR_BUFFER_BIT   |
                GL_DEPTH_BUFFER_BIT   |
                GL_STENCIL_BUFFER_BIT
            );
            
            testScene.drawScene();
                
            window_->swap();
        } catch (vox::exception& e) {
        }
    }

    {   //lock
        boost::unique_lock<boost::mutex> lock(mutex_);
        
        state_ = STATE_STOPPED;
        stopping_.notify_all();
    }   //unlock
}

bool
vox::Renderer::onClose_()
{
    stop();

    thread_->join();

    return true;
}

bool
vox::Renderer::onResize_(unsigned width, unsigned height)
{
    workQueue_.enqueue([this, width, height] {
        ::glViewport(0, 0, width, height);
    });
    
    return true;
}

void
vox::Renderer::stop()
{
    boost::unique_lock<boost::mutex> lock(mutex_);

    state_ = STATE_STOPPING;
    while (state_ != STATE_STOPPED) {
        stopping_.wait(lock);
    }
}

vox::Renderer::Renderer()
    : window_()
    , thread_()
    , state_(STATE_STOPPED)
{
    window_.reset(
        new system::NativeWindow(1024, 768) //TODO remove hard coded values
    );

    window_->setOnClose([this] { return this->onClose_(); });
    window_->setOnResize([this] (unsigned x, unsigned y) { return this->onResize_(x, y); });
}

void
vox::Renderer::start()
{
    if (thread_) {
        return;
    }
   
    thread_.reset(
        new boost::thread([this] { this->main_(); })
    );

    state_ = STATE_RUNNING;

    while (state_ != STATE_STOPPED) {
        window_->doEventsWait();
    }
}

*/