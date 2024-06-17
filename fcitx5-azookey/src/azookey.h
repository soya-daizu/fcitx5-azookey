/*
 * SPDX-FileCopyrightText: 2021~2021 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _FCITX5_AZOOKEY_AZOOKEY_H_
#define _FCITX5_AZOOKEY_AZOOKEY_H_

#include <fcitx-utils/inputbuffer.h>
#include <fcitx/addonfactory.h>
#include <fcitx/addonmanager.h>
#include <fcitx/inputcontext.h>
#include <fcitx/inputcontextproperty.h>
#include <fcitx/inputmethodengine.h>
#include <fcitx/inputpanel.h>
#include <fcitx/instance.h>

class AzookeyEngine;

class AzookeyEngine : public fcitx::InputMethodEngineV2 {
public:
  AzookeyEngine(fcitx::Instance *instance);

  void keyEvent(const fcitx::InputMethodEntry &entry,
                fcitx::KeyEvent &keyEvent) override;

  auto instance() const { return instance_; }

private:
  fcitx::Instance *instance_;
};

class AzookeyEngineFactory : public fcitx::AddonFactory {
  fcitx::AddonInstance *create(fcitx::AddonManager *manager) override {
    FCITX_UNUSED(manager);
    return new AzookeyEngine(manager->instance());
  }
};

#endif // _FCITX5_AZOOKEY_AZOOKEY_H_
