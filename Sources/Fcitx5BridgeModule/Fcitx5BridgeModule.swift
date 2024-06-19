@_exported import KanaKanjiConverterModuleWithDefaultDictionary
import Foundation

// KanaKanjiConverter

@MainActor
@_cdecl("ak_kana_kanji_converter_new")
public func kanaKanjiConverterNew() -> UnsafeRawPointer {
    let ptr = UnsafeMutablePointer<KanaKanjiConverter>.allocate(capacity: 1)
    ptr.initialize(to: KanaKanjiConverter())
    return UnsafeRawPointer(ptr)
}

@_cdecl("ak_kana_kanji_converter_dispose")
public func kanaKanjiConverterDispose(rawPtr: OpaquePointer) {
    let ptr = UnsafeMutablePointer<KanaKanjiConverter>(rawPtr)
    ptr.deallocate()
}

// ComposingText

@_cdecl("ak_composing_text_new")
public func composingTextNew() -> UnsafeRawPointer {
    let ptr = UnsafeMutablePointer<ComposingText>.allocate(capacity: 1)
    ptr.initialize(to: ComposingText())
    return UnsafeRawPointer(ptr)
}

@_cdecl("ak_composing_text_dispose")
public func composingTextDispose(rawPtr: OpaquePointer) {
    let ptr = UnsafeMutablePointer<ComposingText>(rawPtr)
    ptr.deallocate()
}

@_cdecl("ak_composing_text_reset")
public func composingTextReset(rawPtr: OpaquePointer) {
    let ptr = UnsafeMutablePointer<ComposingText>(rawPtr)
    var composingText = ptr.pointee

    composingText.stopComposition()
    print(composingText)

    ptr.initialize(to: composingText)
}

@_cdecl("ak_composing_text_insert")
public func composingTextInsert(rawPtr: OpaquePointer, text: UnsafePointer<CChar>) {
    let ptr = UnsafeMutablePointer<ComposingText>(rawPtr)
    var composingText = ptr.pointee

    let text = String(cString: text)
    composingText.insertAtCursorPosition(text, inputStyle: .roman2kana)
    print(composingText)

    ptr.initialize(to: composingText)
}

@_cdecl("ak_composing_text_delete_backward")
public func composingTextDeleteBackward(rawPtr: OpaquePointer) {
    let ptr = UnsafeMutablePointer<ComposingText>(rawPtr)
    var composingText = ptr.pointee

    composingText.deleteBackwardFromCursorPosition(count: 1)
    print(composingText)

    ptr.initialize(to: composingText)
}

@_cdecl("ak_composing_text_is_composing")
public func composingTextIsComposing(rawPtr: OpaquePointer) -> Bool {
    let ptr = UnsafeMutablePointer<ComposingText>(rawPtr)
    let composingText = ptr.pointee
    return composingText.input.count > 0
}

@_cdecl("ak_composing_text_get_convert_target")
public func composingTextGetConvertTarget(rawPtr: OpaquePointer) -> UnsafePointer<CChar>? {
    let ptr = UnsafeMutablePointer<ComposingText>(rawPtr)
    let composingText = ptr.pointee

    let convertTarget = composingText.convertTarget
    let cString = strdup(convertTarget)
    return UnsafePointer(cString)
}

@_cdecl("ak_composing_text_get_cursor_position")
public func composingTextGetCursorPosition(rawPtr: OpaquePointer) -> Int {
    let ptr = UnsafeMutablePointer<ComposingText>(rawPtr)
    let composingText = ptr.pointee

    return composingText.convertTargetCursorPosition
}

func requestOptions() -> ConvertRequestOptions {
    let option: ConvertRequestOptions = .withDefaultDictionary(
        N_best: 10,
        requireJapanesePrediction: true,
        requireEnglishPrediction: false,
        keyboardLanguage: .ja_JP,
        typographyLetterCandidate: false,
        unicodeCandidate: true,
        englishCandidateInRoman2KanaInput: true,
        fullWidthRomanCandidate: false,
        halfWidthKanaCandidate: false,
        learningType: .nothing,
        maxMemoryCount: 0,
        shouldResetMemory: false,
        memoryDirectoryURL: URL(fileURLWithPath: ""),
        sharedContainerURL: URL(fileURLWithPath: ""),
        zenzaiMode: .off,
        metadata: .init(versionString: "anco for debugging")
    )
    return option
}
