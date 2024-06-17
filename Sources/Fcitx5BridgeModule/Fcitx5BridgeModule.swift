@_exported import KanaKanjiConverterModuleWithDefaultDictionary
import Foundation

@MainActor
@_cdecl("init_swift_azookey")
public func initSwiftAzooKey() {
    let converter = KanaKanjiConverter()
    // 入力を初期化する
    var c = ComposingText()
    // 変換したい文章を追加する
    c.insertAtCursorPosition("あずーきーはしんじだいのきーぼーどあぷりです", inputStyle: .direct)
    let results = converter.requestCandidates(c, options: requestOptions())
    // 結果の一番目を表示
    print(results.mainResults.first!.text)  // azooKeyは新時代のキーボードアプリです
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
