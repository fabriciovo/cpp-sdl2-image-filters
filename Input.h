//
//  Input.hpp
//  cpp-image-filters
//
//  Created by fabricio varisco oliveira on 22/06/23.
//

#ifndef Input_hpp
#define Input_hpp

#include <iostream>
#include <string>

class Input {
private:
    std::string imagePath = "";
    std::string backgroundPath = "";
    std::string hasBackground = "";
    std::string backgroundColor = "";

public:
    Input();
    
    std::string getImagePath();
    std::string getBackgroundPath();
    std::string getHasBackground();
    std::string getBackgroundColor();

};

#endif /* Input_hpp */
