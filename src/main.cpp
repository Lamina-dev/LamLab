#include "LaminaIDE.h"
#include <iostream>
#include <exception>

int main()
{
    try {
        LaminaIDE ide;
        
        if (!ide.Initialize()) {
            std::cerr << "Failed to initialize LaminaLab" << std::endl;
            return -1;
        }
        
        std::cout << "LaminaLab started successfully" << std::endl;
        
        ide.Run();
        
        std::cout << "LaminaLab shutting down..." << std::endl;
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "LaminaLab crashed with exception: " << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        std::cerr << "LaminaLab crashed with unknown exception" << std::endl;
        return -1;
    }
}
