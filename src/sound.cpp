
#include "sound.hpp"


#include <SDL_mixer.h>

#include <stdexcept>


Sound::Sound()
{
  int flags = 0;
  int initted = Mix_Init(flags);
  if (initted < 0)
  {
    throw std::runtime_error("SDL_mixer did not init properly?");
  }


  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
  {
    throw std::runtime_error("Mix_OpenAudio failed");
  }

  LoadSoundLibrary();

  Mix_AllocateChannels(16);

}


Mix_Chunk * Sound::LoadWav(const std::string &filename)
{
  Mix_Chunk *sample = Mix_LoadWAV(filename.c_str());

  if (!sample)
  {
    throw std::runtime_error("Mix_LoadWAV failed");
  }

  return sample;
}


void Sound::LoadSoundLibrary()
{
  for(const std::string path : {"../data/"})
  {
    try
    {
      sounds["bounce"] = LoadWav(path + "bounce.wav");
      sounds["bounce_hit"] = LoadWav(path + "bounce_hit.wav");
      sounds["paddle_bounce"] = LoadWav(path + "paddle_bounce.wav");

      break;
    }
    catch (...)
    {
      continue;
    }
  }

}

#include <iostream>

void Sound::PlaySound(const std::string &what)
{
  int chan = Mix_PlayChannel(-1, sounds.at(what), 0);

  if (chan == -1)
  {
    std::cout << "PlaySound(" << what << ") returned " << chan
      << "  [" << Mix_GetError() << "]" << std::endl;
  }
}



void Sound::Quit()
{
  Mix_HaltChannel(-1);

  Mix_CloseAudio();
  //This is what the docs say to shutdown...
  while(Mix_Init(0)) Mix_Quit();
}
