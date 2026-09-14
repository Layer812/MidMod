#ifndef SMF_PROBE_H
#define SMF_PROBE_H

#include <stdint.h>
#include <SdFat.h>

struct SmfProbeInfo {
  bool valid;
  uint16_t format;
  uint16_t tracks;
  uint16_t tpqn;
  uint16_t initial_bpm;
  uint32_t total_ms;
};

bool smfProbeFile(SdFat &sd, const char *path, SmfProbeInfo &out);

#endif
