#include "AzookeyCandidateList.h"
#include "AzookeyEngine.h"
#include "BridgingHeader.h"

AzookeyCandidateList::AzookeyCandidateList(AzookeyEngine *engine,
                                           fcitx::InputContext *ic,
                                           void *composingText)
    : engine_(engine), ic_(ic), composingText_(composingText) {
  setCursorPositionAfterPaging(fcitx::CursorPositionAfterPaging::SameAsLast);
}

AzookeyCandidateList::AzookeyCandidateList(const AzookeyCandidateList &rhs)
    : engine_(rhs.engine_), ic_(rhs.ic_), composingText_(rhs.composingText_) {
  setCursorPositionAfterPaging(fcitx::CursorPositionAfterPaging::SameAsLast);
  segmentIndex_ = rhs.segmentIndex_;

  segments_.resize(rhs.segments_.size());
  for (unsigned long i = 0; i < rhs.segments_.size(); i++)
    segments_[i] = rhs.segments_[i];

  segmentCandidateIndexes_.resize(rhs.segmentCandidateIndexes_.size());
  for (unsigned long i = 0; i < rhs.segmentCandidateIndexes_.size(); i++)
    segmentCandidateIndexes_[i] = rhs.segmentCandidateIndexes_[i];

  segmentCandidates_.resize(rhs.segmentCandidates_.size());
  for (unsigned long i = 0; i < rhs.segmentCandidates_.size(); i++) {
    segmentCandidates_[i].resize(rhs.segmentCandidates_[i].size());
    for (unsigned long j = 0; j < rhs.segmentCandidates_[i].size(); j++)
      segmentCandidates_[i][j] = rhs.segmentCandidates_[i][j];
  }

  generateSegmentCandidates();
}

void AzookeyCandidateList::generateRealtimeCandidates() {
  void *converter = engine_->kanaKanjiConverter();
  ConvertRequestOptions options = {
      .N_best = 9,
      .requireJapanesePrediction = true,
  };
  ConversionResult *result = ak_kana_kanji_converter_request_candidates(
      converter, composingText_, &options);

  float firstValue;
  for (int i = 0; result->mainResults[i] != nullptr; i++) {
    const Candidate *candidate = result->mainResults[i];

    bool shouldAdd = i < 3;
    if (i == 0)
      firstValue = candidate->value;
    else if (i < 3) {
      float diff = firstValue - candidate->value;
      if (diff > std::abs(firstValue * 0.075))
        shouldAdd = false;
    } else
      break;
    if (!shouldAdd)
      continue;

    fcitx::Text text;
    text.append(candidate->text);
    append(std::make_unique<AzookeyCandidateWord>(engine_, text, i));
  }
  setPageSize(totalSize());

  engine_->freeCandidateResult(result);
}

void AzookeyCandidateList::generateFullLengthCandidates() {
  void *converter = engine_->kanaKanjiConverter();
  ConvertRequestOptions options = {
      .N_best = 9,
      .requireJapanesePrediction = true,
  };
  ConversionResult *result = ak_kana_kanji_converter_request_candidates(
      converter, composingText_, &options);
  long inputCount = ak_composing_text_get_input_count(composingText_);

  for (int i = 0; result->mainResults[i] != nullptr; i++) {
    const Candidate *candidate = result->mainResults[i];
    if (candidate->correspondingCount != inputCount)
      continue;

    fcitx::Text text;
    text.append(candidate->text);
    append(std::make_unique<AzookeyCandidateWord>(engine_, text, i));
  }
  setPageSize(9);
  setCursorIndex(0);

  std::vector<std::string> labels;
  for (int i = 0; i < pageSize(); i++) {
    labels.push_back(std::to_string(i + 1) + ". ");
  }
  setLabels(labels);

  engine_->freeCandidateResult(result);
}

void AzookeyCandidateList::generateMultiSegmentCandidates(
    std::vector<long> &segments) {
  void *converter = engine_->kanaKanjiConverter();
  ConvertRequestOptions options = {
      .N_best = 9,
      .requireJapanesePrediction = false,
  };
  SegmentedConversionResult *result =
      ak_kana_kanji_converter_request_candidates_with_segments(
          converter, composingText_, &segments[0], segments.size(), &options);

  const Candidate ***segmentCandidates = result->mainResults;
  const long *segmentResult = result->segmentResult;

  segmentCandidates_ = {};
  segmentCandidateIndexes_ = {};
  for (int i = 0; segmentCandidates[i] != nullptr; i++) {
    segmentCandidates_.push_back({});
    segmentCandidateIndexes_.push_back(0);
    const Candidate **candidates = segmentCandidates[i];
    for (int j = 0; candidates[j] != nullptr; j++) {
      const Candidate *candidate = candidates[j];
      segmentCandidates_.back().push_back(candidate->text);
    }
  }

  int segmentLength = 0;
  for (; segmentResult[segmentLength] != -1; segmentLength++) {
  }
  segments_ = std::vector<long>(segmentResult, segmentResult + segmentLength);

  engine_->freeSegmentedCandidateResult(result);
}

void AzookeyCandidateList::generateSegmentCandidates() {
  auto candidates = segmentCandidates_[segmentIndex_];
  int candidateIdx = segmentCandidateIndexes_[segmentIndex_];

  clear();

  fcitx::Text text;
  text.append(candidates[candidateIdx]);
  append(std::make_unique<AzookeyCandidateWord>(engine_, text, candidateIdx));

  for (unsigned long i = 0; i < candidates.size(); i++) {
    if (i == (unsigned long)candidateIdx)
      continue;
    fcitx::Text text;
    text.append(candidates[i]);
    append(std::make_unique<AzookeyCandidateWord>(engine_, text, i));
  }
  setPageSize(9);
  setCursorIndex(0);

  std::vector<std::string> labels;
  for (int i = 0; i < pageSize(); i++) {
    labels.push_back(std::to_string(i + 1) + ". ");
  }
  setLabels(labels);
}

AzookeyCandidateWord::AzookeyCandidateWord(AzookeyEngine *engine,
                                           fcitx::Text text, int idx)
    : fcitx::CandidateWord(), engine_(engine), idx_(idx) {
  setText(std::move(text));
}
