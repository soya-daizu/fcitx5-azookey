/*
 * SPDX-FileCopyrightText: 2021~2021 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _FCITX5_AZOOKEY_AZOOKEY_H_
#define _FCITX5_AZOOKEY_AZOOKEY_H_

#include "BridgingHeader.h"
#include <cstddef>
#include <fcitx-utils/inputbuffer.h>
#include <fcitx/addonfactory.h>
#include <fcitx/addonmanager.h>
#include <fcitx/inputcontext.h>
#include <fcitx/inputcontextproperty.h>
#include <fcitx/inputmethodengine.h>
#include <fcitx/inputpanel.h>
#include <fcitx/instance.h>

class AzookeyEngine;

class AzookeyState : public fcitx::InputContextProperty {
public:
  AzookeyState(AzookeyEngine *engine, fcitx::InputContext *ic);
  ~AzookeyState() { ak_composing_text_dispose(composing_text_); }

  void keyEvent(fcitx::KeyEvent &keyEvent);
  void updateUI();
  size_t getBytePosition(const std::string &utf8Str, size_t charPosition);
  void reset();

private:
  AzookeyEngine *engine_;
  fcitx::InputContext *ic_;
  void *composing_text_;
};

class AzookeyEngine : public fcitx::InputMethodEngineV2 {
public:
  AzookeyEngine(fcitx::Instance *instance);
  ~AzookeyEngine() { ak_kana_kanji_converter_dispose(kana_kanji_converter_); }

  void keyEvent(const fcitx::InputMethodEntry &entry,
                fcitx::KeyEvent &keyEvent) override;
  void reset(const fcitx::InputMethodEntry &,
             fcitx::InputContextEvent &event) override;

  auto factory() const { return &factory_; }
  auto instance() const { return instance_; }

private:
  fcitx::Instance *instance_;
  fcitx::FactoryFor<AzookeyState> factory_;
  void *kana_kanji_converter_;
};

class AzookeyEngineFactory : public fcitx::AddonFactory {
  fcitx::AddonInstance *create(fcitx::AddonManager *manager) override {
    FCITX_UNUSED(manager);
    return new AzookeyEngine(manager->instance());
  }
};

#endif // _FCITX5_AZOOKEY_AZOOKEY_H_
