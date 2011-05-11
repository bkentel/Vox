#pragma once
#ifndef VOX_RENDERER_RENDERER_HPP
#define VOX_RENDERER_RENDERER_HPP

#include <boost/utility.hpp>
#include <boost/thread.hpp>

#include "../system/window/NativeWindow.hpp"
#include "../util/blockingQueue.hpp"
#include "../gl/vgl.hpp"

namespace vox {

class RenderWindow : private boost::noncopyable {
public:
    RenderWindow(unsigned width, unsigned height)
        : window_(width, height)
        {
        }

    void close();
    void resize(unsigned width, unsigned height);

    void doEvents() { window_.doEventsWait(); }

    bool isClosed() { return false; //TODO
    }

    void swap() {
        window_.swap();
    }

    vox::system::OpenGlContext acquireGl() {
        return window_.acquireGl();
    }

    void setOnResize(std::function<bool (unsigned width, unsigned height)> callback) {
        window_.setOnResize(callback);
    }

    void setOnClose(std::function<bool ()> callback) {
        window_.setOnClose(callback);
    }
private:
    

    vox::system::NativeWindow window_;
};

class RenderTask : private boost::noncopyable {
public:
    enum State {
        STATE_STARTING,
        STATE_STARTED,
        STATE_PAUSED,
        STATE_STOPPING,
        STATE_STOPPED,
    };

    void start();
    void stop();  
    void state();
public:
    explicit RenderTask(std::shared_ptr<RenderWindow> window);

    void setViewport(unsigned width, unsigned height) {
        tasks_.enqueue(
            [this, width, height] { setViewport_(width, height); }
        );
    }
private:
    void setViewport_(unsigned width, unsigned height);

    void main_();
    void initProgram_();

    std::shared_ptr<RenderWindow> window_;
    
    std::unique_ptr<gl::Program>  glProgram_;

    gl::uniform::mat4f projMatrix_;
    gl::uniform::mat4f mvMatrix_;
    
    Eigen::Matrix4f projOrtho_;
    Eigen::Matrix4f projPersp_;

    Eigen::Matrix4f matProj_;
    Eigen::Matrix4f matMv_;

    State                          state_;
    std::unique_ptr<boost::thread> thread_;
    boost::mutex                   mutex_;
    boost::condition_variable      stateCondition_;

    vox::util::BlockingQueue<std::function<void ()>> tasks_;
};


} //namespace vox

#endif //VOX_RENDERER_RENDERER_HPP
