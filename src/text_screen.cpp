#include <color/color.hpp>
#include <font/font.hpp>
#include <text_screen/text_screen.hpp>

namespace system::io {

entity::TextScreen::TextScreen(system::media::entity::Color defaultColor,
                               system::media::entity::Font  defaultFont)
    : defaultColor(defaultColor), defaultFont(defaultFont),
      row(height / system::media::entity::Font::fontHeight),
      column(width / system::media::entity::Font::fontWidth) {}

entity::TextScreen *
entity::TextScreen::drawChar(const uint32_t &x, const uint32_t &y,
                             const uint8_t &                     ch,
                             const system::media::entity::Color &color,
                             const system::media::entity::Font & font) {

    const uint8_t *charMap = font.getCharMap(ch);

    uint32_t xBoundary = x + system::media::entity::Font::fontWidth,
             yBoundary = y + system::media::entity::Font::fontHeight;

    for(uint32_t _y = y, fontMapRowIndex = 0; _y < yBoundary;
        _y++, fontMapRowIndex++) {
        uint8_t fontMapRow = charMap[fontMapRowIndex];
        for(uint32_t _x = x; _x < xBoundary; _x++) {
            if(fontMapRow & 0b10000000) {
                drawPixel(_x, _y, color);
            }
            fontMapRow = fontMapRow << 1;
        }
    }

    return this;
}

entity::TextScreen *entity::TextScreen::print(const uint8_t *content) {
    int i = 0;
    while(content[i] != '\0') {
        buffer[bufferCount] = content[i];
        bufferCount++;
        i++;
    }

    return this;
}

entity::TextScreen *entity::TextScreen::print(int64_t content) {
    uint8_t arr[64] = {}; // length is long enought now

    if(content == 0) {
        arr[0] = '0';
        return print(arr);
    }

    int64_t tmp = content, length = 0;
    while(tmp != 0) {
        tmp /= 10;
        length++;
    }

    int index = length - 1, end = 0;
    if(content < 0) {
        arr[0]  = '-';
        content = -content;
        index++;
        end++;
    }

    while(index >= end) {
        arr[index] = '0' + content % 10;
        content /= 10;
        index--;
    }

    return print(arr);
}

entity::TextScreen *entity::TextScreen::print(uint64_t content) {
    uint8_t arr[64] = {}; // length is long enought now

    if(content == 0) {
        arr[0] = '0';
        return print(arr);
    }

    uint64_t tmp = content, length = 0;
    while(tmp != 0) {
        tmp /= 10;
        length++;
    }

    int index = length - 1, end = 0;
    if(content < 0) {
        arr[0]  = '-';
        content = -content;
        index++;
        end++;
    }

    while(index >= end) {
        arr[index] = '0' + content % 10;
        content /= 10;
        index--;
    }

    return print(arr);
}

void entity::TextScreen::fresh() {
    int cursor = 0;
    for(int i = 0; i < bufferCount; i++) {
        if(cursor >= row * column) {
            clear();
            cursor = 0;
        }
        switch(buffer[i]) {
            case '\n': {
                cursor += column - cursor % column;
                break;
            }
            case '\r': {
                cursor += column - cursor % column;
                break;
            }
            case '\b': {
                if(cursor > 0) {
                    int x = (cursor % column)
                            * system::media::entity::Font::fontWidth;
                    int y = (cursor / column)
                            * system::media::entity::Font::fontHeight;
                    drawChar(x, y, ' ', defaultColor, defaultFont);
                    cursor--;
                }
                break;
            }
            default: {
                int x = (cursor % column)
                        * system::media::entity::Font::fontWidth;
                int y = (cursor / column)
                        * system::media::entity::Font::fontHeight;
                drawChar(x, y, buffer[i], defaultColor, defaultFont);
                cursor++;
                break;
            }
        }
    }

    // draw cursor
    int x = (cursor % column) * system::media::entity::Font::fontWidth;
    int y = (cursor / column) * system::media::entity::Font::fontHeight;
    drawChar(x, y, '_', defaultColor, defaultFont);
}

} // namespace system::io
