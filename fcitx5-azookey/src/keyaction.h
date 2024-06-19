#ifndef _FCITX5_AZOOKEY_KEYACTION_H_
#define _FCITX5_AZOOKEY_KEYACTION_H_

#include <fcitx-utils/key.h>
#include <fcitx-utils/keysymgen.h>

struct KeyAction {
  enum class Type { Input, Delete, Enter, Space, Escape, Navigation, Unknown };
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

  bool isInput() const { return type == Type::Input; }
  bool isDelete() const { return type == Type::Delete; }
  bool isEnter() const { return type == Type::Enter; }
  bool isSpace() const { return type == Type::Space; }
  bool isEscape() const { return type == Type::Escape; }
  bool isNavigation() const { return type == Type::Navigation; }
  bool isUnknown() const { return type == Type::Unknown; }

  DeleteDirection deleteDirection() const {
    if (key.check(FcitxKey_BackSpace)) {
      return DeleteDirection::Backward;
    }
    if (key.check(FcitxKey_Delete)) {
      return DeleteDirection::Forward;
    }
    return DeleteDirection::Unknown;
  }
  NavigationDirection navigationDirection() const {
    if (key.check(FcitxKey_Left) || key.check(FcitxKey_KP_Left)) {
      return NavigationDirection::Left;
    }
    if (key.check(FcitxKey_Right) || key.check(FcitxKey_KP_Right)) {
      return NavigationDirection::Right;
    }
    if (key.check(FcitxKey_Up) || key.check(FcitxKey_KP_Up)) {
      return NavigationDirection::Up;
    }
    if (key.check(FcitxKey_Down) || key.check(FcitxKey_KP_Down)) {
      return NavigationDirection::Down;
    }
    if (key.check(FcitxKey_Home) || key.check(FcitxKey_KP_Home)) {
      return NavigationDirection::Home;
    }
    if (key.check(FcitxKey_End) || key.check(FcitxKey_KP_End)) {
      return NavigationDirection::End;
    }
    if (key.check(FcitxKey_Page_Up) || key.check(FcitxKey_KP_Page_Up)) {
      return NavigationDirection::PageUp;
    }
    if (key.check(FcitxKey_Page_Down) || key.check(FcitxKey_KP_Page_Down)) {
      return NavigationDirection::PageDown;
    }
    return NavigationDirection::Unknown;
  }

  fcitx::Key key;
  std::string surface;
  Type type;

  KeyAction(const fcitx::Key &key)
      : key(key), surface(fcitx::Key::keySymToUTF8(key.sym())) {
    if (key.check(FcitxKey_Return)) {
      type = Type::Enter;
    } else if (key.check(FcitxKey_space) || key.check(FcitxKey_KP_Space)) {
      type = Type::Space;
    } else if (key.check(FcitxKey_Escape)) {
      type = Type::Escape;
    } else if (key.check(FcitxKey_BackSpace) || key.check(FcitxKey_Delete)) {
      type = Type::Delete;
    } else if (key.isCursorMove()) {
      type = Type::Navigation;
    } else if (key.isSimple()) {
      type = Type::Input;
    } else {
      type = Type::Unknown;
    }
  }
};

#endif
