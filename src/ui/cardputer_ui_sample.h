#ifndef CARDPUTER_UI_SAMPLE_H
#define CARDPUTER_UI_SAMPLE_H

#include <stdint.h>
#include <stddef.h>
#include <midi_playback_api.h>

struct UiFileRow {
  const char *name;
  uint8_t kind; /* 0=up dir, 1=dir, 2=midi */
};

struct UiBrowserModel {
  const char *profile;
  const char *folder;
  const UiFileRow *rows;
  size_t row_count;
  size_t selected;
  uint8_t volume;
  bool loop;
  uint32_t selected_duration_ms;
  uint8_t selected_tracks;
};

void cardputerUiBegin();
void cardputerUiDrawPlayback(const mplay_snapshot_t &s);
void cardputerUiDrawBrowser(const UiBrowserModel &m);
void cardputerUiDrawHelp(bool playback, const char *profile, uint8_t volume, bool loop);
void cardputerUiDrawMessage(const char *title, const char *message);

#endif
