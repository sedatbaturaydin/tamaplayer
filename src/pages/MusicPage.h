#pragma once

enum MusicView {
  MUSIC_VIEW_LIST,
  MUSIC_VIEW_PLAYING
};

void musicPageSetView(MusicView v);
MusicView musicPageGetView();

void drawMusicPage();          // tam ekran
void drawMusicPagePartial();   // partial redraw (progress, state)
