#pragma once

void drawMusicPage();          // tam ekran (parça değişiminde)
void drawMusicPagePartial();   // scroll + bar + süre + durum
void _drawBarAndStatus();      // bar + süre + durum (iç paylaşım)
void musicPageResetScroll();   // parça değişiminde scroll'u sıfırla
bool musicPageScrollTick();    // her loop frame'inde çağrılır, true = redraw gerekli
