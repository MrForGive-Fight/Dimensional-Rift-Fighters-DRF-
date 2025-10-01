#include "Core/GameEngine.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    try {
        std::cout << "=== Anime Arena Fighter ===" << std::endl;
        std::cout << "Starting game engine..." << std::endl;

        ArenaFighter::GameEngine& engine = ArenaFighter::GameEngine::Instance();
        
        if (!engine.Initialize()) {
            std::cerr << "Failed to initialize game engine!" << std::endl;
            return -1;
        }

        // Main game loop
        engine.Run();

        // Cleanup
        engine.Shutdown();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred!" << std::endl;
        return -1;
    }

    return 0;
}