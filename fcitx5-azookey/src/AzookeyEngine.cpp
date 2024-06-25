#include "AzookeyEngine.h"
#include "AzookeyState.h"
#include "BridgingHeader.h"

AzookeyEngine::AzookeyEngine(fcitx::Instance *instance)
    : instance_(instance), factory_([this](fcitx::InputContext &ic) {
        return new AzookeyState(this, &ic);
      }),
      kanaKanjiConverter_(ak_kana_kanji_converter_new()) {
  FCITX_DEBUG() << "init engine";
  instance->inputContextManager().registerProperty("azookeyState", &factory_);
}

void AzookeyEngine::keyEvent(const fcitx::InputMethodEntry &entry,
                             fcitx::KeyEvent &keyEvent) {
  FCITX_UNUSED(entry);
  if (keyEvent.isRelease())
    return;

  auto ic = keyEvent.inputContext();
  auto *state = ic->propertyFor(&factory_);
  state->keyEvent(keyEvent);
}

void AzookeyEngine::reset(const fcitx::InputMethodEntry &,
                          fcitx::InputContextEvent &event) {
  auto *state = event.inputContext()->propertyFor(&factory_);
  state->reset();
}

void AzookeyEngine::freeCandidateResult(ConversionResult *result) {
  freeCandidates(result->mainResults);
  freeCandidates(result->firstClauseResults);
  free((void *)result);
}

void AzookeyEngine::freeSegmentedCandidateResult(
    SegmentedConversionResult *result) {
  for (int i = 0; result->mainResults[i] != nullptr; i++)
    freeCandidates(result->mainResults[i]);
  free((void *)result->mainResults);
  free((void *)result->segmentResult);
  free((void *)result);
}

void AzookeyEngine::freeCandidates(const Candidate **candidates) {
  for (int i = 0; candidates[i] != nullptr; i++) {
    const Candidate *candidate = candidates[i];
    for (int j = 0; candidate->data[j] != nullptr; j++) {
      const DicdataElement *element = candidate->data[j];
      free((void *)element->word);
      free((void *)element->ruby);
      free((void *)element);
    }
    free((void *)candidate->text);
    free((void *)candidate->data);
    free((void *)candidate);
  }
  free((void *)candidates);
}

FCITX_ADDON_FACTORY(AzookeyEngineFactory);
