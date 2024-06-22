#ifndef BridgingHeader_h
#define BridgingHeader_h

#ifdef __cplusplus
extern "C" {
#endif

// KanaKanjiConverter

typedef struct {
  long N_best;
  bool requireJapanesePrediction;
} ConvertRequestOptions;

typedef struct {
  const char *word;
  const char *ruby;
} DicdataElement;

typedef struct {
  const char *text;
  float value;
  long correspondingCount;
  const DicdataElement **data;
} Candidate;

typedef struct {
  const Candidate **mainResults;
  const Candidate **firstClauseResults;
} ConversionResult;

void *ak_kana_kanji_converter_new();

void ak_kana_kanji_converter_dispose(void *raw_ptr);

ConversionResult *ak_kana_kanji_converter_request_candidates(
    void *raw_ptr, void *composing_text_raw_ptr,
    ConvertRequestOptions *options_raw_ptr);

// ComposingText

void *ak_composing_text_new();

void ak_composing_text_dispose(void *raw_ptr);

void ak_composing_text_reset(void *raw_ptr);

void ak_composing_text_insert(void *raw_ptr, const char *text);

void ak_composing_text_delete_backward(void *raw_ptr);

void ak_composing_text_delete_forward(void *raw_ptr);

long ak_composing_text_move_cursor(void *raw_ptr, long count);

long ak_composing_text_set_cursor(void *raw_ptr, long position);

bool ak_composing_text_is_composing(void *raw_ptr);

long ak_composing_text_get_input_count(void *raw_ptr);

const char *ak_composing_text_get_convert_target(void *raw_ptr);

long ak_composing_text_get_cursor_position(void *raw_ptr);

#ifdef __cplusplus
}
#endif

#endif /* BridgingHeader_h */
