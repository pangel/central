#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "main.h"
#include "test/test.h"
#include <unistd.h>
#include <boost/asio.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "interfaceinit/interface_init.h"
#include "interfaceinit/interface_init2.h"
#include "generation/geography.h"
#include "generation/generation1.h"
#include "network/network.h"
#include "network/netEvent.h"
#include "network/dummyServer.h"
#include "network/dummyClient.h"
#include <time.h>
#include <thread>
#include "graphism/tilemap.h"
#include "graphism/graphic_context_iso.h"
#include <unistd.h>
#include <stdio.h>
#include "hud/hudMayor.h"
#include "hud/hudTerro.h"
#include "game_textures.h"
#include "HScenario.h"
#include <TGUI/TGUI.hpp>
#include <string>
#include <random>
#include "Musique.h"
#include <vector>
#include "GrilleHarmonique.h"
#include "ToMIDI.h"
#include <fluidsynth.h>

#include "localState.h"
#include "globalState.h"

bool TERRO = true;
bool AUTOPLAY = false;
bool CLIENT = false;
std::string SEED = "424242";
#define DEBUG true
#include "debug.h"

#ifdef __APPLE__
#define CLOSE_KEYS ((event.key.code == sf::Keyboard::Q) && event.key.system)
#define AUDIO_DRIVER "coreaudio"
#else
#define CLOSE_KEYS ((event.key.code == sf::Keyboard::F4) && event.key.alt)
#define AUDIO_DRIVER "alsa"
#endif
void clientLoop(int id, int nbPlayers, bool isFullScreen,
    sf::VideoMode video_mode, Client* clientPtr, std::string seed, 
    sf::RenderWindow* window) {

  Geography geo = Generation1(seed);
  DBG<<(geo.getWalkableTile()->getCoord().getAbs());
  LocalState loc = LocalState(&geo, nbPlayers, 1);
  HScenario scenar = HScenario(loc);
  loc.setScenario(&scenar);
  loc.setClient(clientPtr);
  GraphicContextIso graContIso = GraphicContextIso(&geo, &loc);
  TileMap tilemap = TileMap(&loc, &geo);

  //geo.printMatrix();
  if (TERRO) {

  } else {
    /* MAYOR */
  }

  sf::Clock clock;
  sf::Time dt = sf::Time::Zero;
  HudMayor hudMayor = HudMayor(window,loc); //One needs to be removed
  HudTerro hudTerro = HudTerro(window, loc, graContIso);
  //hudTerro.init();
  while ((*window).isOpen()) {
    dt = clock.restart();
    if (TERRO) {
      hudTerro.init();
    }
    else {
      hudMayor.init(loc);
    }
    sf::Event event;
    while ((*window).pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        (*window).close();
      }
      if (event.type == sf::Event::KeyPressed) {

        if (event.key.code == sf::Keyboard::Tab) {
          if (!TERRO) {
            tilemap.changeView();
          }
        }
        if (event.key.code == sf::Keyboard::F11) {
          if (isFullScreen) {
            (*window).create(video_mode, "Game Interface");
            isFullScreen = false;
          } else {
            (*window).create(video_mode, "Game Interface",
                sf::Style::Fullscreen);
            isFullScreen = true;
          }
        }
        if (CLOSE_KEYS) {
          (*window).close();
        }

      }
      if (TERRO) {hudTerro.event(window, event, graContIso);}
      else {hudMayor.event(window,event,&tilemap,&loc);}
    }

    loc.run(dt);
    window->clear();
    if (TERRO) {
      graContIso.run(window);
      (*window).setView((*window).getDefaultView());
      hudTerro.draw();
    } else { // MAYOR
      tilemap.run(window);
      hudMayor.draw();
    }
    (*window).display();
  }

  return;
}

void serverLoop(int id, int nbPlayers, Server* serverPtr, std::string seed,
    sf::RenderWindow* window) {

  Geography geo = Generation1(seed);
  GlobalState glob = GlobalState(&geo, nbPlayers, id);
  Tile* firstTile = geo.getWalkableTile();
  glob.addPlayer(Player(1, (firstTile->getCoord()).getAbs(),
      (firstTile->getCoord()).getOrd()));
  glob.getPlayerByID(1).isServer = 1;
  glob.setServer(serverPtr);

  
    // graContIso.load();
    
    //[joseph] ceci est un NPC de test
    //on en génère 500 à la création de la map, puis plus après
    //(pour l'instant, après la classe simulation les fera apparaître et disparaître)
  std::default_random_engine npcGen(42);
  dummy::createNPCs(500, glob, geo, npcGen);


  sf::Clock clock;
  sf::Time dt = sf::Time::Zero;
  while (window->isOpen()) {
    dt = clock.restart();
    //        sf::Event event;
    glob.run(dt);
  }

  return;
}



/**
 * The sound !
 * @author: Adrien K.
 */
 int musicloop(bool* switchmusic) {    
    
    srand( time(NULL)); // obligatoire pour initialisation de l'aleatoire
    
    Musique musique;
    std::vector<std::list<MIDIEvent>> MIDImusic(16);  
      fluid_settings_t* settings;
      fluid_synth_t* synth;
      fluid_audio_driver_t* adriver;
      int sfont_id;
      
      /* Create the settings. */
      settings = new_fluid_settings();
      fluid_settings_setstr(settings, "audio.driver", AUDIO_DRIVER);
      fluid_settings_setnum(settings,"synth.gain",0.4);

      /* Change the settings if necessary*/

      /* Create the synthesizer. */
      synth = new_fluid_synth(settings);

      /* Create the audio driver. The synthesizer starts playing as soon
	 as the driver is created. */
      adriver = new_fluid_audio_driver(settings, synth);

      /* Load a SoundFont and reset presets (so that new instruments
       * get used from the SoundFont) */
      /* "../../../Musique/FluidR3_GM2-2.SF2" */
      std::cout << "Chargement de SoundFont " 
	   << (sfont_id = fluid_synth_sfload(synth, "../../../Musique/GS_MuseScore_v1.442.sf2", 1)) 
	   << " "
	   << FLUID_WARN ;
      setNextMesures(musique,MIDImusic,synth,false);

      /*
      for (auto MIDIchannel : MIDImusic){
	DBG << "New channel";
	for (MIDIEvent event : MIDIchannel){
	  DBG << event.MIDInumber << " " << event.duration << " " << event.begintime << "  |  ";
	}
      }
      */

      bool oldvalue = not (*switchmusic);
      //fluid_synth_program_change (synth, 0, 33);//Acoustic Bass
      //fluid_synth_program_change (synth, 1, 36);//Fretless Bass
      //fluid_synth_program_change (synth, 2, 65);//Soprano Sax

      while(true){
	setNextMesures(musique,MIDImusic,synth,false);
	float time = 0.;
  usleep(100000);
	sf::Clock clock;

	if (*switchmusic != oldvalue){
	  oldvalue = *switchmusic;
	  if (*switchmusic){
	    fluid_synth_program_change (synth, 0, 33);//Acoustic Bass
	    fluid_synth_program_change (synth, 1, 36);//Fretless Bass
	    fluid_synth_program_change (synth, 2, 65);//Soprano Sax
	  }
	  else{      
	    fluid_synth_program_change (synth, 0, 17);//Drawbar Organ
	    fluid_synth_program_change (synth, 1, 113);//Tinkle Bell
	    fluid_synth_program_change (synth, 2, 80);//Ocarina
	  }
	}
	
	while(time < MIDImusic[0].back().begintime + MIDImusic[0].back().duration){
	  time += clock.restart().asSeconds();
	  int cpt = 0;
	  for (auto MIDIchannel = MIDImusic.begin(); MIDIchannel != MIDImusic.end(); MIDIchannel++){
	    for (auto event = MIDIchannel->begin(); event != MIDIchannel->end(); event++){
	      //cout << event->begintime + event->duration << " " << time << " " << event->begintime << " " << event->isplayed << "\n";
	      if ((event->begintime + event->duration > time)&&(event->begintime < time)&&(event->isplayed == false)){
		fluid_synth_noteon(synth, cpt, event->MIDInumber, 80);
		event->isplayed = true;
		//DBG << "play a note " << event->MIDInumber << " channel " << cpt << " time " << time ;
	      }
	      if((event->begintime + event->duration < time)&&(event->isplayed == true)){
		fluid_synth_noteoff(synth, cpt, event->MIDInumber);
		event->isplayed = false;
		//DBG << "stop playing a note " << event->MIDInumber << " channel " << cpt << " time " << time ;
	      }
	    }
	    cpt++;
	  }
	}
      }
      /* Clean up */
      delete_fluid_audio_driver(adriver);
      delete_fluid_synth(synth);
      delete_fluid_settings(settings);
      return 0;
 }


int main(int argc, char ** argv) {
  textures::initialize();

  if (argc > 2 && (std::string) argv[1] == "test") {
    test::run(argv[2]);
    exit(0);
  }

  std::string cur;
  for (int i=1;i<argc;i++) {
    cur = (std::string) argv[i];
    if (cur == "auto") AUTOPLAY = true; // Skip interface_initiale (default: no)
    if (cur == "mayor") TERRO = false; // Show mayor view (default: no)
    if (cur == "terro") TERRO = true; // Show terro view (default: yes)
    if (cur == "client") CLIENT = true; // Client only (default:  no)
    if (cur == "seed" && argc > i+1) SEED = (std::string)argv[++i]; // Seed setting (default: "424242"). Consumes next argument.
  }

  int sizeFenetre[3], b;
  bool isFullScreen;
  sf::VideoMode video_mode;
  /*  if (DEBUG) {
   b = 1;
   isFullScreen = true;
   video_mode = sf::VideoMode::getDesktopMode();
   } else {*/
  ServerInfo s_info;
  ClientInfo c_info;
  Server* serverPtr = NULL;
  if (!CLIENT)
    serverPtr = Network::createServer(s_info);
  Client* clientPtr = Network::createClient(c_info);
  if (AUTOPLAY) {
    b = 1;
    video_mode = sf::VideoMode::getDesktopMode();
    isFullScreen = true;
  } else {
    b = interface_initiale2(sizeFenetre, &isFullScreen, serverPtr, clientPtr);
    video_mode = sf::VideoMode(sizeFenetre[0], sizeFenetre[1], sizeFenetre[2]);
  }
  //}

  if (b == 0) {
    return 0;
  } else {
    sf::RenderWindow window;
    if (isFullScreen) {
      window.create(video_mode, "Game Interface", sf::Style::Fullscreen);
    } else
      window.create(video_mode, "Game Interface");
    window.setKeyRepeatEnabled(false);
    int nbPlayers = 1;
    std::string seed = SEED;
    DBG << "Seed: " << seed;

    //geo.printMatrix();

    //Change the value of the boolean to change the music
    bool switchmusic = false;
    std::thread musicThread { std::bind(musicloop, &switchmusic) };


    std::thread serverThread { std::bind(serverLoop, 0, nbPlayers, serverPtr,
        seed, &window) };

    clientLoop(1, nbPlayers, isFullScreen, video_mode, clientPtr, seed, &window);
    serverThread.join();

    return 1;
  };

  // ISO Map, not used for now
  //Defintion of the window
  /*	if (b == 0) {
   return 0;
   } else {
   sf::VideoMode video_mode = sf::VideoMode(sizeFenetre[0],
   sizeFenetre[1], sizeFenetre[2]);
   sf::RenderWindow window;

   if (isFullScreen) {
   window.create(video_mode, "Game", sf::Style::Fullscreen);
   } else {
   window.create(video_mode, "Game");
   }

   sf::Vector2u size = window.getSize();
   unsigned int w = size.x;
   unsigned int h = size.y;
   //End Defintion of the window

   // Loading Textures

   Geography geo = (Geography) Generation1("424242"); // Il faudra un jour qu'on m'explique ce que dois faire main, parce que là c'est n'importe quoi ~ MrKulu

   sf::Texture a1, b1, b2;
   //A priori le working directory est src/interfaceinit, alors il faut remonter loin ...
   printcwd();
   assert(!a1.loadFromFile("../../../sprite/Anim.png"));
   assert(!b1.loadFromFile("../../../sprite/Road_NS.png"));
   assert(!b2.loadFromFile("../../../sprite/maison1.png"));

   // Definition of the Context Iso

   GraphicContextIso gci(&geo);

   TexturePack tp1;
   tp1.texture = a1;
   tp1.nbAnim = {1,12,16};
   tp1.widthSprite = {16,16,32};
   tp1.heightSprite = 32;
   tp1.offsetX = {0,0,0};
   tp1.offsetY = {8,8,8};
   tp1.isLoop = {true, true, false};
   SpriteTilePack stp1 = {.texture = b1 , .originX = 0 , .originY = 39 , .X1 = 0 , .Y1 = 0, .X2 = 157 , .Y2 = 79 }, stp2 = {.texture = b2 , .originX = 0 , .originY = 109 , .X1 = 0, .Y1 = 0, .X2 = 157 , .Y2 = 149 };

   gci.addTexturePack(tp1);
   gci.addSpriteTilePack(stp1);
   gci.addSpriteTilePack(stp2);

   //inputState s = { true, window };

   // Chargement de la vue

   sf::View isoView;
   isoView.setSize(sf::Vector2f(size.x,size.y)); // A placer dans la boucle pricipale si on veut avoir une fenêtre de taille variable pendant le jeu.
   isoView.setCenter(OFFSET_X+40,OFFSET_Y+20); // Pour tester

   window.setView(isoView); // Pour tester

   while (window.isOpen()) {
   sf::Event event;
   while (window.pollEvent(event)) {

   }

   window.clear();
   window.draw(gci); // Dessine la map
   window.display();
   }
   window.close();
   return 0;
   //window.close();
   //return 0;
   }*/
}
;



namespace dummy {
  void createNPCs(int number, Simulation& simu,
      Geography& geo, std::default_random_engine npcGen) {
    std::uniform_real_distribution<float>
        npcDistX(0.01, geo.getMapWidth() - 0.01);
    std::uniform_real_distribution<float> npcDistY(0.01, geo.getMapHeight()
        - 0.01);
    for (int i = 0; i < number; i++) {
      Position start = Position(npcDistX(npcGen), npcDistY(npcGen));
      Position target = Position(npcDistX(npcGen), npcDistY(npcGen));
      while (start.isInTile(geo).getSpeed() == 0) {
        start = Position(npcDistX(npcGen), npcDistY(npcGen));
      }
      while (target.isInTile(geo).getSpeed() == 0 || target.isInTile(geo).equals(
          start.isInTile(geo))) {
        target = Position(npcDistX(npcGen), npcDistY(npcGen));
      }
      simu.addNPC(start, target, 1, textures::get(i % 2));

      //simu.addNPC(Position(8.5,0.5),Position(8.5,25.5),1,&tp1);

      //simu.addNPC(Position(8.5,25.5),Position(8.5,0.5),1,&tp1);
    }
  }
}
