/**
 * main.cpp - Test Game Application Entry Point
 * 
 * Simple test application that demonstrates engine initialization,
 * basic game loop, scene management, and component usage.
 * 
 * @version 1.0.0
 * @date 2026-03-04
 */

#include "TestGame.hpp"
#include <iostream>
#include <exception>

/**
 * @brief Application entry point
 * @param argc Number of command line arguments
 * @param argv Command line argument array
 * @return Exit code (0 = success, non-zero = failure)
 */
int main(int argc, char* argv[]) {
    // Print startup banner
    std::cout << "===========================================\n";
    std::cout << "  AIEngine Test Game Application v1.0.0\n";
    std::cout << "===========================================\n";
    std::cout << "Educational C++ Game Engine Bootstrap\n";
    std::cout << "Built with: SDL2 + OpenGL 3.3+ + GLM\n";
    std::cout << "===========================================\n\n";

    try {
        // Create and run test game application
        TestGame game;
        
        std::cout << "Initializing test game...\n";
        
        // Initialize the game (sets up engine, creates window, etc.)
        if (!game.Initialize()) {
            std::cerr << "ERROR: Failed to initialize test game!\n";
            std::cerr << "Common causes:\n";
            std::cerr << "- SDL2 not installed or configured\n";
            std::cerr << "- OpenGL drivers too old (need 3.3+)\n";
            std::cerr << "- Display/graphics issues\n";
            return -1;
        }
        
        std::cout << "Test game initialized successfully!\n";
        std::cout << "Starting game loop...\n";
        std::cout << "Press ESC or close window to exit.\n\n";
        
        // Run the main game loop
        int exitCode = game.Run();
        
        std::cout << "\nGame loop finished. Shutting down...\n";
        
        // Cleanup (destructor handles engine shutdown)
        std::cout << "Cleanup completed successfully.\n";
        
        if (exitCode == 0) {
            std::cout << "Test game exited normally.\n";
        } else {
            std::cout << "Test game exited with code: " << exitCode << "\n";
        }
        
        return exitCode;
        
    } catch (const std::exception& e) {
        // Handle any unexpected exceptions
        std::cerr << "FATAL ERROR: Unhandled exception caught!\n";
        std::cerr << "Exception: " << e.what() << "\n";
        std::cerr << "The application will now terminate.\n";
        return -2;
        
    } catch (...) {
        // Handle unknown exceptions
        std::cerr << "FATAL ERROR: Unknown exception caught!\n";
        std::cerr << "The application will now terminate.\n";
        return -3;
    }
}

// Educational notes:
// - Proper error handling with try-catch for robustness
// - Clear user feedback during startup and shutdown phases
// - Exit codes that indicate different types of failures
// - Resource management through RAII (automatic cleanup via destructors)