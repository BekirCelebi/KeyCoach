#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <limits>

struct KeyVisual
{
    sf::Keyboard::Key code;   // SFML key code
    sf::FloatRect      rect;  // x, y, w, h (in window coordinates)
    sf::Color          base;  // base color (finger color)
    std::string        label; // text to draw on top (can be empty)
};

// convenience to add a key
KeyVisual makeKey(sf::Keyboard::Key code,
                  float x, float y, float w, float h,
                  const sf::Color& c,
                  const std::string& label)
{
    return {code, sf::FloatRect(x, y, w, h), c, label};
}

int main()
{
    // ------------------------------------------------------------
    // Window setup
    // ------------------------------------------------------------
    const unsigned int WIN_W = 2048;
    const unsigned int WIN_H = 815;

    sf::RenderWindow window(sf::VideoMode(WIN_W, WIN_H), "Typing Trainer");
    window.setFramerateLimit(120);

    sf::View view(sf::FloatRect(0.f, 0.f, static_cast<float>(WIN_W), static_cast<float>(WIN_H)));
    window.setView(view);

    // zoom / slider state
    float zoom = 1.0f;              // 1.0 = normal size
    const float minZoom = 0.6f;     // smallest keyboard
    const float maxZoom = 1.5f;     // biggest keyboard
    bool sliderDragging = false;

    // apply zoom to the view
    auto applyZoom = [&](float newZoom)
    {
        zoom = std::clamp(newZoom, minZoom, maxZoom);
        view.setSize(static_cast<float>(WIN_W) / zoom,
                     static_cast<float>(WIN_H) / zoom);
        view.setCenter(static_cast<float>(WIN_W) / 2.f,
                       static_cast<float>(WIN_H) / 2.f);
        window.setView(view);
    };

    // initial zoom
    applyZoom(zoom);

    // ------------------------------------------------------------
    // Colors (approximate to your screenshot)
    // each color = a finger (darker, less vibrant)
    // ------------------------------------------------------------
    sf::Color leftPinky  ( 52,  91,  58);   // green
    sf::Color leftRing   ( 46,  78, 136);   // blue-ish
    sf::Color leftMiddle ( 84,  46, 110);   // purple
    sf::Color leftIndex  (124, 110,  39);   // yellow
    sf::Color rightIndex (124,  84,  26);   // orange
    sf::Color rightMiddle(117,  52,  52);   // red
    sf::Color rightRing  ( 39, 104, 104);   // teal
    sf::Color rightPinky ( 78,  78,  78);   // dark gray (modifiers)

    // space bar & neutral keys
    sf::Color neutral    ( 60,  60,  60);

    // highlight overlay (a bit stronger so presses really pop)
    sf::Color highlight(255, 255, 255, 150);

    // ------------------------------------------------------------
    // Layout numbers – all keys are built from these
    // ------------------------------------------------------------
    const float marginTop    = 80.f;
    const float marginLeft   = 80.f;
    const float keyW         = 110.f;   // normal key width
    const float keyH         = 110.f;   // normal key height
    const float gapX         = 10.f;    // horizontal gap
    const float gapY         = 18.f;    // vertical gap

    auto rowY = [&](int row) {
        return marginTop + row * (keyH + gapY);
    };

    auto colX = [&](int col) {
        return marginLeft + col * (keyW + gapX);
    };

    // ------------------------------------------------------------
    // Build all keys
    // ------------------------------------------------------------
    std::vector<KeyVisual> keys;

    // ---------- Row 0: ` 1 2 3 4 5 6 7 8 9 0 - = Back ----------
    float y0 = rowY(0);
    float x0 = marginLeft;

    // your tuned width for ` key (0.7 * keyW)
    keys.push_back(makeKey(sf::Keyboard::Tilde, x0, y0,
                           keyW * 0.7f, keyH, leftPinky, "`"));
    x0 += keyW * 0.7f + gapX;

    // keep all your normal key widths
    keys.push_back(makeKey(sf::Keyboard::Num1,  x0, y0, keyW, keyH, leftPinky,  "1"));
    x0 += keyW + gapX;
    keys.push_back(makeKey(sf::Keyboard::Num2,  x0, y0, keyW, keyH, leftRing,   "2"));
    x0 += keyW + gapX;
    keys.push_back(makeKey(sf::Keyboard::Num3,  x0, y0, keyW, keyH, leftMiddle, "3"));
    x0 += keyW + gapX;
    keys.push_back(makeKey(sf::Keyboard::Num4,  x0, y0, keyW, keyH, leftIndex,  "4"));
    x0 += keyW + gapX;
    keys.push_back(makeKey(sf::Keyboard::Num5,  x0, y0, keyW, keyH, leftIndex,  "5"));
    x0 += keyW + gapX;

    keys.push_back(makeKey(sf::Keyboard::Num6,  x0, y0, keyW, keyH, rightIndex, "6"));
    x0 += keyW + gapX;
    keys.push_back(makeKey(sf::Keyboard::Num7,  x0, y0, keyW, keyH, rightIndex, "7"));
    x0 += keyW + gapX;
    keys.push_back(makeKey(sf::Keyboard::Num8,  x0, y0, keyW, keyH, rightMiddle,"8"));
    x0 += keyW + gapX;
    keys.push_back(makeKey(sf::Keyboard::Num9,  x0, y0, keyW, keyH, rightRing,  "9"));
    x0 += keyW + gapX;
    keys.push_back(makeKey(sf::Keyboard::Num0,  x0, y0, keyW, keyH, rightPinky, "0"));
    x0 += keyW + gapX;
    keys.push_back(makeKey(sf::Keyboard::Hyphen,x0, y0, keyW, keyH, rightPinky, "-"));
    x0 += keyW + gapX;
    keys.push_back(makeKey(sf::Keyboard::Equal, x0, y0, keyW, keyH, rightPinky, "="));
    x0 += keyW + gapX;

    // Backspace is wider (2 units)
    keys.push_back(makeKey(sf::Keyboard::Backspace, x0, y0,
                           keyW * 1.2f + gapX, keyH, rightPinky, "Back"));

    // ---------- Row 1: Tab Q W E R T Y U I O P [ ] \ ----------
    float y1 = rowY(1);
    int col = 0;

    keys.push_back(makeKey(sf::Keyboard::Tab, colX(col), y1,
                           keyW * 1.2f, keyH, leftPinky, "Tab"));
    // use actual Tab width (1.2 * keyW) so there is no extra gap
    float curX = colX(col) + keyW * 1.2f + gapX;
    int logicalCol = 1;

    auto pushKeyRow1 = [&](sf::Keyboard::Key code, const std::string& name,
                           const sf::Color& color)
    {
        keys.push_back(makeKey(code,
                               curX + (logicalCol-1) * (keyW + gapX),
                               y1, keyW, keyH, color, name));
        logicalCol++;
    };

    pushKeyRow1(sf::Keyboard::Q, "Q", leftPinky);
    pushKeyRow1(sf::Keyboard::W, "W", leftRing);
    pushKeyRow1(sf::Keyboard::E, "E", leftMiddle);
    pushKeyRow1(sf::Keyboard::R, "R", leftIndex);
    pushKeyRow1(sf::Keyboard::T, "T", leftIndex);
    pushKeyRow1(sf::Keyboard::Y, "Y", rightIndex);
    pushKeyRow1(sf::Keyboard::U, "U", rightIndex);
    pushKeyRow1(sf::Keyboard::I, "I", rightMiddle);
    pushKeyRow1(sf::Keyboard::O, "O", rightRing);
    pushKeyRow1(sf::Keyboard::P, "P", rightPinky);
    pushKeyRow1(sf::Keyboard::LBracket, "[", rightPinky);
    pushKeyRow1(sf::Keyboard::RBracket, "]", rightPinky);

    float backslashX = curX + 12.f * (keyW + gapX);  // column right after ']'
    keys.push_back(makeKey(sf::Keyboard::BackSlash,
                           backslashX, y1,
                           keyW * .8f, keyH, rightPinky, "\\"));

    // ---------- Row 2: Caps A S D F G H J K L ; ' Enter ----------
    float y2 = rowY(2);
    col = 0;

    keys.push_back(makeKey(sf::Keyboard::Unknown, colX(col), y2,
                           keyW * 1.5f, keyH, leftPinky, "Caps"));
    // use actual Caps width (1.5 * keyW)
    curX = colX(col) + keyW * 1.5f + gapX;
    logicalCol = 1;

    auto pushKeyRow2 = [&](sf::Keyboard::Key code, const std::string& name,
                           const sf::Color& color)
    {
        keys.push_back(makeKey(code,
                               curX + (logicalCol-1) * (keyW + gapX),
                               y2, keyW, keyH, color, name));
        logicalCol++;
    };

    pushKeyRow2(sf::Keyboard::A, "A", leftPinky);
    pushKeyRow2(sf::Keyboard::S, "S", leftRing);
    pushKeyRow2(sf::Keyboard::D, "D", leftMiddle);
    pushKeyRow2(sf::Keyboard::F, "F", leftIndex);
    pushKeyRow2(sf::Keyboard::G, "G", leftIndex);
    pushKeyRow2(sf::Keyboard::H, "H", rightIndex);
    pushKeyRow2(sf::Keyboard::J, "J", rightIndex);
    pushKeyRow2(sf::Keyboard::K, "K", rightMiddle);
    pushKeyRow2(sf::Keyboard::L, "L", rightRing);
    pushKeyRow2(sf::Keyboard::SemiColon, ";", rightPinky);
    pushKeyRow2(sf::Keyboard::Apostrophe, "'", rightPinky);

    float enterX = curX + 11.f * (keyW + gapX);  // column right after '''
    keys.push_back(makeKey(sf::Keyboard::Enter,
                           enterX, y2,
                           keyW * 1.6f, keyH, rightPinky, "Enter"));

    // ---------- Row 3: Shift Z X C V B N M , . / Shift ----------
    float y3 = rowY(3);
    col = 0;

    // left Shift (big)
    keys.push_back(makeKey(sf::Keyboard::LShift, colX(col), y3,
                           keyW * 2.1f, keyH, leftPinky, "Shift"));

    // use actual Shift width (2.1 * keyW)
    curX = colX(col) + keyW * 2.1f + gapX;
    logicalCol = 1;

    auto pushKeyRow3 = [&](sf::Keyboard::Key code, const std::string& name,
                           const sf::Color& color)
    {
        keys.push_back(makeKey(code,
                               curX + (logicalCol-1) * (keyW + gapX),
                               y3, keyW, keyH, color, name));
        logicalCol++;
    };

    pushKeyRow3(sf::Keyboard::Z, "Z", leftPinky);
    pushKeyRow3(sf::Keyboard::X, "X", leftRing);
    pushKeyRow3(sf::Keyboard::C, "C", leftMiddle);
    pushKeyRow3(sf::Keyboard::V, "V", leftIndex);
    pushKeyRow3(sf::Keyboard::B, "B", leftIndex);
    pushKeyRow3(sf::Keyboard::N, "N", rightIndex);
    pushKeyRow3(sf::Keyboard::M, "M", rightIndex);
    pushKeyRow3(sf::Keyboard::Comma, ",", rightMiddle);
    pushKeyRow3(sf::Keyboard::Period, ".", rightRing);
    pushKeyRow3(sf::Keyboard::Slash, "/", rightPinky);

    float rightShiftX = curX + 10.f * (keyW + gapX);  // column right after '/'
    keys.push_back(makeKey(sf::Keyboard::RShift,
                           rightShiftX, y3,
                           keyW * 2.1f, keyH, rightPinky, "Shift"));

    // ---------- Row 4: Ctrl Fn Win Alt SPACE Alt WinAI Arrows ----------
    float y4 = rowY(4);
    col = 0;

    // left modifier cluster – all exactly keyW wide
    float x = marginLeft;

    keys.push_back(makeKey(sf::Keyboard::LControl, x, y4,
                           keyW, keyH, leftPinky, "Ctrl"));
    x += keyW + gapX;

    keys.push_back(makeKey(sf::Keyboard::Unknown, x, y4,
                           keyW, keyH, leftPinky, "Fn"));
    x += keyW + gapX;

    keys.push_back(makeKey(sf::Keyboard::LSystem, x, y4,
                           keyW, keyH, leftPinky, "Win"));
    x += keyW + gapX;

    keys.push_back(makeKey(sf::Keyboard::LAlt, x, y4,
                           keyW, keyH, leftRing, "Alt"));
    x += keyW + gapX;

    // spacebar that ends under the M key
    float spaceX = x;

    // right edge of M key (tuned to your layout)
    float mRight = marginLeft + 9.1f * keyW + 7.f * gapX;
    float spaceW = mRight - spaceX;

    keys.push_back(makeKey(sf::Keyboard::Space, spaceX, y4,
                           spaceW, keyH, neutral, ""));  // no label

    // right side cluster
    x = mRight + gapX;

    // right Alt – regular width, red
    keys.push_back(makeKey(sf::Keyboard::RAlt, x, y4,
                           keyW, keyH, rightMiddle, "Alt"));
    x += keyW + gapX;

    // Windows AI button – regular width, blank
    keys.push_back(makeKey(sf::Keyboard::Menu, x, y4,
                           keyW, keyH, rightRing, ""));
    x += keyW + gapX;

    // arrow keys
    float arrowW = keyW;
    float arrowH = keyH;

    // Left arrow – full block
    keys.push_back(makeKey(sf::Keyboard::Left, x, y4 + arrowH * 0.5f,
                           arrowW, arrowH * 0.5f, rightPinky, ""));
    x += arrowW + gapX;

    // Up + Down share middle block
    keys.push_back(makeKey(sf::Keyboard::Up, x, y4,
                           arrowW, arrowH * 0.5f, rightPinky, ""));
    keys.push_back(makeKey(sf::Keyboard::Down, x, y4 + arrowH * 0.5f,
                           arrowW, arrowH * 0.5f, rightPinky, ""));
    x += arrowW + gapX;

    // Right arrow – full block
    keys.push_back(makeKey(sf::Keyboard::Right, x, y4 + arrowH * 0.5f,
                           arrowW, arrowH * 0.5f, rightPinky, ""));

    // ------------------------------------------------------------
    // Center the whole keyboard in the window
    // ------------------------------------------------------------
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();

    for (const auto& kv : keys)
    {
        minX = std::min(minX, kv.rect.left);
        minY = std::min(minY, kv.rect.top);
        maxX = std::max(maxX, kv.rect.left + kv.rect.width);
        maxY = std::max(maxY, kv.rect.top + kv.rect.height);
    }

    float keyboardWidth  = maxX - minX;
    float keyboardHeight = maxY - minY;

    float dx = (static_cast<float>(WIN_W) - keyboardWidth)  / 2.f - minX;
    float dy = (static_cast<float>(WIN_H) - keyboardHeight) / 2.f - minY;

    for (auto& kv : keys)
    {
        kv.rect.left += dx;
        kv.rect.top  += dy;
    }

    // slider geometry in screen space, anchored to bottom-right of the window
auto getSliderRect = [&]() -> sf::FloatRect
{
    sf::Vector2u size = window.getSize();

    float barWidth  = 220.f;
    float barHeight = 24.f;
    float margin    = 20.f;

    float left = static_cast<float>(size.x) - barWidth  - margin;
    float top  = static_cast<float>(size.y) - barHeight - margin;

    return sf::FloatRect(left, top, barWidth, barHeight);
};
    // ------------------------------------------------------------
    // Font for key labels
    // ------------------------------------------------------------
    sf::Font font;
    if (!font.loadFromFile("DejaVuSans.ttf"))
    {
        return 1;
    }

    // keep track of currently pressed keys
    std::unordered_set<sf::Keyboard::Key> pressed;

// ------------------------------------------------------------
// Main loop
// ------------------------------------------------------------
while (window.isOpen())
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        // close window
        if (event.type == sf::Event::Closed)
            window.close();

        // keep aspect ratio (no stretching when resizing)
        if (event.type == sf::Event::Resized)
        {
            float windowRatio = static_cast<float>(event.size.width) /
                                static_cast<float>(event.size.height);
            float viewRatio = static_cast<float>(WIN_W) /
                              static_cast<float>(WIN_H);

            float sizeX = 1.f;
            float sizeY = 1.f;
            float posX = 0.f;
            float posY = 0.f;

            if (windowRatio > viewRatio)
            {
                // window is wider than our view
                sizeX = viewRatio / windowRatio;
                posX = (1.f - sizeX) / 2.f;
            }
            else
            {
                // window is taller than our view
                sizeY = windowRatio / viewRatio;
                posY = (1.f - sizeY) / 2.f;
            }

            view.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));
            window.setView(view);
        }

        // mouse down on slider
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            sf::View uiView = window.getDefaultView();
            sf::Vector2f mouse = window.mapPixelToCoords(
                { event.mouseButton.x, event.mouseButton.y }, uiView);

            sf::FloatRect bar = getSliderRect();
            if (bar.contains(mouse))
            {
                sliderDragging = true;
                float t = (mouse.x - bar.left) / bar.width;
                float newZoom = minZoom + (maxZoom - minZoom) *
                                             std::clamp(t, 0.f, 1.f);
                applyZoom(newZoom);
            }
        }

        // mouse up – stop dragging
        if (event.type == sf::Event::MouseButtonReleased &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            sliderDragging = false;
        }

        // mouse move while dragging – update zoom
        if (event.type == sf::Event::MouseMoved && sliderDragging)
        {
            sf::View uiView = window.getDefaultView();
            sf::Vector2f mouse = window.mapPixelToCoords(
                { event.mouseMove.x, event.mouseMove.y }, uiView);

            sf::FloatRect bar = getSliderRect();
            float t = (mouse.x - bar.left) / bar.width;
            float newZoom = minZoom + (maxZoom - minZoom) *
                                         std::clamp(t, 0.f, 1.f);
            applyZoom(newZoom);
        }

        // key press / release for highlight
        if (event.type == sf::Event::KeyPressed)
        {
            pressed.insert(event.key.code);
        }
        else if (event.type == sf::Event::KeyReleased)
        {
            pressed.erase(event.key.code);
        }
    }

    window.clear(sf::Color(25, 25, 25)); // dark background

    // 1) Draw keyboard using the main view (with zoom)
    window.setView(view);

    for (const auto& kv : keys)
    {
        sf::RectangleShape rect;
        rect.setPosition(kv.rect.left, kv.rect.top);
        rect.setSize({kv.rect.width, kv.rect.height});
        rect.setFillColor(kv.base);
        rect.setOutlineThickness(3.f);
        rect.setOutlineColor(sf::Color(0, 0, 0));
        window.draw(rect);

        // highlight if pressed
        if (pressed.count(kv.code))
        {
            sf::RectangleShape hi;
            hi.setPosition(kv.rect.left, kv.rect.top);
            hi.setSize({kv.rect.width, kv.rect.height});
            hi.setFillColor(highlight);
            window.draw(hi);
        }

        // draw label (if any)
        if (!kv.label.empty())
        {
            sf::Text text;
            text.setFont(font);
            text.setString(kv.label);
            text.setCharacterSize(40);
            text.setFillColor(sf::Color::Black);

            sf::FloatRect tb = text.getLocalBounds();
            text.setOrigin(tb.left + tb.width / 2.f,
                           tb.top + tb.height / 2.f);
            text.setPosition(kv.rect.left + kv.rect.width / 2.f,
                             kv.rect.top + kv.rect.height / 2.f);
            window.draw(text);
        }
    }

    // 2) Draw slider in screen space (bottom-right)
    window.setView(window.getDefaultView());

    sf::FloatRect bar = getSliderRect();

    // slider bar
    sf::RectangleShape barShape;
    barShape.setPosition(bar.left, bar.top + bar.height / 2.f - 4.f);
    barShape.setSize({ bar.width, 8.f });
    barShape.setFillColor(sf::Color(60, 60, 60));
    window.draw(barShape);

    // slider handle – position corresponds to zoom
    float t = (zoom - minZoom) / (maxZoom - minZoom);
    float handleX = bar.left + t * bar.width;

    sf::RectangleShape handle;
    handle.setSize({ 18.f, bar.height });
    handle.setOrigin(handle.getSize().x / 2.f, handle.getSize().y / 2.f);
    handle.setPosition(handleX, bar.top + bar.height / 2.f);
    handle.setFillColor(sf::Color(200, 200, 200));
    window.draw(handle);

    window.display();
}

    return 0;
}