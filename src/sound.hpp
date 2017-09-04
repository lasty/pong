
#pragma once

#include <string>
#include <map>

class Sound
{
public:
  Sound();


private:

  std::map<std::string, struct Mix_Chunk *> sounds;


  struct Mix_Chunk * LoadWav(const std::string &filename);


  void LoadSoundLibrary();


public:

  void PlaySound(const std::string &what);

  void Quit();

};
