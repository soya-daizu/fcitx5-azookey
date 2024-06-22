#pragma once
#ifndef _FCITX5_AZOOKEY_KEYACTION_H_
#define _FCITX5_AZOOKEY_KEYACTION_H_

#include <fcitx-utils/key.h>
#include <fcitx-utils/keysym.h>
#include <fcitx-utils/keysymgen.h>
#include <fcitx-utils/log.h>

struct KeyAction {
  enum class Type {
    Input,
    Delete,
    Enter,
    Tab,
    Space,
    Escape,
    Navigation,
    Unknown
  };
  enum class TabDirection { Forward, Backward, Unknown };
  enum class DeleteDirection { Backward, Forward, Unknown };
  enum class NavigationDirection {
    Left,
    Right,
    Up,
    Down,
    Home,
    End,
    PageUp,
    PageDown,
    Unknown
  };

  bool isInput() const { return type_ == Type::Input; }
  bool isDelete() const { return type_ == Type::Delete; }
  bool isEnter() const { return type_ == Type::Enter; }
  bool isTab() const { return type_ == Type::Tab; }
  bool isSpace() const { return type_ == Type::Space; }
  bool isEscape() const { return type_ == Type::Escape; }
  bool isNavigation() const { return type_ == Type::Navigation; }
  bool isUnknown() const { return type_ == Type::Unknown; }

  TabDirection tabDirection() const {
    if (key_.check(FcitxKey_Tab))
      return TabDirection::Forward;
    if (key_.check(FcitxKey_Tab, fcitx::KeyState::Shift))
      return TabDirection::Backward;
    return TabDirection::Unknown;
  }
  DeleteDirection deleteDirection() const {
    if (key_.check(FcitxKey_BackSpace))
      return DeleteDirection::Backward;
    if (key_.check(FcitxKey_Delete))
      return DeleteDirection::Forward;
    return DeleteDirection::Unknown;
  }
  NavigationDirection navigationDirection() const {
    if (key_.check(FcitxKey_Left) || key_.check(FcitxKey_KP_Left))
      return NavigationDirection::Left;
    if (key_.check(FcitxKey_Right) || key_.check(FcitxKey_KP_Right))
      return NavigationDirection::Right;
    if (key_.check(FcitxKey_Up) || key_.check(FcitxKey_KP_Up))
      return NavigationDirection::Up;
    if (key_.check(FcitxKey_Down) || key_.check(FcitxKey_KP_Down))
      return NavigationDirection::Down;
    if (key_.check(FcitxKey_Home) || key_.check(FcitxKey_KP_Home))
      return NavigationDirection::Home;
    if (key_.check(FcitxKey_End) || key_.check(FcitxKey_KP_End))
      return NavigationDirection::End;
    if (key_.check(FcitxKey_Page_Up) || key_.check(FcitxKey_KP_Page_Up))
      return NavigationDirection::PageUp;
    if (key_.check(FcitxKey_Page_Down) || key_.check(FcitxKey_KP_Page_Down))
      return NavigationDirection::PageDown;
    return NavigationDirection::Unknown;
  }

  fcitx::Key key_;
  std::string surface_;
  Type type_;

  KeyAction(const fcitx::Key &key)
      : key_(key), surface_(fcitx::Key::keySymToUTF8(key.sym())) {
    if (key_.check(FcitxKey_Return))
      type_ = Type::Enter;
    else if (key_.check(FcitxKey_Tab) ||
             key_.check(FcitxKey_Tab, fcitx::KeyState::Shift))
      type_ = Type::Tab;
    else if (key_.check(FcitxKey_space) || key_.check(FcitxKey_KP_Space))
      type_ = Type::Space;
    else if (key_.check(FcitxKey_Escape))
      type_ = Type::Escape;
    else if (key_.check(FcitxKey_BackSpace) || key_.check(FcitxKey_Delete))
      type_ = Type::Delete;
    else if (key_.isCursorMove())
      type_ = Type::Navigation;
    else if (key_.isSimple())
      type_ = Type::Input;
    else
      type_ = Type::Unknown;
  }
};

#endif
