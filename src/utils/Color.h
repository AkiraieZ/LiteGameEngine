#pragma once

#include <cstdint>
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace LGE {
namespace Utils {

struct ColorRGB {
    float r, g, b;

    ColorRGB() : r(0.0f), g(0.0f), b(0.0f) {}
    ColorRGB(float r, float g, float b) : r(r), g(g), b(b) {}

    static ColorRGB fromRGB8(uint8_t r, uint8_t g, uint8_t b) {
        return ColorRGB(r / 255.0f, g / 255.0f, b / 255.0f);
    }

    static ColorRGB fromHex(const std::string& hex) {
        if (hex.empty()) return ColorRGB();
        std::string h = hex;
        if (h[0] == '#') h = h.substr(1);
        if (h.length() != 6) throw std::invalid_argument("Invalid hex color length");
        
        uint8_t r = std::stoi(h.substr(0, 2), nullptr, 16);
        uint8_t g = std::stoi(h.substr(2, 2), nullptr, 16);
        uint8_t b = std::stoi(h.substr(4, 2), nullptr, 16);
        return fromRGB8(r, g, b);
    }

    std::string toHex() const {
        std::stringstream ss;
        ss << "#" << std::hex << std::setfill('0')
           << std::setw(2) << static_cast<int>(r * 255)
           << std::setw(2) << static_cast<int>(g * 255)
           << std::setw(2) << static_cast<int>(b * 255);
        return ss.str();
    }

    ColorRGB clamp() const {
        return ColorRGB(
            std::max(0.0f, std::min(1.0f, r)),
            std::max(0.0f, std::min(1.0f, g)),
            std::max(0.0f, std::min(1.0f, b))
        );
    }

    ColorRGB operator+(const ColorRGB& other) const {
        return ColorRGB(r + other.r, g + other.g, b + other.b);
    }

    ColorRGB operator-(const ColorRGB& other) const {
        return ColorRGB(r - other.r, g - other.g, b - other.b);
    }

    ColorRGB operator*(float s) const {
        return ColorRGB(r * s, g * s, b * s);
    }

    ColorRGB operator/(float s) const {
        return ColorRGB(r / s, g / s, b / s);
    }

    static ColorRGB lerp(const ColorRGB& a, const ColorRGB& b, float t) {
        t = std::max(0.0f, std::min(1.0f, t));
        return ColorRGB(
            a.r + (b.r - a.r) * t,
            a.g + (b.g - a.g) * t,
            a.b + (b.b - a.b) * t
        );
    }

    static const ColorRGB White;
    static const ColorRGB Black;
    static const ColorRGB Red;
    static const ColorRGB Green;
    static const ColorRGB Blue;
    static const ColorRGB Yellow;
    static const ColorRGB Cyan;
    static const ColorRGB Magenta;
    static const ColorRGB Gray;
    static const ColorRGB Orange;
    static const ColorRGB Purple;
    static const ColorRGB Pink;
    static const ColorRGB Brown;
};

inline const ColorRGB ColorRGB::White(1.0f, 1.0f, 1.0f);
inline const ColorRGB ColorRGB::Black(0.0f, 0.0f, 0.0f);
inline const ColorRGB ColorRGB::Red(1.0f, 0.0f, 0.0f);
inline const ColorRGB ColorRGB::Green(0.0f, 1.0f, 0.0f);
inline const ColorRGB ColorRGB::Blue(0.0f, 0.0f, 1.0f);
inline const ColorRGB ColorRGB::Yellow(1.0f, 1.0f, 0.0f);
inline const ColorRGB ColorRGB::Cyan(0.0f, 1.0f, 1.0f);
inline const ColorRGB ColorRGB::Magenta(1.0f, 0.0f, 1.0f);
inline const ColorRGB ColorRGB::Gray(0.5f, 0.5f, 0.5f);
inline const ColorRGB ColorRGB::Orange(1.0f, 0.5f, 0.0f);
inline const ColorRGB ColorRGB::Purple(0.5f, 0.0f, 0.5f);
inline const ColorRGB ColorRGB::Pink(1.0f, 0.5f, 0.8f);
inline const ColorRGB ColorRGB::Brown(0.6f, 0.3f, 0.0f);

struct ColorRGBA {
    float r, g, b, a;

    ColorRGBA() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
    ColorRGBA(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
    ColorRGBA(const ColorRGB& rgb, float a = 1.0f) : r(rgb.r), g(rgb.g), b(rgb.b), a(a) {}

    static ColorRGBA fromRGBA8(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        return ColorRGBA(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }

    static ColorRGBA fromHex(const std::string& hex) {
        if (hex.empty()) return ColorRGBA();
        std::string h = hex;
        if (h[0] == '#') h = h.substr(1);
        if (h.length() == 6) {
            uint8_t r = std::stoi(h.substr(0, 2), nullptr, 16);
            uint8_t g = std::stoi(h.substr(2, 2), nullptr, 16);
            uint8_t b = std::stoi(h.substr(4, 2), nullptr, 16);
            return fromRGBA8(r, g, b);
        } else if (h.length() == 8) {
            uint8_t r = std::stoi(h.substr(0, 2), nullptr, 16);
            uint8_t g = std::stoi(h.substr(2, 2), nullptr, 16);
            uint8_t b = std::stoi(h.substr(4, 2), nullptr, 16);
            uint8_t a = std::stoi(h.substr(6, 2), nullptr, 16);
            return fromRGBA8(r, g, b, a);
        }
        throw std::invalid_argument("Invalid hex color length");
    }

    std::string toHex() const {
        std::stringstream ss;
        ss << "#" << std::hex << std::setfill('0')
           << std::setw(2) << static_cast<int>(r * 255)
           << std::setw(2) << static_cast<int>(g * 255)
           << std::setw(2) << static_cast<int>(b * 255)
           << std::setw(2) << static_cast<int>(a * 255);
        return ss.str();
    }

    ColorRGBA clamp() const {
        return ColorRGBA(
            std::max(0.0f, std::min(1.0f, r)),
            std::max(0.0f, std::min(1.0f, g)),
            std::max(0.0f, std::min(1.0f, b)),
            std::max(0.0f, std::min(1.0f, a))
        );
    }

    ColorRGBA operator+(const ColorRGBA& other) const {
        return ColorRGBA(r + other.r, g + other.g, b + other.b, a + other.a);
    }

    ColorRGBA operator-(const ColorRGBA& other) const {
        return ColorRGBA(r - other.r, g - other.g, b - other.b, a - other.a);
    }

    ColorRGBA operator*(float s) const {
        return ColorRGBA(r * s, g * s, b * s, a * s);
    }

    ColorRGBA operator/(float s) const {
        return ColorRGBA(r / s, g / s, b / s, a / s);
    }

    static ColorRGBA lerp(const ColorRGBA& a, const ColorRGBA& b, float t) {
        t = std::max(0.0f, std::min(1.0f, t));
        return ColorRGBA(
            a.r + (b.r - a.r) * t,
            a.g + (b.g - a.g) * t,
            a.b + (b.b - a.b) * t,
            a.a + (b.a - a.a) * t
        );
    }

    ColorRGB toRGB() const { return ColorRGB(r, g, b); }

    static const ColorRGBA White;
    static const ColorRGBA Black;
    static const ColorRGBA Transparent;
    static const ColorRGBA Red;
    static const ColorRGBA Green;
    static const ColorRGBA Blue;
    static const ColorRGBA Yellow;
    static const ColorRGBA Cyan;
    static const ColorRGBA Magenta;
    static const ColorRGBA Gray;
    static const ColorRGBA Orange;
    static const ColorRGBA Purple;
    static const ColorRGBA Pink;
    static const ColorRGBA Brown;
};

inline const ColorRGBA ColorRGBA::White(1.0f, 1.0f, 1.0f, 1.0f);
inline const ColorRGBA ColorRGBA::Black(0.0f, 0.0f, 0.0f, 1.0f);
inline const ColorRGBA ColorRGBA::Transparent(0.0f, 0.0f, 0.0f, 0.0f);
inline const ColorRGBA ColorRGBA::Red(1.0f, 0.0f, 0.0f, 1.0f);
inline const ColorRGBA ColorRGBA::Green(0.0f, 1.0f, 0.0f, 1.0f);
inline const ColorRGBA ColorRGBA::Blue(0.0f, 0.0f, 1.0f, 1.0f);
inline const ColorRGBA ColorRGBA::Yellow(1.0f, 1.0f, 0.0f, 1.0f);
inline const ColorRGBA ColorRGBA::Cyan(0.0f, 1.0f, 1.0f, 1.0f);
inline const ColorRGBA ColorRGBA::Magenta(1.0f, 0.0f, 1.0f, 1.0f);
inline const ColorRGBA ColorRGBA::Gray(0.5f, 0.5f, 0.5f, 1.0f);
inline const ColorRGBA ColorRGBA::Orange(1.0f, 0.5f, 0.0f, 1.0f);
inline const ColorRGBA ColorRGBA::Purple(0.5f, 0.0f, 0.5f, 1.0f);
inline const ColorRGBA ColorRGBA::Pink(1.0f, 0.5f, 0.8f, 1.0f);
inline const ColorRGBA ColorRGBA::Brown(0.6f, 0.3f, 0.0f, 1.0f);

struct ColorHSV {
    float h, s, v;

    ColorHSV() : h(0.0f), s(0.0f), v(0.0f) {}
    ColorHSV(float h, float s, float v) : h(h), s(s), v(v) {}

    static ColorHSV fromRGB(const ColorRGB& rgb) {
        float r = rgb.r, g = rgb.g, b = rgb.b;
        float max = std::max({r, g, b});
        float min = std::min({r, g, b});
        float delta = max - min;

        ColorHSV hsv;
        hsv.v = max;
        hsv.s = (max > 0.001f) ? (delta / max) : 0.0f;

        if (delta > 0.001f) {
            if (r >= g && r >= b) {
                hsv.h = 60.0f * std::fmod(((g - b) / delta), 6.0f);
            } else if (g >= r && g >= b) {
                hsv.h = 60.0f * (((b - r) / delta) + 2.0f);
            } else {
                hsv.h = 60.0f * (((r - g) / delta) + 4.0f);
            }
            if (hsv.h < 0.0f) hsv.h += 360.0f;
        } else {
            hsv.h = 0.0f;
        }

        return hsv;
    }

    ColorRGB toRGB() const {
        float c = v * s;
        float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
        float m = v - c;

        float r, g, b;
        if (h >= 0.0f && h < 60.0f) {
            r = c; g = x; b = 0.0f;
        } else if (h >= 60.0f && h < 120.0f) {
            r = x; g = c; b = 0.0f;
        } else if (h >= 120.0f && h < 180.0f) {
            r = 0.0f; g = c; b = x;
        } else if (h >= 180.0f && h < 240.0f) {
            r = 0.0f; g = x; b = c;
        } else if (h >= 240.0f && h < 300.0f) {
            r = x; g = 0.0f; b = c;
        } else {
            r = c; g = 0.0f; b = x;
        }

        return ColorRGB(r + m, g + m, b + m);
    }

    ColorHSV clamp() const {
        float h_clamped = std::fmod(h, 360.0f);
        if (h_clamped < 0.0f) h_clamped += 360.0f;
        return ColorHSV(
            h_clamped,
            std::max(0.0f, std::min(1.0f, s)),
            std::max(0.0f, std::min(1.0f, v))
        );
    }
};

struct ColorHSL {
    float h, s, l;

    ColorHSL() : h(0.0f), s(0.0f), l(0.0f) {}
    ColorHSL(float h, float s, float l) : h(h), s(s), l(l) {}

    static ColorHSL fromRGB(const ColorRGB& rgb) {
        float r = rgb.r, g = rgb.g, b = rgb.b;
        float max = std::max({r, g, b});
        float min = std::min({r, g, b});
        float delta = max - min;

        ColorHSL hsl;
        hsl.l = (max + min) / 2.0f;

        if (delta > 0.001f) {
            hsl.s = (hsl.l < 0.5f) ? (delta / (max + min)) : (delta / (2.0f - max - min));

            if (r >= g && r >= b) {
                hsl.h = 60.0f * std::fmod(((g - b) / delta), 6.0f);
            } else if (g >= r && g >= b) {
                hsl.h = 60.0f * (((b - r) / delta) + 2.0f);
            } else {
                hsl.h = 60.0f * (((r - g) / delta) + 4.0f);
            }
            if (hsl.h < 0.0f) hsl.h += 360.0f;
        } else {
            hsl.h = 0.0f;
            hsl.s = 0.0f;
        }

        return hsl;
    }

    ColorRGB toRGB() const {
        auto hue2rgb = [](float p, float q, float t) {
            if (t < 0.0f) t += 1.0f;
            if (t > 1.0f) t -= 1.0f;
            if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
            if (t < 0.5f) return q;
            if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
            return p;
        };

        float h_norm = h / 360.0f;
        float q = (l < 0.5f) ? (l * (1.0f + s)) : (l + s - l * s);
        float p = 2.0f * l - q;

        return ColorRGB(
            hue2rgb(p, q, h_norm + 1.0f / 3.0f),
            hue2rgb(p, q, h_norm),
            hue2rgb(p, q, h_norm - 1.0f / 3.0f)
        );
    }

    ColorHSL clamp() const {
        float h_clamped = std::fmod(h, 360.0f);
        if (h_clamped < 0.0f) h_clamped += 360.0f;
        return ColorHSL(
            h_clamped,
            std::max(0.0f, std::min(1.0f, s)),
            std::max(0.0f, std::min(1.0f, l))
        );
    }
};

class ColorPalette {
public:
    ColorPalette() = default;

    void addColor(const std::string& name, const ColorRGB& color) {
        colors_[name] = color;
    }

    ColorRGB getColor(const std::string& name) const {
        auto it = colors_.find(name);
        if (it != colors_.end()) return it->second;
        return ColorRGB::Black;
    }

    bool hasColor(const std::string& name) const {
        return colors_.find(name) != colors_.end();
    }

    void removeColor(const std::string& name) {
        colors_.erase(name);
    }

    void clear() {
        colors_.clear();
    }

    size_t size() const {
        return colors_.size();
    }

    std::vector<std::string> getColorNames() const {
        std::vector<std::string> names;
        for (const auto& pair : colors_) {
            names.push_back(pair.first);
        }
        return names;
    }

    static ColorPalette createDefaultPalette() {
        ColorPalette palette;
        palette.addColor("white", ColorRGB::White);
        palette.addColor("black", ColorRGB::Black);
        palette.addColor("red", ColorRGB::Red);
        palette.addColor("green", ColorRGB::Green);
        palette.addColor("blue", ColorRGB::Blue);
        palette.addColor("yellow", ColorRGB::Yellow);
        palette.addColor("cyan", ColorRGB::Cyan);
        palette.addColor("magenta", ColorRGB::Magenta);
        palette.addColor("gray", ColorRGB::Gray);
        palette.addColor("orange", ColorRGB::Orange);
        palette.addColor("purple", ColorRGB::Purple);
        palette.addColor("pink", ColorRGB::Pink);
        palette.addColor("brown", ColorRGB::Brown);
        return palette;
    }

    static std::vector<ColorRGB> generateGradient(const ColorRGB& start, const ColorRGB& end, size_t steps) {
        std::vector<ColorRGB> gradient;
        if (steps == 0) return gradient;
        gradient.reserve(steps);
        for (size_t i = 0; i < steps; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(steps - 1);
            gradient.push_back(ColorRGB::lerp(start, end, t));
        }
        return gradient;
    }

    static std::vector<ColorRGB> generateRainbow(size_t steps, float saturation = 1.0f, float value = 1.0f) {
        std::vector<ColorRGB> rainbow;
        if (steps == 0) return rainbow;
        rainbow.reserve(steps);
        for (size_t i = 0; i < steps; ++i) {
            float h = 360.0f * static_cast<float>(i) / static_cast<float>(steps);
            ColorHSV hsv(h, saturation, value);
            rainbow.push_back(hsv.toRGB());
        }
        return rainbow;
    }

    static std::vector<ColorRGB> generateComplementary(const ColorRGB& base, size_t count = 2) {
        std::vector<ColorRGB> colors;
        colors.reserve(count);
        ColorHSV baseHsv = ColorHSV::fromRGB(base);
        for (size_t i = 0; i < count; ++i) {
            float h = std::fmod(baseHsv.h + 180.0f * (i + 1), 360.0f);
            ColorHSV hsv(h, baseHsv.s, baseHsv.v);
            colors.push_back(hsv.toRGB());
        }
        return colors;
    }

    static std::vector<ColorRGB> generateAnalogous(const ColorRGB& base, size_t count = 3, float angle = 30.0f) {
        std::vector<ColorRGB> colors;
        colors.reserve(count);
        ColorHSV baseHsv = ColorHSV::fromRGB(base);
        float startAngle = -angle * (count - 1) / 2.0f;
        for (size_t i = 0; i < count; ++i) {
            float h = std::fmod(baseHsv.h + startAngle + angle * i, 360.0f);
            if (h < 0.0f) h += 360.0f;
            ColorHSV hsv(h, baseHsv.s, baseHsv.v);
            colors.push_back(hsv.toRGB());
        }
        return colors;
    }

    static std::vector<ColorRGB> generateTriadic(const ColorRGB& base) {
        return generateAnalogous(base, 3, 120.0f);
    }

    static std::vector<ColorRGB> generateTetradic(const ColorRGB& base) {
        return generateAnalogous(base, 4, 90.0f);
    }

    static std::vector<ColorRGB> generateMonochromatic(const ColorRGB& base, size_t count = 5) {
        std::vector<ColorRGB> colors;
        colors.reserve(count);
        ColorHSV baseHsv = ColorHSV::fromRGB(base);
        for (size_t i = 0; i < count; ++i) {
            float v = 0.1f + 0.9f * static_cast<float>(i) / static_cast<float>(count - 1);
            ColorHSV hsv(baseHsv.h, baseHsv.s, v);
            colors.push_back(hsv.toRGB());
        }
        return colors;
    }

private:
    std::unordered_map<std::string, ColorRGB> colors_;
};

}
}
