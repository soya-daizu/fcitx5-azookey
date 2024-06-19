#ifndef BridgingHeader_h
#define BridgingHeader_h

#ifdef __cplusplus
extern "C" {
#endif

// KanaKanjiConverter

void *ak_kana_kanji_converter_new();

void ak_kana_kanji_converter_dispose(void *raw_ptr);

// ComposingText

void *ak_composing_text_new();

void ak_composing_text_dispose(void *raw_ptr);

void ak_composing_text_reset(void *raw_ptr);

void ak_composing_text_insert(void *raw_ptr, const char *text);

void ak_composing_text_delete_backward(void *raw_ptr);

bool ak_composing_text_is_composing(void *raw_ptr);

const char *ak_composing_text_get_convert_target(void *raw_ptr);

int ak_composing_text_get_cursor_position(void *raw_ptr);

void ak_request_candidates();

#ifdef __cplusplus
}
#endif

#endif /* BridgingHeader_h */
