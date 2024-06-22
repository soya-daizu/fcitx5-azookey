#pragma once
#ifndef _FCITX5_AZOOKEY_AZOOKEYENGINE_H_
#define _FCITX5_AZOOKEY_AZOOKEYENGINE_H_

#include "AzookeyState.h"
#include "BridgingHeader.h"
#include <fcitx/addonfactory.h>
#include <fcitx/addonmanager.h>
#include <fcitx/event.h>
#include <fcitx/inputmethodengine.h>
#include <fcitx/instance.h>

class AzookeyEngine : public fcitx::InputMethodEngineV2 {
public:
  AzookeyEngine(fcitx::Instance *instance);
  ~AzookeyEngine() { ak_kana_kanji_converter_dispose(kanaKanjiConverter_); }

  void keyEvent(const fcitx::InputMethodEntry &entry,
                fcitx::KeyEvent &keyEvent) override;
  void reset(const fcitx::InputMethodEntry &,
             fcitx::InputContextEvent &event) override;

  auto instance() const { return instance_; }
  auto factory() const { return &factory_; }
  auto kanaKanjiConverter() const { return kanaKanjiConverter_; }

  void freeCandidateResult(ConversionResult *result);

private:
  fcitx::Instance *instance_;
  fcitx::FactoryFor<AzookeyState> factory_;
  void *kanaKanjiConverter_;
};

class AzookeyEngineFactory : public fcitx::AddonFactory {
  fcitx::AddonInstance *create(fcitx::AddonManager *manager) override {
    FCITX_UNUSED(manager);
    return new AzookeyEngine(manager->instance());
  }
};

#endif
