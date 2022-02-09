#include "Text.h"


Text::Text(std::shared_ptr<DeviceResources> deviceResources) : 
	m_deviceResources(deviceResources),
	m_textMetrics(DWRITE_TEXT_METRICS()),
	m_text(L""),
	m_screenTranslation(D2D1::Matrix3x2F::Translation(0.0f, 0.0f)),
	m_textLayout(nullptr),
	m_color(nullptr),
	m_fontFamily(FONT_FAMILY::SEGOE_UI),
	m_fontCollection(nullptr),
	m_fontWeight(DWRITE_FONT_WEIGHT_LIGHT),
	m_fontStyle(DWRITE_FONT_STYLE_NORMAL),
	m_fontStretch(DWRITE_FONT_STRETCH_NORMAL),
	m_fontSize(12.0f),
	m_locale(L"en-US"),
	m_textAlignment(DWRITE_TEXT_ALIGNMENT_LEADING),
	m_paragraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR),
	m_wordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP),
	m_trimming(DWRITE_TRIMMING()),
	m_top(0.0f),
	m_left(0.0f),
	m_height(100.0f),
	m_width(100.0f)
{
	INFOMAN(m_deviceResources);

	GFX_THROW_INFO(
		m_deviceResources->D2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f), m_color.ReleaseAndGetAddressOf())
	);

	// Create the trimming structure
	ZeroMemory(&m_trimming, sizeof(DWRITE_TRIMMING));
	m_trimming.granularity = DWRITE_TRIMMING_GRANULARITY::DWRITE_TRIMMING_GRANULARITY_NONE;

	ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));

	UpdateTextFormat();

	// Call text changed to initialize the text layout
	TextChanged();
}

void Text::Draw()
{
	INFOMAN(m_deviceResources);
	ID2D1DeviceContext6* context = m_deviceResources->D2DDeviceContext();

	context->SetTransform(m_screenTranslation * m_deviceResources->OrientationTransform2D());

	GFX_THROW_INFO_ONLY(
		context->DrawTextLayout(
			D2D1::Point2F(0.0f, 0.0f),
			m_textLayout.Get(),
			m_color.Get(),
			D2D1_DRAW_TEXT_OPTIONS_CLIP			// <-- investigate these options, clipping is interesting
		)
	);
}

void Text::UpdateTextFormat()
{
	INFOMAN(m_deviceResources);

	Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat = nullptr;
	GFX_THROW_INFO(
		m_deviceResources->DWriteFactory()->CreateTextFormat(
			FontFamily::ToWString(m_fontFamily).c_str(),
			m_fontCollection.Get(),
			m_fontWeight,
			m_fontStyle,
			m_fontStretch,
			m_fontSize,
			m_locale.c_str(),
			textFormat.ReleaseAndGetAddressOf()
		)
	);

	GFX_THROW_INFO(textFormat.As(&m_textFormat));

	GFX_THROW_INFO(m_textFormat->SetTextAlignment(m_textAlignment));
	GFX_THROW_INFO(m_textFormat->SetParagraphAlignment(m_paragraphAlignment));
	GFX_THROW_INFO(m_textFormat->SetWordWrapping(m_wordWrapping));
	GFX_THROW_INFO(m_textFormat->SetTrimming(&m_trimming, nullptr));
}

void Text::SetColor(D2D1_COLOR_F color)
{
	m_deviceResources->D2DDeviceContext()->CreateSolidColorBrush(color, m_color.ReleaseAndGetAddressOf());
	UpdateTextFormat();
}
void Text::SetFontFamily(FONT_FAMILY fontFamily)
{
	m_fontFamily = fontFamily;
	UpdateTextFormat();
}
void Text::SetFontCollection() // Not sure how to do this - m_fontCollection is okay to be nullptr
{
	m_fontCollection = nullptr;
}
void Text::SetFontWeight(DWRITE_FONT_WEIGHT fontWeight)
{
	m_fontWeight = fontWeight;
	UpdateTextFormat();
}
void Text::SetFontStyle(DWRITE_FONT_STYLE fontStyle)
{
	m_fontStyle = fontStyle;
	UpdateTextFormat();
}
void Text::SetFontStretch(DWRITE_FONT_STRETCH fontStretch)
{
	m_fontStretch = fontStretch;
	UpdateTextFormat();
}
void Text::SetFontSize(float fontSize)
{
	m_fontSize = fontSize;
	UpdateTextFormat();
}
void Text::SetLocale() // Not sure how to do this - m_locale is "en-US" for all known cases
{
	m_locale = L"en-US";
	UpdateTextFormat();
}
void Text::SetTextAlignment(DWRITE_TEXT_ALIGNMENT textAlignment)
{
	m_textAlignment = textAlignment;
	UpdateTextFormat();
}
void Text::SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment)
{
	m_paragraphAlignment = paragraphAlignment;
	UpdateTextFormat();
}
void Text::SetWordWrapping(DWRITE_WORD_WRAPPING wordWrapping)
{
	m_wordWrapping = wordWrapping;
	UpdateTextFormat();
}
void Text::SetTrimmingGranularity(DWRITE_TRIMMING_GRANULARITY granularity)
{
	m_trimming.granularity = granularity;
	UpdateTextFormat();
}

void Text::TextChanged()
{
	INFOMAN(m_deviceResources);

	Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout;
	GFX_THROW_INFO(
		m_deviceResources->DWriteFactory()->CreateTextLayout(
			m_text.c_str(),
			(uint32_t)m_text.length(),
			m_textFormat.Get(),
			m_width,
			m_height,
			textLayout.ReleaseAndGetAddressOf()
		)
	);


	GFX_THROW_INFO(textLayout.As(&m_textLayout));

	GFX_THROW_INFO(m_textLayout->GetMetrics(&m_textMetrics));

	// Update Screen Translation ---
	m_screenTranslation = D2D1::Matrix3x2F::Translation(m_left, m_top);
}

void Text::Pop()
{
	if (m_text.size() > 0)
		m_text.pop_back();

	TextChanged();
}