#include "AzookeyCandidateList.h"
#include "AzookeyEngine.h"
#include "BridgingHeader.h"

AzookeyCandidateList::AzookeyCandidateList(AzookeyEngine *engine,
                                           fcitx::InputContext *ic,
                                           void *composingText)
    : engine_(engine), ic_(ic), composingText_(composingText) {
  setCursorPositionAfterPaging(fcitx::CursorPositionAfterPaging::SameAsLast);
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
  setGlobalCursorIndex(0);

  std::vector<std::string> labels;
  for (int i = 0; i < pageSize(); i++) {
    labels.push_back(std::to_string(i + 1) + ". ");
  }
  setLabels(labels);

  engine_->freeCandidateResult(result);
}

AzookeyCandidateWord::AzookeyCandidateWord(AzookeyEngine *engine,
                                           fcitx::Text text, int idx)
    : fcitx::CandidateWord(), engine_(engine), idx(idx) {
  setText(std::move(text));
}
