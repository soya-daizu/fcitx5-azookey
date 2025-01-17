#include "AzookeyState.h"
#include "AzookeyCandidateList.h"
#include "AzookeyEngine.h"
#include "BridgingHeader.h"
#include "KeyAction.h"
#include <fcitx-utils/capabilityflags.h>
#include <fcitx-utils/textformatflags.h>
#include <fcitx/inputpanel.h>
#include <fcitx/text.h>

// AzookeyState

AzookeyState::AzookeyState(AzookeyEngine *engine, fcitx::InputContext *ic)
    : engine_(engine), ic_(ic), composingText_(ak_composing_text_new()) {
  FCITX_DEBUG() << "init state";
}

void AzookeyState::keyEvent(fcitx::KeyEvent &keyEvent) {
  FCITX_DEBUG() << "keyEvent: " << keyEvent.key();

  KeyAction action = KeyAction(keyEvent.key());
  if (action.isUnknown()) {
    FCITX_DEBUG() << "unknown keyEvent: " << keyEvent.key();
    return;
  }

  if (inputState_ == InputState::None) {
    if (action.isInput()) {
      ak_composing_text_insert(composingText_, action.surface_.c_str());
      setInputState(InputState::Composing);
      updateUI();
      return keyEvent.filterAndAccept();
    }
  }

  if (inputState_ == InputState::Composing) {
    if (action.isInput()) {
      ak_composing_text_insert(composingText_, action.surface_.c_str());
      setInputState(InputState::Composing);
      updateCandidates();
      updateUI();
      return keyEvent.filterAndAccept();
    }
    if (action.isDelete()) {
      auto direction = action.deleteDirection();
      switch (direction) {
      case KeyAction::DeleteDirection::Backward:
        ak_composing_text_delete_backward(composingText_);
        break;
      case KeyAction::DeleteDirection::Forward:
        ak_composing_text_delete_forward(composingText_);
        break;
      default:
        return keyEvent.filterAndAccept();
      }

      bool isNotEmpty = ak_composing_text_is_composing(composingText_);
      if (isNotEmpty) {
        setInputState(InputState::Composing);
        updateCandidates();
        updateUI();
      } else {
        reset();
      }

      return keyEvent.filterAndAccept();
    }
    if (action.isNavigation()) {
      auto direction = action.navigationDirection();
      switch (direction) {
      case KeyAction::NavigationDirection::Left:
        ak_composing_text_move_cursor(composingText_, -1);
        break;
      case KeyAction::NavigationDirection::Right:
        ak_composing_text_move_cursor(composingText_, 1);
        break;
      case KeyAction::NavigationDirection::Home:
        ak_composing_text_set_cursor(composingText_, 0);
        break;
      case KeyAction::NavigationDirection::End:
        ak_composing_text_set_cursor(composingText_, -1);
        break;
      default:
        return keyEvent.filterAndAccept();
      }
      setInputState(InputState::Composing);
      updateUI();
      return keyEvent.filterAndAccept();
    }
    if (action.isTab()) {
      if (action.tabDirection() == KeyAction::TabDirection::Forward) {
        setInputState(InputState::FullLengthSelecting);
        updateCandidates();
        updateUI();
      }
      return keyEvent.filterAndAccept();
    }
    if (action.isSpace()) {
      setInputState(InputState::ClausePreviewing);
      updateCandidates();
      updateUI();
      return keyEvent.filterAndAccept();
    }
    if (action.isEnter()) {
      fcitx::Text preeditText;
      if (ic_->capabilityFlags().test(fcitx::CapabilityFlag::Preedit))
        preeditText = ic_->inputPanel().clientPreedit();
      else
        preeditText = ic_->inputPanel().preedit();
      ic_->commitString(preeditText.toStringForCommit());
      reset();
      return keyEvent.filterAndAccept();
    }
    if (action.isEscape()) {
      reset();
      return keyEvent.filterAndAccept();
    }
  }

  if (inputState_ == InputState::FullLengthSelecting) {
    if (action.isInput()) {
      ak_composing_text_insert(composingText_, action.surface_.c_str());
      setInputState(InputState::Composing);
      updateUI();
      return keyEvent.filterAndAccept();
    }
    if (action.isDelete() || action.isEscape()) {
      setInputState(InputState::Composing);
      updateUI();
      return keyEvent.filterAndAccept();
    }
    if (action.isNavigation()) {
      auto direction = action.navigationDirection();
      auto candidateList = std::dynamic_pointer_cast<AzookeyCandidateList>(
          ic_->inputPanel().candidateList());
      switch (direction) {
      case KeyAction::NavigationDirection::Up:
        candidateList->prevCandidate();
        break;
      case KeyAction::NavigationDirection::Down:
        candidateList->nextCandidate();
        break;
      case KeyAction::NavigationDirection::Left:
      case KeyAction::NavigationDirection::PageUp:
        candidateList->prev();
        break;
      case KeyAction::NavigationDirection::Right:
      case KeyAction::NavigationDirection::PageDown:
        candidateList->next();
        break;
      default:
        return keyEvent.filterAndAccept();
      }

      setInputState(InputState::FullLengthSelecting);
      updateUI();
      return keyEvent.filterAndAccept();
    }
    if (action.isTab()) {
      auto direction = action.tabDirection();
      auto candidateList = std::dynamic_pointer_cast<AzookeyCandidateList>(
          ic_->inputPanel().candidateList());
      switch (direction) {
      case KeyAction::TabDirection::Backward:
        candidateList->prevCandidate();
        break;
      case KeyAction::TabDirection::Forward:
        candidateList->nextCandidate();
        break;
      default:
        return keyEvent.filterAndAccept();
      }

      setInputState(InputState::FullLengthSelecting);
      updateUI();
      return keyEvent.filterAndAccept();
    }
    if (action.isSpace()) {
      auto candidateList = std::dynamic_pointer_cast<AzookeyCandidateList>(
          ic_->inputPanel().candidateList());
      candidateList->nextCandidate();
      setInputState(InputState::FullLengthSelecting);
      updateUI();
      return keyEvent.filterAndAccept();
    }
    if (action.isEnter()) {
      fcitx::Text preeditText;
      if (ic_->capabilityFlags().test(fcitx::CapabilityFlag::Preedit))
        preeditText = ic_->inputPanel().clientPreedit();
      else
        preeditText = ic_->inputPanel().preedit();
      ic_->commitString(preeditText.toStringForCommit());
      reset();
      return keyEvent.filterAndAccept();
    }
  }

  if (inputState_ == InputState::ClausePreviewing) {
    if (action.isNavigation()) {
      auto direction = action.navigationDirection();
      auto candidateList = tempCandidateList_.get();
      switch (direction) {
      case KeyAction::NavigationDirection::ShiftLeft:
        if (candidateList->shiftSegment(false)) {
          setInputState(InputState::ClausePreviewing);
          updateCandidates();
        }
        break;
      case KeyAction::NavigationDirection::ShiftRight:
        if (candidateList->shiftSegment(true)) {
          setInputState(InputState::ClausePreviewing);
          updateCandidates();
        }
        break;
      case KeyAction::NavigationDirection::Left:
        candidateList->moveSegment(false);
        candidateList->generateSegmentCandidates();
        setInputState(InputState::ClausePreviewing);
        break;
      case KeyAction::NavigationDirection::Right:
        candidateList->moveSegment(true);
        candidateList->generateSegmentCandidates();
        setInputState(InputState::ClausePreviewing);
        break;
      default:
        return keyEvent.filterAndAccept();
      }

      updateUI();
      return keyEvent.filterAndAccept();
    }
    if (action.isSpace()) {
      auto candidateList = tempCandidateList_.get();
      candidateList->nextCandidate();
      setInputState(InputState::ClauseSelecting);
      updateUI();
      return keyEvent.filterAndAccept();
    }
    if (action.isEnter()) {
      fcitx::Text preeditText;
      if (ic_->capabilityFlags().test(fcitx::CapabilityFlag::Preedit))
        preeditText = ic_->inputPanel().clientPreedit();
      else
        preeditText = ic_->inputPanel().preedit();
      ic_->commitString(preeditText.toStringForCommit());
      reset();
      return keyEvent.filterAndAccept();
    }
    if (action.isEscape()) {
      setInputState(InputState::Composing);
      updateUI();
      return keyEvent.filterAndAccept();
    }
  }

  if (inputState_ == InputState::ClauseSelecting) {
    if (action.isNavigation()) {
      auto direction = action.navigationDirection();
      auto candidateList = std::dynamic_pointer_cast<AzookeyCandidateList>(
          ic_->inputPanel().candidateList());
      switch (direction) {
      case KeyAction::NavigationDirection::Up:
        candidateList->prevCandidate();
        setInputState(InputState::ClauseSelecting);
        break;
      case KeyAction::NavigationDirection::Down:
        candidateList->nextCandidate();
        setInputState(InputState::ClauseSelecting);
        break;
      case KeyAction::NavigationDirection::ShiftLeft:
        if (candidateList->shiftSegment(false)) {
          updateCandidates();
          setInputState(InputState::ClausePreviewing);
        }
        break;
      case KeyAction::NavigationDirection::ShiftRight:
        if (candidateList->shiftSegment(true)) {
          updateCandidates();
          setInputState(InputState::ClausePreviewing);
        }
        break;
      case KeyAction::NavigationDirection::Left:
        candidateList->moveSegment(false);
        candidateList->generateSegmentCandidates();
        setInputState(InputState::ClausePreviewing);
        break;
      case KeyAction::NavigationDirection::Right:
        candidateList->moveSegment(true);
        candidateList->generateSegmentCandidates();
        setInputState(InputState::ClausePreviewing);
        break;
      default:
        return keyEvent.filterAndAccept();
      }

      updateUI();
      return keyEvent.filterAndAccept();
    }
    if (action.isSpace()) {
      auto candidateList = std::dynamic_pointer_cast<AzookeyCandidateList>(
          ic_->inputPanel().candidateList());
      candidateList->nextCandidate();
      setInputState(InputState::ClauseSelecting);
      updateUI();
      return keyEvent.filterAndAccept();
    }
    if (action.isEnter()) {
      fcitx::Text preeditText;
      if (ic_->capabilityFlags().test(fcitx::CapabilityFlag::Preedit))
        preeditText = ic_->inputPanel().clientPreedit();
      else
        preeditText = ic_->inputPanel().preedit();
      ic_->commitString(preeditText.toStringForCommit());
      reset();
      return keyEvent.filterAndAccept();
    }
    if (action.isEscape()) {
      setInputState(InputState::ClausePreviewing);
      updateUI();
      return keyEvent.filterAndAccept();
    }
  }
}

void AzookeyState::updateCandidates() {
  if (inputState_ == InputState::Composing) {
    tempCandidateList_ =
        std::make_unique<AzookeyCandidateList>(engine_, ic_, composingText_);
    tempCandidateList_->generateRealtimeCandidates();
  }

  if (inputState_ == InputState::FullLengthSelecting) {
    tempCandidateList_ =
        std::make_unique<AzookeyCandidateList>(engine_, ic_, composingText_);
    tempCandidateList_->generateFullLengthCandidates();
  }

  if (inputState_ == InputState::ClausePreviewing) {
    std::vector<long> segments;
    int segmentIndex = 0;
    if (tempCandidateList_) {
      segments = tempCandidateList_->segments();
      segmentIndex = tempCandidateList_->segmentIndex();
    }

    tempCandidateList_ =
        std::make_unique<AzookeyCandidateList>(engine_, ic_, composingText_);
    tempCandidateList_->generateMultiSegmentCandidates(segments);
    tempCandidateList_->generateSegmentCandidates();
    tempCandidateList_->setSegmentIndex(segmentIndex);
  }

  if (inputState_ == InputState::ClauseSelecting) {
    auto candidateList = std::dynamic_pointer_cast<AzookeyCandidateList>(
        ic_->inputPanel().candidateList());

    std::vector<long> segments = candidateList->segments();
    int segmentIndex = candidateList->segmentIndex();

    tempCandidateList_ =
        std::make_unique<AzookeyCandidateList>(engine_, ic_, composingText_);
    tempCandidateList_->generateMultiSegmentCandidates(segments);
    tempCandidateList_->generateSegmentCandidates();
    tempCandidateList_->setSegmentIndex(segmentIndex);
  }
}

void AzookeyState::updateUI() {
  auto &inputPanel = ic_->inputPanel();
  fcitx::Text preeditText;

  if (inputState_ == InputState::None) {
    inputPanel.reset();
  }

  if (inputState_ == InputState::Composing) {
    inputPanel.reset();
    const char *convertTarget =
        ak_composing_text_get_convert_target(composingText_);
    int cursorPosition = ak_composing_text_get_cursor_position(composingText_);
    if (convertTarget) {
      preeditText.append(convertTarget, fcitx::TextFormatFlag::Underline);

      int bytePosition = getBytePosition(convertTarget, cursorPosition);
      preeditText.setCursor(bytePosition);
      free((void *)convertTarget);

      if (tempCandidateList_) {
        inputPanel.setCandidateList(std::move(tempCandidateList_));
        inputPanel.setAuxDown(fcitx::Text("[Tabキーで選択]"));
      }
    }
  }

  if (inputState_ == InputState::FullLengthSelecting) {
    inputPanel.reset();
    if (tempCandidateList_)
      inputPanel.setCandidateList(std::move(tempCandidateList_));

    auto candidateList = std::dynamic_pointer_cast<AzookeyCandidateList>(
        ic_->inputPanel().candidateList());
    preeditText.append(candidateList->currentCandidateString(),
                       fcitx::TextFormatFlag::HighLight);

    fcitx::Text auxDownText;
    auxDownText.append("[");
    auxDownText.append(std::to_string(candidateList->globalCursorIndex() + 1));
    auxDownText.append("/");
    auxDownText.append(std::to_string(candidateList->totalSize()));
    auxDownText.append("]");
    inputPanel.setAuxDown(auxDownText);
  }

  if (inputState_ == InputState::ClausePreviewing) {
    if (lastInputState_ == InputState::ClauseSelecting) {
      auto candidateList = std::dynamic_pointer_cast<AzookeyCandidateList>(
          ic_->inputPanel().candidateList());
      tempCandidateList_ =
          std::make_unique<AzookeyCandidateList>(*(candidateList.get()));
    }
    inputPanel.reset();

    if (tempCandidateList_)
      preeditText = tempCandidateList_->buildWholeText();
  }

  if (inputState_ == InputState::ClauseSelecting) {
    if (tempCandidateList_)
      inputPanel.setCandidateList(std::move(tempCandidateList_));

    auto candidateList = std::dynamic_pointer_cast<AzookeyCandidateList>(
        ic_->inputPanel().candidateList());
    preeditText = candidateList->buildWholeText();

    fcitx::Text auxDownText;
    auxDownText.append("[");
    auxDownText.append(std::to_string(candidateList->globalCursorIndex() + 1));
    auxDownText.append("/");
    auxDownText.append(std::to_string(candidateList->totalSize()));
    auxDownText.append("]");
    inputPanel.setAuxDown(auxDownText);
  }

  if (ic_->capabilityFlags().test(fcitx::CapabilityFlag::Preedit)) {
    inputPanel.setClientPreedit(preeditText);
    ic_->updatePreedit();
  } else {
    inputPanel.setPreedit(preeditText);
  }
  ic_->updatePreedit();
  ic_->updateUserInterface(fcitx::UserInterfaceComponent::InputPanel);
}

size_t AzookeyState::getBytePosition(const std::string &utf8Str,
                                     size_t charPosition) {
  size_t bytePosition = 0;
  size_t currentCharPosition = 0;

  while (bytePosition < utf8Str.size() && currentCharPosition < charPosition) {
    unsigned char ch = utf8Str[bytePosition];

    // Determine the number of bytes for the current character
    size_t charLength = 0;
    if ((ch & 0x80) == 0) { // 1-byte character (ASCII)
      charLength = 1;
    } else if ((ch & 0xE0) == 0xC0) { // 2-byte character
      charLength = 2;
    } else if ((ch & 0xF0) == 0xE0) { // 3-byte character
      charLength = 3;
    } else if ((ch & 0xF8) == 0xF0) { // 4-byte character
      charLength = 4;
    } else {
      // Invalid UTF-8 encoding
      return 0;
    }

    bytePosition += charLength;
    ++currentCharPosition;
  }

  if (currentCharPosition != charPosition) {
    // Character position is out of range
    return 0;
  }

  return bytePosition;
}

void AzookeyState::reset() {
  ak_composing_text_reset(composingText_);
  setInputState(InputState::None);
  tempCandidateList_.reset();
  updateUI();
}
