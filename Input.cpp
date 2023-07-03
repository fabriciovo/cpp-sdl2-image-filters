//
//  Input.cpp
//  cpp-image-filters
//
//  Created by fabricio varisco oliveira on 22/06/23.
//

#include "Input.h"

Input::Input(){
    std::cout << "Enter the path to the image file: " << std::endl;
    std::cin >> imagePath;
    std::cin.ignore();
    std::cout << "Want a background? (y/n)" << std::endl;
    std::cin >> hasBackground;
    std::cin.ignore();
    
    while (hasBackground != "y" && hasBackground != "n") {
        std::cout << "Invalid input. Please enter 'y' or 'n':" << std::endl;
        std::cin >> hasBackground;
        std::cin.ignore();
    }

    if (hasBackground == "y") {
        std::cout << "Enter the path to the background file:" << std::endl;
        std::cin >> backgroundPath;
    }
}


std::string Input::getImagePath(){
    return imagePath;
}
std::string Input::getHasBackground(){
    return hasBackground;
}
std::string Input::getBackgroundPath(){
    return backgroundPath;
}
std::string Input::getBackgroundColor(){
    return backgroundColor;
}


