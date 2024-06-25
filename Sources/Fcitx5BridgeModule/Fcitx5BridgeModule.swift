import KanaKanjiConverterModuleWithDefaultDictionary
import Foundation

// KanaKanjiConverter

struct ConvertRequestOptions {
  public let N_best: Int?;
  public let requireJapanesePrediction: Bool?;
}

struct DicDataElement {
  public let word: UnsafePointer<CChar>?
  public let ruby: UnsafePointer<CChar>?
}

struct Candidate {
    public let text: UnsafePointer<CChar>?
    public let value: Float32
    public let correspondingCount: Int
    public let data: UnsafeMutablePointer<UnsafeMutablePointer<DicDataElement>?>
}

struct ConversionResult {
    public let mainResults: UnsafeMutablePointer<UnsafeMutablePointer<Candidate>?>
    public let firstClauseResults: UnsafeMutablePointer<UnsafeMutablePointer<Candidate>?>
}

struct SegmentedConversionResult {
    public let mainResults: UnsafeMutablePointer<UnsafeMutablePointer<UnsafeMutablePointer<Candidate>?>?>
    public let segmentResult: UnsafeMutablePointer<Int>
}

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

@MainActor
@_cdecl("ak_kana_kanji_converter_request_candidates")
public func kanaKanjiConverterRequestCandidates(rawPtr: OpaquePointer, composingTextRawPtr: OpaquePointer, optionsRawPtr: OpaquePointer) -> UnsafeRawPointer {
    let ptr = UnsafeMutablePointer<KanaKanjiConverter>(rawPtr)
    let composingTextPtr = UnsafeMutablePointer<ComposingText>(composingTextRawPtr)
    let optionsPtr = UnsafeMutablePointer<ConvertRequestOptions>(optionsRawPtr)
    let converter = ptr.pointee
    let composingText = composingTextPtr.pointee
    let options = optionsPtr.pointee

    let results = converter.requestCandidates(composingText, options: requestOptions(options: options))
    //print(results.mainResults.map { $0.text })
    //print(results.firstClauseResults.map { $0.text })

    let mainResultsPtr = UnsafeMutablePointer<UnsafeMutablePointer<Candidate>?>.allocate(capacity: results.mainResults.count + 1)
    for i in 0..<results.mainResults.count {
        let candidate = results.mainResults[i]

        let dataPtr = UnsafeMutablePointer<UnsafeMutablePointer<DicDataElement>?>.allocate(capacity: candidate.data.count + 1)
        for j in 0..<candidate.data.count {
            let item = candidate.data[j]
            let ptr = UnsafeMutablePointer<DicDataElement>.allocate(capacity: 1)
            ptr.initialize(to: DicDataElement(word: strdup(item.word), ruby: strdup(item.ruby)))
            dataPtr[j] = ptr
        }
        dataPtr[candidate.data.count] = nil

        let ptr = UnsafeMutablePointer<Candidate>.allocate(capacity: 1)
        ptr.initialize(to: Candidate(
            text: UnsafePointer(strdup(candidate.text)),
            value: candidate.value,
            correspondingCount: candidate.correspondingCount,
            data: dataPtr
        ))
        mainResultsPtr[i] = ptr
    }
    mainResultsPtr[results.mainResults.count] = nil

    let firstClauseResultsPtr = UnsafeMutablePointer<UnsafeMutablePointer<Candidate>?>.allocate(capacity: results.firstClauseResults.count + 1)
    for i in 0..<results.firstClauseResults.count {
        let candidate = results.firstClauseResults[i]

        let dataPtr = UnsafeMutablePointer<UnsafeMutablePointer<DicDataElement>?>.allocate(capacity: candidate.data.count + 1)
        for j in 0..<candidate.data.count {
            let item = candidate.data[j]
            let ptr = UnsafeMutablePointer<DicDataElement>.allocate(capacity: 1)
            ptr.initialize(to: DicDataElement(word: strdup(item.word), ruby: strdup(item.ruby)))
            dataPtr[j] = ptr
        }
        dataPtr[candidate.data.count] = nil

        let ptr = UnsafeMutablePointer<Candidate>.allocate(capacity: 1)
        ptr.initialize(to: Candidate(
            text: UnsafePointer(strdup(candidate.text)),
            value: candidate.value,
            correspondingCount: candidate.correspondingCount,
            data: dataPtr
        ))
        firstClauseResultsPtr[i] = ptr
    }
    firstClauseResultsPtr[results.firstClauseResults.count] = nil

    let conversionResultPtr = UnsafeMutablePointer<ConversionResult>.allocate(capacity: 1)
    conversionResultPtr.initialize(to: ConversionResult(
        mainResults: mainResultsPtr,
        firstClauseResults: firstClauseResultsPtr
    ))
    return UnsafeRawPointer(conversionResultPtr)
}

@MainActor
@_cdecl("ak_kana_kanji_converter_request_candidates_with_segments")
public func kanaKanjiConverterRequestCandidates(rawPtr: OpaquePointer, composingTextRawPtr: OpaquePointer, segmentsRawPtr: OpaquePointer?, optionsRawPtr: OpaquePointer) -> UnsafeRawPointer {
    let ptr = UnsafeMutablePointer<KanaKanjiConverter>(rawPtr)
    let composingTextPtr = UnsafeMutablePointer<ComposingText>(composingTextRawPtr)
    let segmentsPtr = UnsafeMutablePointer<Int?>(segmentsRawPtr)
    let optionsPtr = UnsafeMutablePointer<ConvertRequestOptions>(optionsRawPtr)
    let converter = ptr.pointee
    let composingText = composingTextPtr.pointee
    let options = optionsPtr.pointee

    var segments: [Int]?
    if (segmentsPtr != nil) {
      segments = [Int]()
      while segmentsPtr![segments!.count] != nil {
          segments!.append(Int(segmentsPtr![segments!.count]!))
      }
    }

    let results = converter.requestCandidatesWithSegments(composingText, segments: segments, options: requestOptions(options: options))
    print(results)

    let resultsPtr = UnsafeMutablePointer<UnsafeMutablePointer<UnsafeMutablePointer<Candidate>?>?>.allocate(capacity: results.mainResults.count + 1)
    for i in 0..<results.mainResults.count {
        let candidates = results.mainResults[i]
        let candidatesPtr = UnsafeMutablePointer<UnsafeMutablePointer<Candidate>?>.allocate(capacity: candidates.count + 1)
        for j in 0..<candidates.count {
            let candidate = candidates[j]

            let dataPtr = UnsafeMutablePointer<UnsafeMutablePointer<DicDataElement>?>.allocate(capacity: candidate.data.count + 1)
            for k in 0..<candidate.data.count {
                let item = candidate.data[k]
                let ptr = UnsafeMutablePointer<DicDataElement>.allocate(capacity: 1)
                ptr.initialize(to: DicDataElement(word: strdup(item.word), ruby: strdup(item.ruby)))
                dataPtr[k] = ptr
            }
            dataPtr[candidate.data.count] = nil

            let ptr = UnsafeMutablePointer<Candidate>.allocate(capacity: 1)
            ptr.initialize(to: Candidate(
                text: UnsafePointer(strdup(candidate.text)),
                value: candidate.value,
                correspondingCount: candidate.correspondingCount,
                data: dataPtr
            ))
            candidatesPtr[j] = ptr
        }
        candidatesPtr[candidates.count] = nil
        resultsPtr[i] = candidatesPtr
    }
    resultsPtr[results.mainResults.count] = nil

    let segmentResultPtr = UnsafeMutablePointer<Int>.allocate(capacity: results.segmentResult.count + 1)
    for i in 0..<results.segmentResult.count {
        segmentResultPtr[i] = results.segmentResult[i]
    }
    segmentResultPtr[results.segmentResult.count] = -1

    let conversionResultPtr = UnsafeMutablePointer<SegmentedConversionResult>.allocate(capacity: 1)
    conversionResultPtr.initialize(to: SegmentedConversionResult(
        mainResults: resultsPtr,
        segmentResult: segmentResultPtr
    ))
    return UnsafeRawPointer(conversionResultPtr)
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

    ptr.initialize(to: composingText)
}

@_cdecl("ak_composing_text_insert")
public func composingTextInsert(rawPtr: OpaquePointer, text: UnsafePointer<CChar>) {
    let ptr = UnsafeMutablePointer<ComposingText>(rawPtr)
    var composingText = ptr.pointee

    let text = String(cString: text)
    composingText.insertAtCursorPosition(text, inputStyle: .roman2kana)

    ptr.initialize(to: composingText)
}

@_cdecl("ak_composing_text_delete_backward")
public func composingTextDeleteBackward(rawPtr: OpaquePointer) {
    let ptr = UnsafeMutablePointer<ComposingText>(rawPtr)
    var composingText = ptr.pointee

    composingText.deleteBackwardFromCursorPosition(count: 1)

    ptr.initialize(to: composingText)
}

@_cdecl("ak_composing_text_delete_forward")
public func composingTextDeleteForward(rawPtr: OpaquePointer) {
    let ptr = UnsafeMutablePointer<ComposingText>(rawPtr)
    var composingText = ptr.pointee

    composingText.deleteForwardFromCursorPosition(count: 1)

    ptr.initialize(to: composingText)
}

@_cdecl("ak_composing_text_move_cursor")
public func composingTextMoveCursor(rawPtr: OpaquePointer, count: Int) -> Int {
    let ptr = UnsafeMutablePointer<ComposingText>(rawPtr)
    var composingText = ptr.pointee

    let movedCount = composingText.moveCursorFromCursorPosition(count: count)

    ptr.initialize(to: composingText)
    return movedCount
}

@_cdecl("ak_composing_text_set_cursor")
public func composingTextSetCursor(rawPtr: OpaquePointer, position: Int) -> Int {
    let ptr = UnsafeMutablePointer<ComposingText>(rawPtr)
    var composingText = ptr.pointee

    let count: Int
    if position < 0 {
        let currentPosition = composingText.convertTarget.count
        count = currentPosition + position + 1
    } else {
        let currentPosition = composingText.convertTargetCursorPosition
        count = position - currentPosition
    }
    let movedCount = composingText.moveCursorFromCursorPosition(count: count)

    ptr.initialize(to: composingText)
    return movedCount
}

@_cdecl("ak_composing_text_is_composing")
public func composingTextIsComposing(rawPtr: OpaquePointer) -> Bool {
    let ptr = UnsafeMutablePointer<ComposingText>(rawPtr)
    let composingText = ptr.pointee
    return composingText.input.count > 0
}

@_cdecl("ak_composing_text_get_input_count")
public func composingTextGetInputCount(rawPtr: OpaquePointer) -> Int {
    let ptr = UnsafeMutablePointer<ComposingText>(rawPtr)
    let composingText = ptr.pointee
    return composingText.input.count
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

func requestOptions(options: ConvertRequestOptions) -> KanaKanjiConverterModule.ConvertRequestOptions {
    let result: KanaKanjiConverterModule.ConvertRequestOptions = .withDefaultDictionary(
        N_best: options.N_best ?? 10,
        requireJapanesePrediction: options.requireJapanesePrediction ?? true,
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
    return result
}
