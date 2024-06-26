#pragma once
#ifndef _FCITX5_AZOOKEY_AZOOKEYSTATE_H_
#define _FCITX5_AZOOKEY_AZOOKEYSTATE_H_

#include "AzookeyCandidateList.h"
#include "BridgingHeader.h"
#include <fcitx/inputcontext.h>
#include <fcitx/inputcontextproperty.h>

class AzookeyEngine;

class AzookeyState : public fcitx::InputContextProperty {
  enum class InputState {
    None,
    Composing,
    FullLengthSelecting,
    ClausePreviewing,
    ClauseSelecting
  };

public:
  AzookeyState(AzookeyEngine *engine, fcitx::InputContext *ic);
  ~AzookeyState() { ak_composing_text_dispose(composingText_); }

  void keyEvent(fcitx::KeyEvent &keyEvent);
  void updateCandidates();
  void updateUI();
  void reset();

private:
  AzookeyEngine *engine_;
  fcitx::InputContext *ic_;
  void *composingText_;

  size_t getCharCount(const std::string &utf8Str);
  size_t getBytePosition(const std::string &utf8Str, size_t charPosition);

  void setInputState(InputState inputState) {
    lastInputState_ = inputState_;
    inputState_ = inputState;
  }
  InputState inputState_ = InputState::None;
  InputState lastInputState_ = InputState::None;

  std::unique_ptr<AzookeyCandidateList> tempCandidateList_;
};

#endif // _FCITX5_AZOOKEY_AZOOKEYSTATE_H_
