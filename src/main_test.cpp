#include "Includes.h"
#include "SFML/OpenGL.hpp"
#include "SFML/Window.hpp"
#include "SFML\System.hpp"
#include "glad/glad.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

int main(int argc, char *argv[]) {
    sf::Context context(sf::ContextSettings(24, 8, 4, 4, 6, sf::ContextSettings::Core), 800, 600);

    // load the extension functions since sfml doesn't do it for you
    if (!gladLoadGL()) {
        cout << "Unable to load opengl" << endl;
        exit(-1);
    }

    // deactiavate the context since we only needed it to laod opengl functions
    context.setActive(false);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}