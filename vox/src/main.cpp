#include "common.hpp"
#include "renderer/renderer.hpp"

int
wmain(int argc, wchar_t* argv[], wchar_t* envp[])
try {
    std::shared_ptr<vox::RenderWindow> window(
        new vox::RenderWindow(1024, 768)
    );
    
    vox::RenderTask renderer(window);

    window->setOnResize([&renderer](unsigned width, unsigned height) -> bool {
        renderer.setViewport(width, height);
        return true;
    });

    bool finished = false;
    window->setOnClose([&renderer, &finished]() -> bool {
        renderer.stop();
        finished = true;
        return true;
    });

    renderer.start();

    while (!finished) {
        window->doEvents();
    }

	return 0;
} catch (vox::exception& e) {
    std::cout << diagnostic_information(e);
	return -1;
} catch (...) {
	return -1;
}
