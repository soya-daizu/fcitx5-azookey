/*
 * SPDX-FileCopyrightText: 2021~2021 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "azookey.h"
#include "BridgingHeader.h"

// AzookeyState

AzookeyState::AzookeyState(AzookeyEngine *engine, fcitx::InputContext *ic)
    : engine_(engine), ic_(ic), composing_text_(ak_composing_text_new()) {
  FCITX_DEBUG() << "init state";
}

void AzookeyState::keyEvent(fcitx::KeyEvent &keyEvent) {
  FCITX_DEBUG() << "keyEvent: " << keyEvent.key();

  bool isComposing = ak_composing_text_is_composing(composing_text_);
  if (isComposing) {
    if (keyEvent.key().check(FcitxKey_BackSpace)) {
      ak_composing_text_delete_backward(composing_text_);
      updateUI();
      return keyEvent.filterAndAccept();
    }
    if (keyEvent.key().check(FcitxKey_Return)) {
      const char *convertTarget =
          ak_composing_text_get_convert_target(composing_text_);
      ic_->commitString(convertTarget);
      free((void *)convertTarget);

      reset();
      return keyEvent.filterAndAccept();
    }
    if (keyEvent.key().check(FcitxKey_Escape)) {
      reset();
      return keyEvent.filterAndAccept();
    }
  } else {
    if (!keyEvent.key().isSimple()) {
      return;
    }
  }

  ak_composing_text_insert(composing_text_, keyEvent.key().toString().c_str());
  updateUI();
  return keyEvent.filterAndAccept();
}

void AzookeyState::updateUI() {
  auto &inputPanel = ic_->inputPanel();
  inputPanel.reset();

  const char *convertTarget =
      ak_composing_text_get_convert_target(composing_text_);
  int cursorPosition = ak_composing_text_get_cursor_position(composing_text_);
  if (convertTarget) {
    fcitx::Text text;
    text.append(convertTarget, fcitx::TextFormatFlag::Underline);

    int bytePosition = getBytePosition(convertTarget, cursorPosition);
    text.setCursor(bytePosition);

    free((void *)convertTarget);
    inputPanel.setPreedit(text);
  }

  ic_->updateUserInterface(fcitx::UserInterfaceComponent::InputPanel);
  ic_->updatePreedit();
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
  ak_composing_text_reset(composing_text_);
  updateUI();
}

// AzookeyEngine

AzookeyEngine::AzookeyEngine(fcitx::Instance *instance)
    : instance_(instance), factory_([this](fcitx::InputContext &ic) {
        return new AzookeyState(this, &ic);
      }),
      kana_kanji_converter_(ak_kana_kanji_converter_new()) {
  FCITX_DEBUG() << "init engine";
  instance->inputContextManager().registerProperty("azookeyState", &factory_);
}

void AzookeyEngine::keyEvent(const fcitx::InputMethodEntry &entry,
                             fcitx::KeyEvent &keyEvent) {
  FCITX_UNUSED(entry);
  if (keyEvent.isRelease() || keyEvent.key().states()) {
    return;
  }

  auto ic = keyEvent.inputContext();
  auto *state = ic->propertyFor(&factory_);
  state->keyEvent(keyEvent);
}

void AzookeyEngine::reset(const fcitx::InputMethodEntry &,
                          fcitx::InputContextEvent &event) {
  auto *state = event.inputContext()->propertyFor(&factory_);
  state->reset();
}

FCITX_ADDON_FACTORY(AzookeyEngineFactory);
