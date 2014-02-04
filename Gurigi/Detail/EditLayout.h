#pragma once

#include "../ComUtility.h"
#include "../Objects.h"

namespace Gurigi
{
    namespace Detail
    {
        // From CustomLayout in Windows SDK examples.

        class TextAnalysis : public ComBase<ComBaseList<IDWriteTextAnalysisSource,
            ComBaseList<IDWriteTextAnalysisSink, ComBaseList<IUnknown>>>>
        {
        public:
            struct Run
            {
                Run()
                    : textStart(0)
                    , textLength(0)
                    , scriptAnalysis()
                    , bidiLevel(0)
                    , isNumberSubstituted(false)
                    , isSideways(false)
                {}

                bool contains(size_t pos) const
                {
                    return textStart <= pos && pos < textStart + textLength;
                }

                size_t textStart;
                size_t textLength;
                DWRITE_SCRIPT_ANALYSIS scriptAnalysis;
                uint8_t bidiLevel;
                bool isNumberSubstituted;
                bool isSideways;
            };

        public:
            TextAnalysis(const std::wstring &text, const std::wstring &locale,
                IDWriteNumberSubstitution *numberSubstitution, DWRITE_READING_DIRECTION readingDirection)
                : text_(text)
                , locale_(locale)
                , numberSubstitution_(numberSubstitution)
                , readingDirection_(readingDirection)
            {}

        public:
            HRESULT analyze(IDWriteTextAnalyzer *analyzer);

        public:
            const std::list<Run> &runs() const
            {
                return runs_;
            }
            const std::vector<DWRITE_LINE_BREAKPOINT> &breakpoints() const
            {
                return breakpoints_;
            }

        public:
            // IDWriteTextAnalysisSource declaration
            virtual HRESULT __stdcall GetTextAtPosition(uint32_t, const wchar_t **, uint32_t *);
            virtual HRESULT __stdcall GetTextBeforePosition(uint32_t, const wchar_t **, uint32_t *);
            virtual DWRITE_READING_DIRECTION __stdcall GetParagraphReadingDirection();
            virtual HRESULT __stdcall GetLocaleName(uint32_t, uint32_t *, const wchar_t **);
            virtual HRESULT __stdcall GetNumberSubstitution(uint32_t, uint32_t *, IDWriteNumberSubstitution **);
            // IDWriteTextAnalysisSink declaration
            virtual HRESULT __stdcall SetLineBreakpoints(uint32_t, uint32_t, const DWRITE_LINE_BREAKPOINT *);
            virtual HRESULT __stdcall SetScriptAnalysis(uint32_t, uint32_t, const DWRITE_SCRIPT_ANALYSIS *);
            virtual HRESULT __stdcall SetBidiLevel(uint32_t, uint32_t, uint8_t, uint8_t);
            virtual HRESULT __stdcall SetNumberSubstitution(uint32_t, uint32_t, IDWriteNumberSubstitution *);

        private:
            Run &getNextRun(size_t &);
            void setCurrentRun(size_t);
            void splitCurrentRun(size_t);

        private:
            std::wstring text_;
            std::wstring locale_;
            IDWriteNumberSubstitution *numberSubstitution_;
            DWRITE_READING_DIRECTION readingDirection_;
            std::list<Run> runs_;
            std::list<Run>::iterator runIt_;
            std::vector<DWRITE_LINE_BREAKPOINT> breakpoints_;
        };

        class EditLayoutSource
        {
        private:
            Objects::SizeF size_;
            float currentY_;

        public:
            EditLayoutSource();

        public:
            void clear();
            void prepare(const Objects::SizeF &);
            void getNextArea(float, Objects::RectangleF &);
        };

        class EditLayoutSink
        {
        public:
            template<typename T>
            using RandomAnyRange = boost::any_range<T, boost::random_access_traversal_tag, T &, ptrdiff_t>;

        private:
            struct GlyphRun
            {
                size_t textStartPos;
                size_t textLength;
                ComPtr<IDWriteFontFace> fontFace;
                float fontEmSize;
                Objects::PointF baselineOffset;
                size_t glyphStartPos;
                size_t glyphCount;
                uint8_t bidiLevel;
                bool isSideways;
            };

        private:
            std::vector<GlyphRun> glyphRuns_;
            std::vector<uint16_t> glyphIndices_;
            std::vector<float> glyphAdvances_;
            std::vector<DWRITE_GLYPH_OFFSET> glyphOffsets_;
            std::vector<uint16_t> glyphClusters_;

        public:
            EditLayoutSink();
            void clear();
            void prepare(size_t);
            void addGlyphRun(size_t, const RandomAnyRange<uint16_t> &,
                const Objects::PointF &,
                const RandomAnyRange<uint16_t> &, const RandomAnyRange<float> &, const RandomAnyRange<DWRITE_GLYPH_OFFSET> &,
                const ComPtr<IDWriteFontFace> &,
                float, uint8_t, bool);
            void draw(Gurigi::Drawing::Context &, const Objects::PointF &, const ComPtr<ID2D1Brush> &) const;
            bool getTextPosInfo(size_t, bool, Objects::PointF &, ComPtr<IDWriteFontFace> &, float &) const;
            bool hitTestTextPos(const Objects::PointF &, size_t &, bool &) const;
        };

        class EditLayout
        {
        private:
            struct TextFormatInfo
            {
                ComPtr<IDWriteFontFace> fontFace;
                std::wstring localeName;
                DWRITE_READING_DIRECTION readingDirection;
                // TODO: color, decoration, ...
            };

            struct Run: TextAnalysis::Run
            {
                Run()
                    : TextAnalysis::Run()
                    , glyphStart(0)
                    , glyphCount(0)
                    , textFormatInfo(nullptr)
                {}
                Run(const TextAnalysis::Run &p)
                    : TextAnalysis::Run(p)
                    , glyphStart(0)
                    , glyphCount(0)
                    , textFormatInfo(nullptr)
                {}

                size_t glyphStart;
                size_t glyphCount;
                TextFormatInfo *textFormatInfo;
            };

            struct ClusterPosition
            {
                size_t textPosition;
                size_t runIndex;
                size_t runEndPosition;
            };

        private:
            bool invalidated_;
            std::wstring text_;
            std::vector<ComPtr<IDWriteTextFormat>> textFormats_;
            std::vector<TextFormatInfo> textFormatInfos_;
            float fontEmSize_;
            float descentMax_;
            DWRITE_READING_DIRECTION defaultReadingDirection_;
            Objects::SizeF size_;
            std::wstring locale_;
            IDWriteNumberSubstitution *numberSubstitution_;

            std::vector<Run> runs_;
            std::vector<Run>::iterator runIt_;
            std::vector<DWRITE_LINE_BREAKPOINT> breakpoints_;
            std::vector<uint16_t> glyphIndices_;
            std::vector<uint16_t> glyphClusters_;
            std::vector<float> glyphAdvances_;
            std::vector<DWRITE_GLYPH_OFFSET> glyphOffsets_;

        public:
            EditLayout();
            ~EditLayout();

        public:
            const std::wstring &text() const;
            void text(const std::wstring &);
            const std::vector<ComPtr<IDWriteTextFormat>> &textFormats() const;
            void textFormats(const std::vector<ComPtr<IDWriteTextFormat>> &);
            void textFormats(std::vector<ComPtr<IDWriteTextFormat>> &&);
            DWRITE_READING_DIRECTION defaultReadingDirection() const;
            void defaultReadingDirection(DWRITE_READING_DIRECTION);
            float fontEmSize() const;
            void fontEmSize(float);
            const Objects::SizeF &size() const;
            void size(const Objects::SizeF &);

        public:
            void analyze();
            void flow(EditLayoutSource &, EditLayoutSink &);

        private:
            void invalidate();
            Run &getNextRun(size_t &);
            void setCurrentRun(size_t);
            void splitCurrentRun(size_t);
            bool substituteFonts();
            bool setInlineTextFormats();
            bool shapeGlyphRuns(IDWriteTextAnalyzer *);
            bool shapeGlyphRun(IDWriteTextAnalyzer *, Run &, size_t &);
            void setClusterPosition(ClusterPosition &, size_t) const;
            void advanceClusterPosition(ClusterPosition &) const;
            size_t getClusterGlyphStart(const ClusterPosition &) const;
            float getClusterRangeWidth(const ClusterPosition &, const ClusterPosition &) const;
            float getClusterRangeWidth(size_t, size_t, const std::vector<float> &) const;
            void fitText(const ClusterPosition &, size_t, float, ClusterPosition &);
            void produceGlyphRuns(EditLayoutSink &, const Objects::RectangleF &, const ClusterPosition &, const ClusterPosition &);
            void produceBidiOrdering(size_t, std::vector<size_t> &) const;
            void produceJustifiedAdvances(const Objects::RectangleF &, const ClusterPosition &, const ClusterPosition &,
                std::vector<float> &) const;
        };
    }
}
