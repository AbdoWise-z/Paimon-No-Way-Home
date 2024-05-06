
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

#include <iostream>
#include <fstream>
#include <flags/flags.h>
#include <json/json.hpp>

#include <application.hpp>

#define EXTERN
#include "globals.h"
#include "states/level-menu-state.h"

#include "states/play-state.hpp"
#include "states/main-menu-state.h"
#include "states/splash-screen-state.hpp"

int main(int argc, char** argv) {

    // quick definitions
    our::SUPPRESS_SHADER_ERRORS = false;

    
    flags::args args(argc, argv); // Parse the command line arguments
    // config_path is the path to the json file containing the application configuration
    // Default: "config/app.json"
    auto config_path = args.get<std::string>("c", "config/app.jsonc");
    // run_for_frames is how many frames to run the application before automatically closing
    // This is useful for testing multiple configurations in a batch
    // Default: 0 where the application runs indefinitely until manually closed
    int run_for_frames = args.get<int>("f", 0);
    // Open the config file and exit if failed
    std::ifstream file_in(config_path);
    if(!file_in){
        std::cerr << "Couldn't open file: " << config_path << std::endl;
        return -1;
    }
    // Read the file into a json object then close the file
    nlohmann::json app_config = nlohmann::json::parse(file_in, nullptr, true, true);
    file_in.close();

    // Create the application
    our::Application app(app_config);
    
    // Register all the states of the project in the application
    app.registerState<Playstate>("play");
    app.registerState<MainMenuState>("main-menu");
    app.registerState<SplashScreenState>("splash");
    app.registerState<LevelMenuState>("level-menu");

    our::level_path = "config/levels/level-4.jsonc";

    // Then choose the state to run based on the option "start-scene" in the config
    if(app_config.contains(std::string{"start-scene"})){
        app.changeState(app_config["start-scene"].get<std::string>());
    }

    // Finally run the application
    // Here, the application loop will run till the terminatio condition is statisfied
    return app.run(run_for_frames);
}