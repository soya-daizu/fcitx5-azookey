#pragma once
#ifndef _FCITX5_AZOOKEY_AZOOKEYCANDIDATELIST_H_
#define _FCITX5_AZOOKEY_AZOOKEYCANDIDATELIST_H_

#include <fcitx/candidatelist.h>
#include <fcitx/inputcontext.h>

class AzookeyEngine;

class AzookeyCandidateWord : public fcitx::CandidateWord {
public:
  AzookeyCandidateWord(AzookeyEngine *engine, fcitx::Text text, int idx);
  void select(fcitx::InputContext *inputContext) const override {}

private:
  AzookeyEngine *engine_;
  int idx;
};

class AzookeyCandidateList : public fcitx::CommonCandidateList {
public:
  AzookeyCandidateList(AzookeyEngine *engine, fcitx::InputContext *ic,
                       void *composingText);

  void generateRealtimeCandidates();
  void generateFullLengthCandidates();

  fcitx::CandidateLayoutHint layoutHint() const override {
    return fcitx::CandidateLayoutHint::Vertical;
  }

private:
  AzookeyEngine *engine_;
  fcitx::InputContext *ic_;
  void *composingText_;
};

#endif // _FCITX5_AZOOKEY_AZOOKEYCANDIDATELIST_H_
