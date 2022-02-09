#pragma once
#include "pch.h"

#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "FontFamily.h"

#include <memory>
#include <string>

class Text
{
public:
	Text(std::shared_ptr<DeviceResources> deviceResources);

	void Draw();

	void SetText(std::wstring text) { m_text = text; TextChanged(); }
	void SetText(std::string text) { m_text = std::wstring(text.begin(), text.end()); TextChanged(); }

	void AddChar(char key) { m_text.push_back(key); TextChanged(); }
	void Pop();

	void SetColor(D2D1_COLOR_F color);
	void SetFontFamily(FONT_FAMILY fontFamily);
	void SetFontCollection(); // Not sure how to do this - m_fontCollection is okay to be nullptr
	void SetFontWeight(DWRITE_FONT_WEIGHT fontWeight);
	void SetFontStyle(DWRITE_FONT_STYLE fontStyle);
	void SetFontStretch(DWRITE_FONT_STRETCH fontStretch);
	void SetFontSize(float fontSize);
	void SetLocale(); // Not sure how to do this - m_locale is "en-US" for all known cases
	void SetTextAlignment(DWRITE_TEXT_ALIGNMENT textAlignment);
	void SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment);
	void SetWordWrapping(DWRITE_WORD_WRAPPING wordWrapping);
	void SetTrimmingGranularity(DWRITE_TRIMMING_GRANULARITY granularity);

	void SetTop(float top) { m_top = top; TextChanged(); }
	void SetLeft(float left) { m_left = left; TextChanged(); }
	void SetHeight(float height) { m_height = height; TextChanged(); }
	void SetWidth(float width) { m_width = width; TextChanged(); }

private:
	void UpdateTextFormat();
	void TextChanged();

	float m_top, m_left, m_height, m_width;

	std::shared_ptr<DeviceResources> m_deviceResources;

	std::wstring m_text;

	DWRITE_TEXT_METRICS								m_textMetrics;
	D2D1::Matrix3x2F								m_screenTranslation;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	m_color;
	FONT_FAMILY										m_fontFamily;
	Microsoft::WRL::ComPtr<IDWriteFontCollection>	m_fontCollection;
	DWRITE_FONT_WEIGHT								m_fontWeight;
	DWRITE_FONT_STYLE								m_fontStyle;
	DWRITE_FONT_STRETCH								m_fontStretch;
	float											m_fontSize;
	std::wstring									m_locale;
	DWRITE_TEXT_ALIGNMENT							m_textAlignment;
	DWRITE_PARAGRAPH_ALIGNMENT						m_paragraphAlignment;
	DWRITE_WORD_WRAPPING							m_wordWrapping;
	DWRITE_TRIMMING									m_trimming;

	Microsoft::WRL::ComPtr<IDWriteTextFormat3>		m_textFormat;
	Microsoft::WRL::ComPtr<IDWriteTextLayout4>		m_textLayout;
};