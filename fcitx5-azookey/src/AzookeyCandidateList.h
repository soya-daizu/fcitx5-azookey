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

  int idx() const { return idx_; }

private:
  AzookeyEngine *engine_;
  int idx_;
};

class AzookeyCandidateList : public fcitx::CommonCandidateList {
public:
  AzookeyCandidateList(AzookeyEngine *engine, fcitx::InputContext *ic,
                       void *composingText);
  AzookeyCandidateList(const AzookeyCandidateList &rhs);

  void generateRealtimeCandidates();
  void generateFullLengthCandidates();
  void generateMultiSegmentCandidates(std::vector<long> &segments);
  void generateSegmentCandidates();

  fcitx::CandidateLayoutHint layoutHint() const override {
    return fcitx::CandidateLayoutHint::Vertical;
  }
  void prevCandidate() override {
    fcitx::CommonCandidateList::prevCandidate();
    if (segmentCandidateIndexes_.size() > 0)
      segmentCandidateIndexes_[segmentIndex_] = currentCandidateIdx();
  }
  void nextCandidate() override {
    fcitx::CommonCandidateList::nextCandidate();
    if (segmentCandidateIndexes_.size() > 0)
      segmentCandidateIndexes_[segmentIndex_] = currentCandidateIdx();
  }
  void next() override {
    fcitx::CommonCandidateList::next();
    if (segmentCandidateIndexes_.size() > 0)
      segmentCandidateIndexes_[segmentIndex_] = currentCandidateIdx();
  }
  void prev() override {
    fcitx::CommonCandidateList::prev();
    if (segmentCandidateIndexes_.size() > 0)
      segmentCandidateIndexes_[segmentIndex_] = currentCandidateIdx();
  }

  std::string currentCandidateString() {
    return candidate(cursorIndex()).text().toString();
  }
  int currentCandidateIdx() {
    const AzookeyCandidateWord &akCandidate =
        dynamic_cast<const AzookeyCandidateWord &>(candidate(cursorIndex()));
    return akCandidate.idx();
  }

  // Multi-segment mode
  std::vector<long> segments() { return segments_; }
  bool shiftSegment(bool forward) {
    if (forward) {
      if (segmentIndex_ + 1 < (int)segments_.size()) {
        FCITX_INFO() << "before segments: " << segments_;
        segments_[segmentIndex_]++;
        segments_[segmentIndex_ + 1]--;
        if (segments_[segmentIndex_ + 1] == 0)
          segments_.pop_back();
        FCITX_INFO() << "after segments: " << segments_;
        return true;
      }
    } else {
      if (segments_[segmentIndex_] > 1) {
        FCITX_INFO() << "before segments: " << segments_;
        segments_[segmentIndex_]--;
        if (segmentIndex_ + 1 == (int)segments_.size())
          segments_.push_back(1);
        else
          segments_[segmentIndex_ + 1]++;
        FCITX_INFO() << "after segments: " << segments_;
        return true;
      }
    }
    return false;
  }
  bool moveSegment(bool forward) {
    if (forward) {
      segmentIndex_++;
      if (segmentIndex_ >= (int)segments_.size())
        segmentIndex_ = 0;
    } else {
      segmentIndex_--;
      if (segmentIndex_ < 0)
        segmentIndex_ = segments_.size() - 1;
    }
    return true;
  }
  fcitx::Text buildWholeText() {
    fcitx::Text text;
    for (size_t i = 0; i < segmentCandidates_.size(); i++) {
      size_t segmentCandidateIndex = segmentCandidateIndexes_[i];
      if (i == (size_t)segmentIndex_) {
        text.setCursor(text.textLength());
        text.append(segmentCandidates_[i][segmentCandidateIndex],
                    fcitx::TextFormatFlag::HighLight);
      } else {
        text.append(segmentCandidates_[i][segmentCandidateIndex],
                    fcitx::TextFormatFlag::Underline);
      }
    }
    return text;
  }

private:
  AzookeyEngine *engine_;
  fcitx::InputContext *ic_;
  void *composingText_;

  // Multi-segment mode
  std::vector<long> segments_;
  int segmentIndex_{};
  std::vector<std::vector<std::string>> segmentCandidates_;
  std::vector<int> segmentCandidateIndexes_;
};

#endif // _FCITX5_AZOOKEY_AZOOKEYCANDIDATELIST_H_
