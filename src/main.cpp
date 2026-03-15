#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include <stdio.h>

#include "physics/VehicleBody.hpp"
#include "physics/VehicleConstants.hpp"
#include "core/Engine.hpp"
#include "render/Renderer.hpp"
#include "game/Track.hpp"
#include "game/Car.hpp"
#include "game/RaceManager.hpp"

using namespace F1Game::Physics;

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    // Phase5 Full demo: Monaco + race + renderer + controller
    F1Game::Driver driver{"Player", 1, 1.0f, 1.0f, 1.0f};
    F1Game::CarConfig config{"Test F1", "SF-26", 2026, 850.0f, 1.0f, 360.0f};

    F1Game::Car playerCar;
    playerCar.Setup(driver, config);

    F1Game::TrackData monacoData = F1Game::CreateMonacoTrack();
    F1Game::Track track;
    track.Load(monacoData);

    F1Game::RaceManager race;
    race.SetupRace(F1Game::RaceType::TimeTrial, track, 1);
    race.AddCar(&playerCar);
    race.StartRace();

    F1Game::Render::Renderer renderer;

    F1Game::Engine engine;
    engine.SetRenderer(&renderer);
    engine.SetRaceManager(&race);
    engine.SetTrack(&track);
    engine.SetPlayerCar(&playerCar);
    engine.Init(screenWidth, screenHeight, "F1 Racing Phase5 Demo - Monaco");

    renderer.Init(screenWidth, screenHeight);
    renderer.SetTargetCar(&playerCar);
    renderer.SetTrack(&track);

    engine.Run();

    return 0;
}
