/*
 * SPDX-FileCopyrightText: 2021~2021 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "azookey.h"
#include "BridgingHeader.h"

AzookeyEngine::AzookeyEngine(fcitx::Instance *instance) {
  init_swift_azookey();
  FCITX_INFO() << "init";
}

void AzookeyEngine::keyEvent(const fcitx::InputMethodEntry &entry,
                             fcitx::KeyEvent &keyEvent) {
  FCITX_UNUSED(entry);
  FCITX_INFO() << keyEvent.key() << " isRelease=" << keyEvent.isRelease();
}

FCITX_ADDON_FACTORY(AzookeyEngineFactory);
