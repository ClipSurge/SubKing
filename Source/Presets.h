#pragma once
#include "Params.h"
#include <vector>
#include <utility>
namespace sk {
struct Preset { const char* name; std::vector<std::pair<int, float>> v; };
inline const std::vector<Preset>& presets() {
  static const std::vector<Preset> k = {
   {"Sub Foundation", { {P_O1LVL,0},{P_O2LVL,0},{P_SUBLVL,1},{P_CUTOFF,20000},{P_RES,0},{P_FDRIVE,0},{P_FENV,0},{P_DMIX,0.15f},{P_DDRIVE,0.1f},{P_AD,500},{P_AS,1} }},
   {"808 Boom", { {P_O1LVL,0},{P_O2LVL,0},{P_SUBLVL,1},{P_GLIDE,0},{P_LEGATO,0},{P_CUTOFF,20000},{P_RES,0},{P_FENV,0},{P_KTRACK,0},
                  {P_PENV,26},{P_PDEC,45},{P_AA,0.5f},{P_AD,1100},{P_AS,0},{P_AR,250},{P_DDRIVE,0.35f},{P_DMIX,0.4f},{P_DTONE,2500} }},
   {"Reese Monster", { {P_O1WAVE,0},{P_O2WAVE,0},{P_O1LVL,0.9f},{P_O2LVL,0.9f},{P_O2DET,22},{P_SUBLVL,0.55f},{P_CUTOFF,520},{P_RES,0.25f},{P_FDRIVE,0.4f},
                  {P_FENV,0.1f},{P_FS,0.5f},{P_LFORATE,0.35f},{P_LFOCUT,0.18f},{P_DDRIVE,0.45f},{P_DMIX,0.5f},{P_DTONE,5500} }},
   {"Acid Squelch", { {P_O1WAVE,0},{P_O2LVL,0},{P_SUBLVL,0},{P_GLIDE,70},{P_CUTOFF,330},{P_RES,0.78f},{P_FDRIVE,0.45f},{P_FENV,0.75f},{P_KTRACK,0.5f},
                  {P_FD,190},{P_FS,0},{P_AD,250},{P_AS,0.8f},{P_AR,60},{P_DDRIVE,0.35f},{P_DMIX,0.35f} }},
   {"Dub Wobble 1/8", { {P_O1WAVE,0},{P_O2WAVE,1},{P_O2OCT,-1},{P_O2DET,10},{P_SUBLVL,0.7f},{P_CUTOFF,260},{P_RES,0.55f},{P_FENV,0.1f},
                  {P_LFOSHAPE,0},{P_LFOSYNC,4},{P_LFOCUT,0.8f},{P_LFORETRIG,1},{P_DDRIVE,0.3f},{P_DMIX,0.35f} }},
   {"Growler", { {P_O1WAVE,1},{P_O2WAVE,1},{P_O2DET,35},{P_O1LVL,0.8f},{P_O2LVL,0.8f},{P_SUBLVL,0.5f},{P_CUTOFF,700},{P_RES,0.4f},{P_FENV,0.3f},
                  {P_LFOSHAPE,1},{P_LFORATE,5.5f},{P_LFOCUT,0.35f},{P_LFOPITCH,12},{P_DDRIVE,0.8f},{P_DMIX,0.8f},{P_DTONE,4500} }},
   {"Neuro 1/16", { {P_O1WAVE,0},{P_O2WAVE,0},{P_O2DET,14},{P_O2LVL,0.9f},{P_SUBLVL,0.4f},{P_CUTOFF,420},{P_RES,0.6f},{P_FDRIVE,0.5f},
                  {P_LFOSHAPE,2},{P_LFOSYNC,6},{P_LFOCUT,0.55f},{P_LFORETRIG,1},{P_DDRIVE,0.75f},{P_DMIX,0.7f},{P_DTONE,6000} }},
   {"Pluck Bass", { {P_O1WAVE,0},{P_O2WAVE,1},{P_O2LVL,0.4f},{P_SUBLVL,0.6f},{P_GLIDE,0},{P_LEGATO,0},{P_CUTOFF,220},{P_RES,0.3f},{P_FENV,0.85f},
                  {P_FD,140},{P_FS,0.05f},{P_AD,420},{P_AS,0.25f},{P_AR,90},{P_DDRIVE,0.2f},{P_DMIX,0.25f} }},
   {"Rubber Funk", { {P_O1WAVE,1},{P_O2WAVE,0},{P_O2LVL,0.5f},{P_O2OCT,0},{P_SUBLVL,0.5f},{P_GLIDE,0},{P_LEGATO,0},{P_CUTOFF,380},{P_RES,0.5f},
                  {P_FENV,0.65f},{P_FD,120},{P_FS,0.2f},{P_KTRACK,0.5f},{P_AD,260},{P_AS,0.7f},{P_AR,70},{P_DDRIVE,0.25f},{P_DMIX,0.3f} }},
   {"Warm Moog", { {P_O1WAVE,0},{P_O2WAVE,2},{P_O2DET,6},{P_O2LVL,0.7f},{P_SUBLVL,0.6f},{P_CUTOFF,600},{P_RES,0.35f},{P_FDRIVE,0.35f},{P_FENV,0.35f},
                  {P_FD,350},{P_FS,0.35f},{P_GLIDE,45},{P_DDRIVE,0.15f},{P_DMIX,0.2f} }},
  };
  return k;
}
}
